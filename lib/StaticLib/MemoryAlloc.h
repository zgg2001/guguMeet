/*
内存池类
对内存块进行管理
2021/4/22
*/

#ifndef _Memory_Alloc_h_
#define _Memory_Alloc_h_

//导入内存块头文件
#include"MemoryBlock.h"

class MemoryAlloc
{
public:
	
	MemoryAlloc();
	virtual ~MemoryAlloc();
	//设置初始化
	void setInit(size_t nSize, size_t nBlockSize);
	//初始化
	void initMemory();
	//申请内存
	void* allocMem(size_t nSize);
	//释放内存
	void freeMem(void* p);

protected:
	//内存池地址
	char* _pBuf;
	//头部内存单元
	MemoryBlock* _pHeader;
	//内存块大小
	size_t _nSize;
	//内存块数量
	size_t _nBlockSize;
	//多线程锁
	std::mutex _mutex;
};

#endif