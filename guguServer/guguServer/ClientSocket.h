/*
* �ͻ����� 
* ����˶�����ÿ����һ���µĿͻ��ˣ������½�һ���ͻ��˶���
* ͨ���ÿͻ��˶�����ͻ��˽��з�����Ϣ�Ȳ���
* Ŀǰ��˵ֻʵ���˶����������ݣ������ͻ�������������Ϣ����һ��Ԥ������Ϊ��ʱ����������Ϣ
* 2021/4/22
* ʵ�ֶ�ʱ����������������⣬����������CellServer�߳��ڵ�CheckTime()�����н���
* 2021/5/6
*/
#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

//socket�������
#ifdef _WIN32
	#define FD_SETSIZE 1024 
	#define WIN32_LEAN_AND_MEAN
	#include<winSock2.h>
	#include<WS2tcpip.h>
	#include<windows.h>
	#include"CellThread.h"
	#pragma comment(lib, "ws2_32.lib")//���Ӵ˶�̬���ӿ� windows���� 
	//���Ӷ�̬�� �˶�̬���ﺬ�� ��ʱ����mytimer �� cmd���� �� ������ʱ����HBtimer
	#include "lib\\DynamicLib\\pch.h"
	#pragma comment(lib, "lib\\DynamicLib\\guguDll.lib")
#ifdef _DEBUG
	//����debug��̬�� �˾�̬���ﺬ��һ���ڴ��
	#include"debugLib\\StaticLib\\Alloctor.h"
	#pragma comment(lib,"debugLib\\StaticLib\\guguAlloc.lib")
#else
	//����release��̬�� �˾�̬���ﺬ��һ���ڴ��
	#include"lib\\StaticLib\\Alloctor.h"
	#pragma comment(lib,"lib\\StaticLib\\guguAlloc.lib")
#endif
#else
	#include<sys/socket.h>
	#include<arpa/inet.h>//selcet
	#include<unistd.h>//uni std
	#include<string.h>

	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR (-1)
#endif

//��������С 
#ifndef RECV_BUFFER_SIZE
	#define RECV_BUFFER_SIZE 4096
	#define SEND_BUFFER_SIZE 512
#endif

//�ͻ�������ʱ�� 20000����
#define CLIENT_HREAT_TIME 20000
//�ͻ��˶�ʱ����ʱ�� 200����
#define CLIENT_AUTOMATIC_SEND_TIME 200

//�ͻ����� 
class ClientSocket
{
public:
	int _id = -1;
	int _Serverid = -1;

public:
	//���� 
	ClientSocket(SOCKET sockfd = INVALID_SOCKET);
	//���� 
	virtual ~ClientSocket();
	//��ȡsocket 
	SOCKET GetSockfd();
	//��ȡ���ջ����� 
	char* MsgBuf();
	//��ȡ���ջ�����β������ 
	int GetLen();
	//���û�����β������
	void SetLen(int len);
	//�������� 
	int SendData(DataHeader* head);
	//����������ʱ(����)
	void ResetDtHeart();
	//����Ƿ�����
	bool CheckHeart(time_t dt);
	//���ö�ʱ���ͼ�ʱ��
	void ResetDtSend();
	//����Ƿ�ö�ʱ����
	bool CheckSend(time_t dt);
	//���ͻ���������������
	int SendAll();

private:
	SOCKET _sockfd;
	//��������� 
	char* _Msg_Recv_buf;//��Ϣ������ 
	int _Len_Recv_buf;//����������β������

	char* _Msg_Send_buf;//��Ϣ���ͻ����� 
	int _Len_Send_buf;//���ͻ���������β������
	//������ʱ��
	time_t _dtHeart;
	//��ʱ���ͼ�ʱ��
	time_t _dtSend;
};

#endif
