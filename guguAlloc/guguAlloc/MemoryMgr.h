/*
内存管理工具类
对内存池进行管理
2021/4/22
*/

#ifndef _Memory_Mgr_h_
#define _Memory_Mgr_h_
//内存池最大申请
#define MAX_MEMORY_SIZE 128

//导入内存池模板类
#include"MemoryAlloc.h"

class MemoryMgr
{
public:
	//饿汉式单例模式
	static MemoryMgr* Instance();
	//申请内存
	void* allocMem(size_t nSize);
	//释放内存
	void freeMem(void* p);
	//增加内存块引用次数
	void addRef(void* p);

private:
	MemoryMgr();
	virtual ~MemoryMgr();
	//内存映射初始化
	void init_szAlloc(int begin, int end, MemoryAlloc* pMem);

private:
	//映射数组
	MemoryAlloc* _szAlloc[MAX_MEMORY_SIZE + 1];
	//64字节内存池
	MemoryAlloc _mem64;
	//128字节内存池
	MemoryAlloc _mem128;
};

#endif