/*
�̷߳�װ��
2021/5/27
*/
#ifndef _CELL_THREAD_H_
#define _CELL_THREAD_H_

#include<functional>
#include"CellSemaphore.h"

class CellThread
{
private:
	typedef std::function<void(CellThread*)> EventCall;

public:
	//�����߳�
	void Start(EventCall onCreate = nullptr, EventCall onRun = nullptr, EventCall onDestory = nullptr);
	//�ر��߳�
	void Close();
	//�������˳�
	void Exit();
	//�Ƿ�������
	bool isRun();

protected:
	//��������
	void OnWork();

private:
	//�����¼� ��������
	EventCall _onCreate;
	EventCall _onRun;
	EventCall _onDestory;
	//�ı�����ʱ ��Ҫ����
	std::mutex _mutex;
	//�����̵߳���ֹ���˳�
	CellSemaphore _semaphore;
	//�߳��Ƿ�����
	bool _state = false;

};

#endif // !_CELL_THREAD_H_

