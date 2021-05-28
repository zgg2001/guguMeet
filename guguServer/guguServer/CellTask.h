/*
* 子线程发送部分
* 本头文件中实现了任务执行的分离操作
* 通过list结构存储需要执行的任务
* start()启动线程进行任务处理
* 为防止出现冲突，所有临界操作均进行上锁，且首先使用缓冲区储存新任务
* 2021/4/22
* 移除任务基类，任务队列内容改为匿名函数
* 通过匿名函数的方式储存要执行的任务，使任务可以更加多样化
* 2021/5/4
* 更改线程为线程类CellTask对象 方便进行退出的管理
* 2021/5/27
*/
#ifndef _CELL_Task_hpp_
#define _CELL_Task_hpp_

#include <thread>
#include <mutex>
#include <list>
#include <functional>//mem_fn
#include "CellThread.h"

//发送线程类
class CellTaskServer
{
private:
	typedef std::function<void()> CellTask;

public:
	int _id = -1;//所属接收线程id

public:
	CellTaskServer();
	virtual ~CellTaskServer();
	//添加任务 
	void addTask(CellTask ptask);
	//启动服务
	void Start();
	//关闭
	void Close();

protected:
	//工作函数 
	void OnRun(CellThread* thread);

private:
	//任务数据 
	std::list<CellTask>_tasks;
	//任务数据缓冲区 
	std::list<CellTask>_tasksBuf;
	//锁 锁数据缓冲区 
	std::mutex _mutex;
	//发送线程
	CellThread _thread;
};

#endif
