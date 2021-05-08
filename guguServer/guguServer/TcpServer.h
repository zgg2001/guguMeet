/*
* 服务端类
* 实现基础的socket连接操作
* 通过start方法生成子线程 监控收包
* 主线程仅进行客户端加入退出监控
* 2021/4/22
*/
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include"CellServer.h"
#include<stdio.h>
#include<atomic>

//服务端类 
class TcpServer : INetEvent
{
public:
	//构造 
	TcpServer();
	//析构 
	virtual ~TcpServer();
	//初始化socket 返回1为正常 
	int InitSocket();
	//绑定IP/端口
	int Bind(const char* ip, unsigned short port);
	//监听端口
	int Listen(int n);
	//接受连接
	int Accept();
	//关闭socket 
	void CloseSocket();
	//添加客户端至服务端  
	void AddClientToServer(ClientSocket* pClient);
	//线程启动 
	void Start(int nCellServer);
	//判断是否工作中 
	inline bool IsRun();
	//查询是否有待处理消息 
	bool OnRun();
	//显示各线程数据信息 
	void time4msg();
	//客户端加入事件 
	virtual void OnNetJoin(ClientSocket* pClient);
	//客户端退出
	virtual void OnNetLeave(ClientSocket* pClient);
	//客户端发送消息事件
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHeader* pHead);
	virtual void OnNetRecv(ClientSocket* pClient);

private:
	//socket相关 
	SOCKET _sock;
	std::vector<CellServer*> _cellServers;//线程处理 
	//计时器
	mytimer _time;
	//发送包的数量
	std::atomic_int _msgCount;
	//接收包的数量
	std::atomic_int _recvCount;
	//客户端计数
	std::atomic_int _clientCount;
};

#endif
