#include"TcpServer.h"
class MyServer : public TcpServer
{
public:
	//客户端加入事件 
	virtual void OnNetJoin(ClientSocket* pClient)
	{
		TcpServer::OnNetJoin(pClient);
	}

	//客户端退出
	virtual void OnNetLeave(ClientSocket* pClient)
	{
		TcpServer::OnNetLeave(pClient);
	}

	//服务端发送消息事件
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHeader* pHead)
	{
		//发包数++ 
		TcpServer::OnNetMsg(pCellServer, pClient, pHead);
		//printf("接收到包头，命令：%d，数据长度：%d\n",head->cmd,head->date_length);
		switch (pHead->cmd)
		{
		case CMD_LOGIN://登录 接收登录包体 
		{
			Login* login = (Login*)pHead;
			/*处理*/
			LoginResult* result = new LoginResult;
			result->Result = 1;
			pCellServer->AddSendTask(pClient, result);
		}
		break;
		case CMD_LOGOUT://登出 接收登出包体 
		{
			Logout* logout = (Logout*)pHead;
			/*处理*/
			LogoutResult* result = new LogoutResult();
			result->Result = 1;
			pCellServer->AddSendTask(pClient, result);
		}
		break;
		case CMD_C2S_HEART://心跳包体 
		{
			//重置心跳
			pClient->ResetDtHeart();
			//发送心跳回文
			S2C_Heart* result = new S2C_Heart();
			pCellServer->AddSendTask(pClient, result);
		}
		break;
		default://错误 
		{
			/*处理*/
			pHead->cmd = CMD_ERROR;
			pHead->date_length = 0;
			pCellServer->AddSendTask(pClient, pHead);
		}
		break;
		}
	}

	//服务端接收消息事件
	virtual void OnNetRecv(ClientSocket* pClient)
	{
		//收包数++ 
		TcpServer::OnNetRecv(pClient);
	}
};

int main()
{
	printf("Welcome\n");

	MyServer* s1 = new MyServer();

	s1->InitSocket();
	s1->Bind(NULL, 8888);
	s1->Listen(5);
	s1->Start(4);
	while (s1->IsRun())
	{
		s1->OnRun();
	}
	s1->CloseSocket();
	return 0;
}