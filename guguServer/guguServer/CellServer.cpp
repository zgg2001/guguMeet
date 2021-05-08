#include"CellServer.h"

CellServer::CellServer(SOCKET sock)
{
	_sock = sock;
	_pThread = nullptr;
	_pNetEvent = nullptr;
	//缓冲区相关 
	_Recv_buf = new char[RECV_BUFFER_SIZE];
	//客户端集合情况相关 
	_maxSock = INVALID_SOCKET;
	FD_ZERO(&_fd_read_bak);
	_client_change = true;
}

CellServer::~CellServer()
{
	delete[] _Recv_buf;
	delete _pThread;
	delete _pNetEvent;
	//关闭socket 
	CloseSocket();
	_sock = INVALID_SOCKET;
}

void CellServer::setEventObj(INetEvent* event)
{
	_pNetEvent = event;
}

void CellServer::CloseSocket()
{
	if (INVALID_SOCKET != _sock)
	{
#ifdef _WIN32
		//关闭客户端socket
		for (auto iter = _clients.begin(); iter != _clients.end(); ++iter)
		{
			closesocket(iter->first);
			delete iter->second;
		}
		//关闭socket
		closesocket(_sock);
#else
		//关闭客户端socket
		for (int n = 0; n < _clients.size(); ++n)
		{
			close(_clients[n]->GetSockfd());
			delete _clients[n];
		}
		//关闭socket/LINUX
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
		//清除容器 
		_clients.clear();
		_clientsBuf.clear();
	}
}

bool CellServer::IsRun()
{
	return _sock != INVALID_SOCKET;
}

bool CellServer::OnRun()
{
	while (IsRun())
	{
		//将缓冲队列中的客户数据加入正式队列 
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//上锁 
			for (auto client : _clientsBuf)
			{
				_clients[client->GetSockfd()] = client;
			}
			_clientsBuf.clear();
			_client_change = true;
		}
		//如果没有需要处理的客户端就跳过 
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//休眠一毫秒 
			std::this_thread::sleep_for(t);
			//更新时间戳
			_oldTime = HBtimer::getNowMillSec();
			continue;
		}
		fd_set fdRead;//建立集合 
		FD_ZERO(&fdRead);//清空集合 
		//把连接的客户端 放入read集合 
		if (false == _client_change)
		{
			//没有发生改变 就读取备份 
			memcpy(&fdRead, &_fd_read_bak, sizeof(fd_set));
		}
		else
		{
			//如果集合发生改变 就重新录入一遍 
			_maxSock = -1;//最大socket 
			for (auto iter = _clients.begin(); iter != _clients.end(); ++iter)
			{
				FD_SET(iter->first, &fdRead);
				if (_maxSock < iter->first)
				{
					_maxSock = iter->first;
				}
			}
			//备份更新 
			memcpy(&_fd_read_bak, &fdRead, sizeof(fd_set));
			_client_change = false;
		}
		//select函数筛选select 
		timeval t{ 0,1 };//非阻塞 方便心跳计时
		int ret = select(_maxSock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			printf("select任务结束\n");
			CloseSocket();
			return false;
		}
		//遍历所有socket 查看是否有待处理事件 
#ifdef _WIN32
		for (int n = 0; n < (int)fdRead.fd_count; n++)
		{
			auto iter = _clients.find(fdRead.fd_array[n]);
			if (iter != _clients.end())
			{
				if (-1 == RecvData(iter->second))
				{
					if (_pNetEvent)//主线程中删除客户端 
					{
						_pNetEvent->OnNetLeave(iter->second);
					}
					closesocket(iter->first);
					delete iter->second;
					_clients.erase(iter);//移除
					_client_change = true;//客户端退出 需要通知系统重新录入fdset集合 
				}
			}
		}
#else
		std::vector<ClientSocket*> ClientSocket_temp;
		for(auto iter = _clients.begin(); iter != _clients.end(); ++iter)
		{
			if (FD_ISSET(iter->first, &fdRead))
			{
				if (-1 == RecvData(iter->second))//处理请求 客户端退出的话 
				{
					if (_pNetEvent)//主线程中删除客户端 
					{
						_pNetEvent->OnNetLeave(iter->second);
					}
					ClientSocket_temp.push_back(iter->second);
					_clients.erase(iter);//移除
					_client_change = true;//客户端退出 需要通知系统重新录入fdset集合 
				}
			}
		}
		for (auto client : ClientSocket_temp)
		{
			closesocket(client->GetSockfd());
			_clients.erase(client->GetSockfd());
			delete client;
		}
