/*
* ���߳̽��ղ���
* ���ļ���ʵ���߳����Լ�DoTask�ӿ�
* ʹ������̷߳��� ���߳̽������� �������̴߳�����Ϣ
* ����ʵ���˷�������Ľӿڣ�ʹ����ÿͻ��˶����SendData����������Ϣ
* Ŀǰ���õ���select�ṹ δ�����ܳ��������ṹ
* 2021/4/22
* �Ƴ�������࣬����������ݸ�Ϊ��������
* ͨ�����������ķ�ʽ����Ҫִ�е����񣬵���addTask()�������ʱ��ֱ��ʹ��lambda
* 2021/5/4
* �½�CheckTime���� �Լ� ʱ������� ʵ��������⡢��ʱ���ͻ�����Ϣ����
* ����clients�����ݽṹ��vector��Ϊmap ��select�е�FD_ISSET��Ϊmap::find �ӿ����Ч��
* 2021/5/6
*/
#ifndef _CELL_SERVER_H_
#define _CELL_SERVER_H_

#include"INetEvent.h"
#include"CellTask.h"
#include"ClientSocket.h"

#include <vector>
#include <map>

//�߳���
class CellServer
{
public:
	//���� 
	CellServer(SOCKET sock = INVALID_SOCKET);
	//����
	virtual ~CellServer();
	//�����¼� 
	void setEventObj(INetEvent* event);
	//�ر�socket 
	void CloseSocket();
	//�ж��Ƿ����� 
	bool IsRun();
	//��ѯ�Ƿ��д�������Ϣ 
	bool OnRun();
	//��������
	int RecvData(ClientSocket* t_client);//�������� 
	//��Ӧ����
	void NetMsg(DataHeader* pHead, ClientSocket* pClient);
	//���ӿͻ��� 
	void addClient(ClientSocket* client);
	//�����߳�
	void Start();
	//��ȡ���߳��ڿͻ�������
	int GetClientCount() const;
	//�������
	void AddSendTask(ClientSocket* pClient, DataHeader* pHead);
	//����¼�
	void CheckTime();

private:
	//select�Ż�
	SOCKET _maxSock;//���socketֵ 
	fd_set _fd_read_bak;//�����ϱ���
	bool _client_change;//�ͻ��˼���bool true��ʾ�����ı� ������ͳ�� fd_read����

	//��������� 
	char* _Recv_buf;//���ջ����� 
	//socket��� 
	SOCKET _sock;
	//��ʽ�ͻ����� 
	std::map<SOCKET, ClientSocket*> _clients;//����ͻ���
	//�ͻ�������
	std::vector<ClientSocket*> _clientsBuf;
	std::mutex _mutex;//��
	//�߳� 
	std::thread* _pThread;
	//�˳��¼��ӿ� 
	INetEvent* _pNetEvent;
	//�����̶߳��� 
	CellTaskServer _taskServer;

	//�ɵ�ʱ���
	time_t _oldTime = HBtimer::getNowMillSec();
};

#endif
