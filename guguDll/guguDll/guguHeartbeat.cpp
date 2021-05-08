#include "guguHeartbeat.h"
#include "pch.h"
using namespace std::chrono;

HBtimer::HBtimer()
{
}

HBtimer::~HBtimer()
{
}

time_t HBtimer::getNowMillSec()
{
    //获取高精度当前时间(毫秒) high_resolution_clock::now();
    //duration_cast是类型转换方法
    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}
