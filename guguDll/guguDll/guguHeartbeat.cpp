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
    //��ȡ�߾��ȵ�ǰʱ��(����) high_resolution_clock::now();
    //duration_cast������ת������
    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}
