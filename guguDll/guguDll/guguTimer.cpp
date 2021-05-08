#include"guguTimer.h"
#include"pch.h"

std::chrono::steady_clock::time_point _begin;//起始时间
std::chrono::steady_clock::time_point _end;//终止时间

mytimer::mytimer()
{
    _begin = std::chrono::steady_clock::now();
    _end = std::chrono::steady_clock::time_point();
}

mytimer::~mytimer()
{
}

void mytimer::UpDate()
{
    _begin = std::chrono::steady_clock::now();
}

double mytimer::GetSecond()
{
    _end = std::chrono::steady_clock::now();
    //使用duration类型变量进行时间的储存   duration_cast是类型转换方法
    std::chrono::duration<double> temp = std::chrono::duration_cast<std::chrono::duration<double>>(_end - _begin);
    return temp.count();//count() 获取当前时间的计数数量
}
