/*
内存块类
内存管理的最小单位
2021/4/22
*/

#ifndef _Memory_Block_h_
#define _Memory_Block_h_

//声明内存池类
class MemoryAlloc;
//最底层导入内存头文件/断言头文件/锁头文件
#include<stdlib.h>
#include<assert.h>
#include<mutex>
//如果为debug模式则加入调试信息
#ifdef _DEBUG
#include<stdio.h>
#define xPrintf(...) printf(__VA_ARGS__)
#else
#define xPrintf(...)
#endif

class MemoryBlock
{
public:
	//内存块编号
	int _nID;
	//引用情况
	int _nRef;
	//所属内存池
	MemoryAlloc* _pAlloc;
	//下一块位置
	MemoryBlock* _pNext;
	//是否在内存池内
	bool _bPool;

private:

};

#endif
