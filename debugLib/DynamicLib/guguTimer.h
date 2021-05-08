/*
* 计时器类
* 2021/4/23
*/
#ifndef _MY_TIMER_H_
#define _MY_TIMER_H_

#include<chrono>

class __declspec(dllexport) mytimer
{
private:
    
public:
    mytimer();

    virtual ~mytimer();

    //调用update时，使起始时间等于当前时间
    void UpDate();

    //调用getsecond方法时，经过的时间为当前时间减去之前统计过的起始时间。
    double GetSecond();

};

#endif
