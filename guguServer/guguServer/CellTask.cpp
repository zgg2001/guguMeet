#include"CellTask.h"

CellTaskServer::CellTaskServer()
{
}

CellTaskServer::~CellTaskServer()
{
}

void CellTaskServer::addTask(CellTask ptask)
{
	std::lock_guard<std::mutex>lock(_mutex);
	_tasksBuf.push_back(ptask);
}

void CellTaskServer::Start()
{
	//线程
	std::thread t(std::mem_fn(&CellTaskServer::OnRun), this);
	t.detach();
}

void CellTaskServer::OnRun()
{
	while (1)
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
}
