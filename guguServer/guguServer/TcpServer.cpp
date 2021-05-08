#include"TcpServer.h"

TcpServer::TcpServer()
{
	_msgCount = 0;
	_recvCount = 0;
	_clientCount = 0;
	_sock = INVALID_SOCKET;
}

TcpServer::~TcpServer()
{
	//关闭socket 
	CloseSocket();
}

int TcpServer::InitSocket()
{
#ifdef _WIN32
	//启动windows socket 2,x环境 
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	if (0 != WSAStartup(ver, &dat))
	{
		return -1;//-1为环境错误 
	}
#endif 
	//创建socket 
	if (INVALID_SOCKET != _sock)
	{
		printf("<Socket=%d>关闭连接\n", _sock);
		CloseSocket();//如果之前有连接 就关闭连接 
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		return 0;//0为socket创建错误 
	}
	return 1;
}

int TcpServer::Bind(const char* ip, unsigned short port)
{
	//如果为无效套接字 则初始化 
	if (INVALID_SOCKET == _sock)
	{
		InitSocket();
	}
	//绑定网络端口和IP地址 
	sockaddr_in _myaddr = {};
	_myaddr.sin_family = AF_INET;//IPV4
	_myaddr.sin_port = htons(port);//端口
#ifdef _WIN32
	if (ip)//ip为空则监听所有网卡 
	{
		//_myaddr.sin_addr.S_un.S_addr = inet_addr(ip);//IP 老式写法 VS2019不支持
		inet_pton(AF_INET, ip, &_myaddr.sin_addr.S_un.S_addr);
	}
	else
	{
		_myaddr.sin_addr.S_un.S_addr = INADDR_ANY;//IP
	}
#else
	if (ip)//ip为空则监听所有网卡 
	{
		//_myaddr.sin_addr.S_un.S_addr = inet_addr(ip);//IP 老式写法 VS2019不支持
		inet_pton(AF_INET, ip, &_myaddr.sin_addr.s_addr);
	}
	else
	{
		_myaddr.sin_addr.S_un.S_addr = INADDR_ANY;//IP
	}
#endif
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_myaddr, sizeof(sockaddr_in)))//socket (强制转换)sockaddr结构体 结构体大小 
	{
		printf("绑定失败\n");
		return 0;
	}
	else
	{
		printf("绑定成功\n绑定端口为%d\n", port);
		return 1;
	}
}

int TcpServer::Listen(int n)
{
	//如果为无效套接字 则提示 
	if (INVALID_SOCKET == _sock)
	{
		printf("请先初始化套接字并绑定IP端口\n");
		return 0;
	}
	//监听网络端口
	if (SOCKET_ERROR == listen(_sock, n))//最大连接队列 
	{
		printf("监听失败\n");
		return 0;
	}
	else
	{
		printf("监听成功\n");
		return 1;
	}
}

int TcpServer::Accept()
{
	//等待接收客户端连接
	sockaddr_in clientAddr = {};//新建sockadd结构体接收客户端数据 
	int addr_len = sizeof(sockaddr_in);//获取sockadd结构体长度 
	SOCKET temp_socket = INVALID_SOCKET;//声明客户端套接字 
#ifdef _WIN32	
	temp_socket = accept(_sock, (sockaddr*)&clientAddr, &addr_len);//自身套接字 客户端结构体 结构体大小 	
#else
	temp_socket = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&addr_len);//自身套接字 客户端结构体 结构体大小 
#endif
	if (INVALID_SOCKET == temp_socket)//接收失败 
	{
		printf("<Socket=%d>错误,接受到无效客户端SOCKET\n", temp_socket);
		return 0;
	}
	else
	{
		/*
		新客户端IP地址
		inet_ntoa(clientAddr.sin_addr)
		*/
		//将新的客户端加入动态数组
		AddClientToServer(new ClientSocket(temp_socket));
		return 1;
	}
}

void TcpServer::CloseSocket()
{
	if (INVALID_SOCKET != _sock)
	{
#ifdef _WIN32
		//关闭socket
		closesocket(_sock);
		//清除windows socket 环境 
		WSACleanup();
#else
		//关闭socket/LINUX
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
	}
}

void TcpServer::AddClientToServer(ClientSocket* pClient)
{
	//找出客户端最少的线程 然后加入 
	auto pMinServer = _cellServers[0];
	for (auto pCellServer : _cellServers)
	{
		if (pMinServer->GetClientCount() > pCellServer->GetClientCount())
		{
			pMinServer = pCellServer;
		}
	}
	pMinServer->addClient(pClient);
	//客户端数量++ 
	OnNetJoin(pClient);
}

void TcpServer::Start(int nCellServer)
{
	for (int n = 0; n < nCellServer; n++)
	{
		//线程加入容器 
		auto ser = new CellServer(_sock);
		_cellServers.push_back(ser);
		ser->setEventObj(this);
		ser->Start();
	}
}

bool TcpServer::IsRun()
{
	return _sock != INVALID_SOCKET;
}

bool TcpServer::OnRun()
{
	if (IsRun())
	{
		time4msg();//查看各线程数据信息 
		fd_set fdRead;//建立集合 这里要new一下 要不栈会超16K给warning
		//fd_set fdWrite; 
		//fd_set fdExcept;
		FD_ZERO(&fdRead);//清空集合 
		//FD_ZERO(&fdWrite); 
		//FD_ZERO(&fdExcept); 
		FD_SET(_sock, &fdRead);//放入集合 
		//FD_SET(_sock,&fdWrite); 
		//FD_SET(_sock,&fdExcept);
		timeval s_t = { 0,0 };//select最大响应时间 

		//select函数筛选select 
		int ret = select(_sock + 1, &fdRead, 0, 0, &s_t);
		if (ret < 0)
		{
			printf("select任务结束\n");
			CloseSocket();
			return false;
		}
		if (FD_ISSET(_sock, &fdRead))//获取是否有新socket连接 
		{
			FD_CLR(_sock, &fdRead);//清理
			Accept();//连接 
		}
		return true;
	}
	return false;
}

void TcpServer::time4msg()
{
	auto t1 = _time.GetSecond();
	if (1.0 <= t1)
	{
		//时间间隔  本机socket连接序号  客户端数量  每秒收包数 
		printf("time<%lf>,socket<%d>,clients<%d>,msgCount<%d>,recvCount<%d>\n", t1, _sock, (int)_clientCount, (int)(_msgCount / t1 + 0.5), (int)(_recvCount / t1 + 0.5));
		_msgCount = 0;
		_recvCount = 0;//清零 
		_time.UpDate();
	}
}

void TcpServer::OnNetJoin(ClientSocket* pClient)
{
	_clientCount++;
}

void TcpServer::OnNetLeave(ClientSocket* pClient)
{
	//客户端数量-- 
	_clientCount--;
}

void TcpServer::OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHeader* pHead)
{
	//发包数++ 
	_msgCount++;
}

void TcpServer::OnNetRecv(ClientSocket* pClient)
{
	//收包数 
	_recvCount++;
}