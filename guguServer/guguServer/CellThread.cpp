#include "CellThread.h"

void CellThread::Start(EventCall onCreate, EventCall onRun, EventCall onDestory)
{
	//上锁
	std::lock_guard<std::mutex> lock(_mutex);
	if (!_state)
	{
		//事件赋值
		if (onCreate)
			_onCreate = onCreate;
		if (onRun)
			_onRun = onRun;
		if (onDestory)
			_onDestory = onDestory;
		//线程启动
		_state = true;
		std::thread t(std::mem_fn(&CellThread::OnWork), this);
		t.detach();
	}
}

void CellThread::Close()
{
	//上锁
	std::lock_guard<std::mutex> lock(_mutex);
	if (_state)
	{
		_state = false;
		_semaphore.wait();
	}
}

void CellThread::Exit()
{
	//上锁
	std::lock_guard<std::mutex> lock(_mutex);
	if (_state)
	{
		_state = false;
		//这里的目的是退出线程 没必要阻塞等信号量
	}
}

bool CellThread::isRun()
{
	return _state;
}

void CellThread::OnWork()
{
	//开始事件
	if (_onCreate)
		_onCreate(this);
	//运行
	if (_onRun)
		_onRun(this);
	//销毁
	if (_onDestory)
		_onDestory(this);
	_semaphore.wakeup();
}
