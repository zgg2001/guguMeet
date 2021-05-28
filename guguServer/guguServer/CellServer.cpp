#include"CellServer.h"

CellServer::CellServer(int id)
{
	_id = id;
	_taskServer._id = id;//���Ͷ�id
	_pNetEvent = nullptr;
	//��������� 
	_Recv_buf = new char[RECV_BUFFER_SIZE];
	//�ͻ��˼��������� 
	_maxSock = INVALID_SOCKET;
	FD_ZERO(&_fd_read_bak);
	_client_change = true;
}

CellServer::~CellServer()
{
	printf("CellServer:%d ~CellServer start\n", _id);
	//�ر�socket 
	CloseSocket(); 
	//�ͷŵ�
	delete[] _Recv_buf;
	printf("CellServer:%d ~CellServer end\n", _id);
}

void CellServer::setEventObj(INetEvent* event)
{
	_pNetEvent = event;
}

void CellServer::CloseSocket()
{
	printf("CellServer:%d close start\n", _id);
	//�ر������߳�
	_taskServer.Close();
	//�رս����߳�
	_thread.Close();
	//ɾ���ͻ���map��¼ = ɾ���ͻ��˶��� ��������������close socket
	for (auto iter = _clients.begin(); iter != _clients.end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _clientsBuf.begin(); iter != _clientsBuf.end(); ++iter)
	{
		delete* iter;
	}
	//������� 
	_clients.clear();
	_clientsBuf.clear();
	printf("CellServer:%d close end\n", _id);
}

bool CellServer::OnRun(CellThread* thread)
{
	while (thread->isRun())
	{
		//����������еĿͻ����ݼ�����ʽ���� 
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//���� 
			for (auto client : _clientsBuf)
			{
				_clients[client->GetSockfd()] = client;
				if(_pNetEvent)
					_pNetEvent->OnNetJoin(client);//�ͻ�������++ 
				client->_Serverid = _id;//���������߳�id
			}
			_clientsBuf.clear();
			_client_change = true;
		}
		//���û����Ҫ����Ŀͻ��˾����� 
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//����һ���� 
			std::this_thread::sleep_for(t);
			//����ʱ���
			_oldTime = HBtimer::getNowMillSec();
			continue;
		}
		fd_set fdRead;//�������� 
		FD_ZERO(&fdRead);//��ռ��� 
		//�����ӵĿͻ��� ����read���� 
		if (false == _client_change)
		{
			//û�з����ı� �Ͷ�ȡ���� 
			memcpy(&fdRead, &_fd_read_bak, sizeof(fd_set));
		}
		else
		{
			//������Ϸ����ı� ������¼��һ�� 
			_maxSock = -1;//���socket 
			for (auto iter = _clients.begin(); iter != _clients.end(); ++iter)
			{
				FD_SET(iter->first, &fdRead);
				if (_maxSock < iter->first)
				{
					_maxSock = iter->first;
				}
			}
			//���ݸ��� 
			memcpy(&_fd_read_bak, &fdRead, sizeof(fd_set));
			_client_change = false;
		}
		//select����ɸѡselect 
		timeval t{ 0,1 };//������ ����������ʱ
		int ret = select(_maxSock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			printf("CellServer:%d OnRun select error\n", _id);
			//�������˳�
			thread->Exit();
			return false;
		}
		//��������socket �鿴�Ƿ��д������¼� 
#ifdef _WIN32
		for (int n = 0; n < (int)fdRead.fd_count; n++)
		{
			auto iter = _clients.find(fdRead.fd_array[n]);
			if (iter != _clients.end())
			{
				if (-1 == RecvData(iter->second))
				{
					if (_pNetEvent)//���߳���ɾ���ͻ��� 
					{
						_pNetEvent->OnNetLeave(iter->second);
					}
					delete iter->second;
					_clients.erase(iter);//�Ƴ�
					_client_change = true;//�ͻ����˳� ��Ҫ֪ͨϵͳ����¼��fdset���� 
				}
			}
		}
