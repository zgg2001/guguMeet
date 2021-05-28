#include "CellThread.h"

void CellThread::Start(EventCall onCreate, EventCall onRun, EventCall onDestory)
{
	//����
	std::lock_guard<std::mutex> lock(_mutex);
	if (!_state)
	{
		//�¼���ֵ
		if (onCreate)
			_onCreate = onCreate;
		if (onRun)
			_onRun = onRun;
		if (onDestory)
			_onDestory = onDestory;
		//�߳�����
		_state = true;
		std::thread t(std::mem_fn(&CellThread::OnWork), this);
		t.detach();
	}
}

void CellThread::Close()
{
	//����
	std::lock_guard<std::mutex> lock(_mutex);
	if (_state)
	{
		_state = false;
		_semaphore.wait();
	}
}

void CellThread::Exit()
{
	//����
	std::lock_guard<std::mutex> lock(_mutex);
	if (_state)
	{
		_state = false;
		//�����Ŀ�����˳��߳� û��Ҫ�������ź���
	}
}

bool CellThread::isRun()
{
	return _state;
}

void CellThread::OnWork()
{
	//��ʼ�¼�
	if (_onCreate)
		_onCreate(this);
	//����
	if (_onRun)
		_onRun(this);
	//����
	if (_onDestory)
		_onDestory(this);
	_semaphore.wakeup();
}
