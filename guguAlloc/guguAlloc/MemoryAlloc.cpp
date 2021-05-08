#include"MemoryAlloc.h"

MemoryAlloc::MemoryAlloc()
{
	_pBuf = nullptr;
	_pHeader = nullptr;
	_nSize = 0;
	_nBlockSize = 0;
}

MemoryAlloc::~MemoryAlloc()
{
	if (_pBuf)
	{
		free(_pBuf);
		//现在有一个问题就是内存池外申请的内存不会被主动释放
	}
}

void MemoryAlloc::setInit(size_t nSize, size_t nBlockSize)
{
	/*补全nSize
	const size_t n = sizeof(void*)
	_nSize = (nSize/n) * n + (nSize % n ? n : 0);
	*/
	_pBuf = nullptr;
	_pHeader = nullptr;
	_nSize = nSize;
	_nBlockSize = nBlockSize;
	initMemory();
}

void MemoryAlloc::initMemory()
{
	//断言
	assert(nullptr == _pBuf);
	//若已申请则返回
	if (nullptr != _pBuf)
	{
		return;
	}
	//计算内存池的大小  (块大小+块头) * 块数量
	size_t temp_size = _nSize + sizeof(MemoryBlock);//需要偏移的真正大小
	size_t bufSize = temp_size * _nBlockSize;
	//向系统申请池内存
	_pBuf = (char*)malloc(bufSize);
	//初始化内存池
	_pHeader = (MemoryBlock*)_pBuf;
	if (nullptr != _pHeader)
	{
		_pHeader->_bPool = true;//在池中
		_pHeader->_nID = 0;//第0块
		_pHeader->_nRef = 0;//引用次数为0
		_pHeader->_pAlloc = this;//属于当前内存池
		_pHeader->_pNext = nullptr;//下一块
		MemoryBlock* pTemp1 = _pHeader;
		//遍历内存块进行初始化
		for (size_t n = 1; n < _nBlockSize; n++)
		{
			MemoryBlock* pTemp2 = (MemoryBlock*)(_pBuf + (n * temp_size));//指针偏移到下一块
			pTemp2->_bPool = true;//在池中
			pTemp2->_nID = n;//第n块
			pTemp2->_nRef = 0;
			pTemp2->_pAlloc = this;
			pTemp2->_pNext = nullptr;
			pTemp1->_pNext = pTemp2;
			pTemp1 = pTemp2;
		}
	}
}

void* MemoryAlloc::allocMem(size_t nSize)
{
	//自解锁
	std::lock_guard<std::mutex> lock(_mutex);
	//若内存池不存在则初始化
	if (nullptr == _pBuf)
	{
		initMemory();
	}
	MemoryBlock* pReturn = nullptr;
	if (nullptr == _pHeader)//如内存池已满 重新申请
	{
		pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
		if (nullptr != pReturn)
		{
			pReturn->_bPool = false;//不在池中
			pReturn->_nID = -1;
			pReturn->_nRef = 1;
			pReturn->_pAlloc = this;
			pReturn->_pNext = nullptr;
		}
	}
	else//否则直接使用内存池
	{
		pReturn = _pHeader;
		_pHeader = _pHeader->_pNext;
		assert(0 == pReturn->_nRef);
		pReturn->_nRef = 1;
	}
	//debug打印
	if (nullptr != pReturn)
	{
		xPrintf("NEW - allocMem:%p,id=%d,size=%d\n", pReturn, pReturn->_nID, nSize);
	}
	return ((char*)pReturn + sizeof(MemoryBlock));
}

void MemoryAlloc::freeMem(void* p)
{
	//传进来的是消息区 需要加上信息头
	MemoryBlock* pBlock = (MemoryBlock*)((char*)p - sizeof(MemoryBlock));
	assert(1 == pBlock->_nRef);
	//判断是否被多次引用
	if (--pBlock->_nRef != 0)
	{
		return;
	}
	//判断是否在内存池中
	if (pBlock->_bPool)
	{
		//自解锁
		std::lock_guard<std::mutex> lock(_mutex);
		//把内存块放入内存池首位
		pBlock->_pNext = _pHeader;
		_pHeader = pBlock;
	}
	else
	{
		free(pBlock);
	}
}