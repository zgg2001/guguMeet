/*
����ʵ���������ź������жϣ�ʹ�ÿ���������������£��ȴ�һ���ź����Ļ���
2021/5/27
*/
#ifndef _CELL_SEMAPHORE_H_
#define _CELL_SEMAPHORE_H_

#include<condition_variable>//��������
#include<mutex>

class CellSemaphore
{
public:
	CellSemaphore();
	~CellSemaphore();
	//��ʼ����
	void wait();
	//����
	void wakeup();

private:
	//�ȴ���
	int _wait = 0;
	//������
	int _wakeup = 0;
	//��������
	std::condition_variable _cv;
	//��
	std::mutex _mutex;

};

#endif // !_CELL_SEMAPHORE_H_

