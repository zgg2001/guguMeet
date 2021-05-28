/*
* ���̷߳��Ͳ���
* ��ͷ�ļ���ʵ��������ִ�еķ������
* ͨ��list�ṹ�洢��Ҫִ�е�����
* start()�����߳̽���������
* Ϊ��ֹ���ֳ�ͻ�������ٽ����������������������ʹ�û���������������
* 2021/4/22
* �Ƴ�������࣬����������ݸ�Ϊ��������
* ͨ�����������ķ�ʽ����Ҫִ�е�����ʹ������Ը��Ӷ�����
* 2021/5/4
* �����߳�Ϊ�߳���CellTask���� ��������˳��Ĺ���
* 2021/5/27
*/
#ifndef _CELL_Task_hpp_
#define _CELL_Task_hpp_

#include <thread>
#include <mutex>
#include <list>
#include <functional>//mem_fn
#include "CellThread.h"

//�����߳���
class CellTaskServer
{
private:
	typedef std::function<void()> CellTask;

public:
	int _id = -1;//���������߳�id

public:
	CellTaskServer();
	virtual ~CellTaskServer();
	//������� 
	void addTask(CellTask ptask);
	//��������
	void Start();
	//�ر�
	void Close();

protected:
	//�������� 
	void OnRun(CellThread* thread);

private:
	//�������� 
	std::list<CellTask>_tasks;
	//�������ݻ����� 
	std::list<CellTask>_tasksBuf;
	//�� �����ݻ����� 
	std::mutex _mutex;
	//�����߳�
	CellThread _thread;
};

#endif
