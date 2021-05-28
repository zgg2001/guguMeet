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
	//���������߳�
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
		//�������������ݼ��� 
		if (!_tasksBuf.empty())//��Ϊ�� 
		{
			std::lock_guard<std::mutex>lock(_mutex);
			for (auto pTask : _tasksBuf)
			{
				_tasks.push_back(pTask);
			}
			_tasksBuf.clear();
		}
		//���������
		if (_tasks.empty())
		{
			//��Ϣһ���� 
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}
		//��������
		for (auto pTask : _tasks)
		{
			pTask();
		}
		//������� 
		_tasks.clear();
	}
	printf("CellTask:%d OnRun() exit\n", _id);
}
