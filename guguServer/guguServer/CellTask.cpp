#include"CellTask.h"

CellTaskServer::CellTaskServer()
{
}

CellTaskServer::~CellTaskServer()
{
	if (_thread.isRun())
	{
		Close();
	}
}

void CellTaskServer::addTask(CellTask ptask)
{
	std::lock_guard<std::mutex>lock(_mutex);
	_tasksBuf.push_back(ptask);
}

void CellTaskServer::Start()
{
	//启动接收线程
	_thread.Start(
		//onCreate
			nullptr,
		//onRun
		[this](CellThread*)
		{
			OnRun(&_thread);
		},
		//onDestory
			nullptr);
}

void CellTaskServer::Close()
{
	printf("CellTask:%d close start\n", _id);
	_thread.Close();
	printf("CellTask:%d close end\n", _id);
}

void CellTaskServer::OnRun(CellThread* thread)
{
	while (thread->isRun())
	{
		//将缓冲区内数据加入 
		if (!_tasksBuf.empty())//不为空 
		{
			std::lock_guard<std::mutex>lock(_mutex);
			for (auto pTask : _tasksBuf)
			{
				_tasks.push_back(pTask);
			}
			_tasksBuf.clear();
		}
		//如果无任务
		if (_tasks.empty())
		{
			//休息一毫秒 
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}
		//处理任务
		for (auto pTask : _tasks)
		{
			pTask();
		}
		//清空任务 
		_tasks.clear();
	}
	printf("CellTask:%d OnRun() exit\n", _id);
}
