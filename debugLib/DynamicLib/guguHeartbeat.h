/*
* ������ʱ����
* 2021/5/5
*/
#ifndef _GUGU_HEARTBEAT_H_
#define _GUGU_HEARTBEAT_H_

#include<chrono>

class __declspec(dllexport) HBtimer
{
private:

public:
    HBtimer();

    virtual ~HBtimer();

    //��ȡ��ǰʱ��� (����)
    static time_t getNowMillSec();

};

#endif