/*
* 本文件中定义了服务端的基础接口
* 在TcpServer.h中服务端类继承了该接口
* 目前仅定义了四个基础事件
* 2021/4/22
*/
#ifndef _INET_EVENT_H_
#define _INET_EVENT_H_

class ClientSocket;
class CellServer;
struct DataHeader;

//服务端基础接口
class INetEvent
{
public:
	//客户端退出事件 
	virtual void OnNetJoin(ClientSocket* pClient) = 0;
	//客户端退出事件 
	virtual void OnNetLeave(ClientSocket* pClient) = 0;
	//服务端发送消息事件 
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHeader* pHead) = 0;
	//服务端接收消息事件
	virtual void OnNetRecv(ClientSocket* pClient) = 0;
};

#endif
