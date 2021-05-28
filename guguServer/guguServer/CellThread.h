/*
线程封装类
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
	//启动线程
	void Start(EventCall onCreate = nullptr, EventCall onRun = nullptr, EventCall onDestory = nullptr);
	//关闭线程
	void Close();
	//工作中退出
	void Exit();
	//是否运行中
	bool isRun();

protected:
	//工作函数
	void OnWork();

private:
	//三个事件 匿名函数
	EventCall _onCreate;
	EventCall _onRun;
	EventCall _onDestory;
	//改变数据时 需要加锁
	std::mutex _mutex;
	//控制线程的终止与退出
	CellSemaphore _semaphore;
	//线程是否启动
	bool _state = false;

};

#endif // !_CELL_THREAD_H_

