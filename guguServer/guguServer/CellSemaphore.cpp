#include "CellSemaphore.h"

CellSemaphore::CellSemaphore()
{
}

CellSemaphore::~CellSemaphore()
{
}

void CellSemaphore::wait()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (--_wait < 0)
	{
		//×èÈû¿ªÊ¼ µÈ´ý»½ÐÑ
		_cv.wait(lock, [this]()->bool 
		{
			return _wakeup > 0;
		});
		--_wakeup;
	}
}

void CellSemaphore::wakeup()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (++_wait <= 0)
	{
		++_wakeup;
		_cv.notify_one();
	}
}