#endif // _WIN32
		//printf("空闲时间处理其他业务\n");
		//检测事件
		CheckTime();
	}
	return true;
}

void CellServer::CheckTime()
{
	time_t nowTime = HBtimer::getNowMillSec();
	time_t dt = nowTime - _oldTime;
	_oldTime = nowTime;
	for (auto iter = _clients.begin(); iter != _clients.end();)
	{
		//检测心跳是否超时
		if (iter->second->CheckHeart(dt) == true)
		{
			if (_pNetEvent)//主线程中删除客户端 
			{
				_pNetEvent->OnNetLeave(iter->second);
			}
			closesocket(iter->second->GetSockfd());
			delete iter->second;
			_clients.erase(iter++);//移除
			_client_change = true;//客户端退出 需要通知系统重新录入fdset集合 
			continue;
		}
		//检测是否到定时发送消息
		if (iter->second->CheckSend(dt) == true)
		{
			iter->second->SendAll();
			iter->second->ResetDtSend();
		}
		iter++;
	}
}

int CellServer::RecvData(ClientSocket* t_client)
{
	//传给主线程 提示收到包 
	_pNetEvent->OnNetRecv(t_client);
	//接收客户端发送的数据  
	int buf_len = recv(t_client->GetSockfd(), _Recv_buf, RECV_BUFFER_SIZE, 0);
	if (buf_len <= 0)
	{
		//printf("客户端已退出\n");
		return -1;
	}
	//心跳重置 (凡是收到消息就重置)
	//t_client->ResetDtHeart();
	//将接收缓冲区的数据拷贝到消息缓冲区 
	memcpy(t_client->MsgBuf() + t_client->GetLen(), _Recv_buf, buf_len);
	//消息缓冲区的数据末尾后移 
	t_client->SetLen(t_client->GetLen() + buf_len);
	//判断消息缓冲区的数据长度是否大于等于包头长度 
	while (t_client->GetLen() >= sizeof(DataHeader))//处理粘包问题 
	{
		//选出包头数据 
		DataHeader* header = (DataHeader*)t_client->MsgBuf();
		//判断消息缓冲区内数据长度是否大于等于报文长度 避免少包问题 
		if (t_client->GetLen() >= header->date_length)
		{
			//计算出消息缓冲区内剩余未处理数据的长度
			int size = t_client->GetLen() - header->date_length;
			//响应数据 
			NetMsg(header, t_client);
			//将消息缓冲区剩余未处理的数据前移
			memcpy(t_client->MsgBuf(), t_client->MsgBuf() + header->date_length, size);
			//消息缓冲区的数据末尾前移
			t_client->SetLen(size);
		}
		else
		{
			//消息缓冲区数据不足 
			break;
		}
	}
	return 0;
}

void CellServer::NetMsg(DataHeader* pHead, ClientSocket* pClient)
{
	//传给主线程 发送包 
	_pNetEvent->OnNetMsg(this, pClient, pHead);
}

void CellServer::addClient(ClientSocket* client)
{
	std::lock_guard<std::mutex> lock(_mutex);
	//_mutex.lock();
	_clientsBuf.push_back(client);
	//_mutex.unlock();
}

void CellServer::Start()
{
	//mem_fn 将成员函数转为函数对象，使用对象指针或者对象进行绑定  相比mem_fun和mem_fun_ref更加智能 
	_pThread = new std::thread(std::mem_fn(&CellServer::OnRun), this);
	_taskServer.Start();//启动发送线程 
}

int CellServer::GetClientCount() const
{
	return _clients.size() + _clientsBuf.size();
}

void CellServer::AddSendTask(ClientSocket* pClient, DataHeader* pHead)
{
	_taskServer.addTask([pClient,pHead]() 
	{
		pClient->SendData(pHead);
		delete pHead;
	});
}