#else
		std::vector<ClientSocket*> ClientSocket_temp;
		for(auto iter = _clients.begin(); iter != _clients.end(); ++iter)
		{
			if (FD_ISSET(iter->first, &fdRead))
			{
				if (-1 == RecvData(iter->second))//�������� �ͻ����˳��Ļ� 
				{
					if (_pNetEvent)//���߳���ɾ���ͻ��� 
					{
						_pNetEvent->OnNetLeave(iter->second);
					}
					ClientSocket_temp.push_back(iter->second);
					_clients.erase(iter);//�Ƴ�
					_client_change = true;//�ͻ����˳� ��Ҫ֪ͨϵͳ����¼��fdset���� 
				}
			}
		}
		for (auto client : ClientSocket_temp)
		{
			_clients.erase(client->GetSockfd());
			delete client;
		}
#endif // _WIN32
		//printf("����ʱ�䴦������ҵ��\n");
		//����¼�
		CheckTime();
	}
	printf("CellServer:%d OnRun() exit\n", _id);
	return true;
}

void CellServer::CheckTime()
{
	time_t nowTime = HBtimer::getNowMillSec();
	time_t dt = nowTime - _oldTime;
	_oldTime = nowTime;
	for (auto iter = _clients.begin(); iter != _clients.end();)
	{
		//��������Ƿ�ʱ
		if (iter->second->CheckHeart(dt) == true)
		{
			if (_pNetEvent)//���߳���ɾ���ͻ��� 
			{
				_pNetEvent->OnNetLeave(iter->second);
			}
			delete iter->second;
			_clients.erase(iter++);//�Ƴ�
			_client_change = true;//�ͻ����˳� ��Ҫ֪ͨϵͳ����¼��fdset���� 
			continue;
		}
		//����Ƿ񵽶�ʱ������Ϣ
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
	//�������߳� ��ʾ�յ��� 
	_pNetEvent->OnNetRecv(t_client);
	//���տͻ��˷��͵�����  
	int buf_len = recv(t_client->GetSockfd(), _Recv_buf, RECV_BUFFER_SIZE, 0);
	if (buf_len <= 0)
	{
		//printf("�ͻ������˳�\n");
		return -1;
	}
	//�������� (�����յ���Ϣ������)
	//t_client->ResetDtHeart();
	//�����ջ����������ݿ�������Ϣ������ 
	memcpy(t_client->MsgBuf() + t_client->GetLen(), _Recv_buf, buf_len);
	//��Ϣ������������ĩβ���� 
	t_client->SetLen(t_client->GetLen() + buf_len);
	//�ж���Ϣ�����������ݳ����Ƿ���ڵ��ڰ�ͷ���� 
	while (t_client->GetLen() >= sizeof(DataHeader))//����ճ������ 
	{
		//ѡ����ͷ���� 
		DataHeader* header = (DataHeader*)t_client->MsgBuf();
		//�ж���Ϣ�����������ݳ����Ƿ���ڵ��ڱ��ĳ��� �����ٰ����� 
		if (t_client->GetLen() >= header->date_length)
		{
			//�������Ϣ��������ʣ��δ�������ݵĳ���
			int size = t_client->GetLen() - header->date_length;
			//��Ӧ���� 
			NetMsg(header, t_client);
			//����Ϣ������ʣ��δ���������ǰ��
			memcpy(t_client->MsgBuf(), t_client->MsgBuf() + header->date_length, size);
			//��Ϣ������������ĩβǰ��
			t_client->SetLen(size);
		}
		else
		{
			//��Ϣ���������ݲ��� 
			break;
		}
	}
	return 0;
}

void CellServer::NetMsg(DataHeader* pHead, ClientSocket* pClient)
{
	//�������߳� ���Ͱ� 
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
	//���������߳�
	_thread.Start(
		//onCreate
			nullptr,
		//onRun
		[this](CellThread*)
		{
			OnRun(&_thread);
		},
		//onDestory
			nullptr);
	//���������߳� 
	_taskServer.Start();
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
