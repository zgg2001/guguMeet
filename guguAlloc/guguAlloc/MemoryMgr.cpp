#include"MemoryMgr.h"

MemoryMgr::MemoryMgr()
{
	_mem64.setInit(64, 1000);
	init_szAlloc(0, 64, &_mem64);
	_mem128.setInit(128, 10);
	init_szAlloc(65, 128, &_mem128);
}

MemoryMgr::~MemoryMgr()
{
}

//初始化
void MemoryMgr::init_szAlloc(int begin, int end, MemoryAlloc* pMem)
{
	//begin到end大小的内存申请都映射到相关的内存池上
	for (int i = begin; i <= end; i++)
	{
		_szAlloc[i] = pMem;
	}
}

//饿汉式单例模式
MemoryMgr* MemoryMgr::Instance()
{
	static MemoryMgr myMemoryMgr;
	//单例对象
	return &myMemoryMgr;
}

//申请内存
void* MemoryMgr::allocMem(size_t nSize)
{
	//若申请的内存大小正常，则直接申请
	if (nSize <= MAX_MEMORY_SIZE)
	{
		return _szAlloc[nSize]->allocMem(nSize);
	}
	else//否则用malloc申请一个
	{
		MemoryBlock* pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
		if (nullptr != pReturn)
		{
			pReturn->_bPool = false;//不在池中
			pReturn->_nID = -1;
			pReturn->_nRef = 1;
			pReturn->_pAlloc = nullptr;
			pReturn->_pNext = nullptr;
			//debug打印
			xPrintf("NEW - allocMem:%p,id=%d,size=%d\n", pReturn, pReturn->_nID, nSize);
		}
		return ((char*)pReturn + sizeof(MemoryBlock));
	}
}

//释放内存
void MemoryMgr::freeMem(void* p)
{
	//传进来的是消息区 需要加上信息头
	MemoryBlock* pBlock = (MemoryBlock*)((char*)p - sizeof(MemoryBlock));
	//debug打印
	xPrintf("DELETE - allocMem:%p,id=%d\n", pBlock, pBlock->_nID);
	//内存池内的内存块/内存池外的内存块 不同的处理方式
	if (pBlock->_bPool == true)
	{
		pBlock->_pAlloc->freeMem(p);
	}
	else
	{
		if (--pBlock->_nRef == 0)
		{
			free(pBlock);
		}
	}
}

//增加内存块引用次数
void MemoryMgr::addRef(void* p)
{
	MemoryBlock* pBlock = (MemoryBlock*)((char*)p - sizeof(MemoryBlock));
	++pBlock->_nRef;
}
