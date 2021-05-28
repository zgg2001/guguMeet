/*
本类实现了条件信号量的判断，使得可以在阻塞的情况下，等待一个信号量的唤醒
2021/5/27
*/
#ifndef _CELL_SEMAPHORE_H_
#define _CELL_SEMAPHORE_H_

#include<condition_variable>//条件变量
#include<mutex>

class CellSemaphore
{
public:
	CellSemaphore();
	~CellSemaphore();
	//开始阻塞
	void wait();
	//唤醒
	void wakeup();

private:
	//等待数
	int _wait = 0;
	//唤醒数
	int _wakeup = 0;
	//条件变量
	std::condition_variable _cv;
	//锁
	std::mutex _mutex;

};

#endif // !_CELL_SEMAPHORE_H_

