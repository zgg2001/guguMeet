#include"ClientSocket.h"

ClientSocket::ClientSocket(SOCKET sockfd)
{
	_sockfd = sockfd;
	//缓冲区相关 
	_Msg_Recv_buf = new char[RECV_BUFFER_SIZE * 5];
	_Len_Recv_buf = 0;
	_Msg_Send_buf = new char[SEND_BUFFER_SIZE];
	_Len_Send_buf = 0;
	//重置心跳计时
	ResetDtHeart();
	//重置发送计时器
	ResetDtSend();
}

ClientSocket::~ClientSocket()
{
	delete[] _Msg_Recv_buf;
	delete[] _Msg_Send_buf;
}

SOCKET ClientSocket::GetSockfd()
{
	return _sockfd;
}

char* ClientSocket::MsgBuf()
{
	return _Msg_Recv_buf;
}

int ClientSocket::GetLen()
{
	return _Len_Recv_buf;
}

void ClientSocket::SetLen(int len)
{
	_Len_Recv_buf = len;
}

int ClientSocket::SendData(DataHeader* head)
{
	int ret = SOCKET_ERROR;
	//要发送的数据长度 
	int nSendLen = head->date_length;
	//要发送的数据 
	const char* pSendData = (const char*)head;
	//发送次数 
	int times = 0;
	while (1)
	{
		//如果接收数据超过发送缓冲区 就发送 
		if (_Len_Send_buf + nSendLen >= SEND_BUFFER_SIZE)
		{
			//缓冲区剩余空间大小 
			int nCopyLen = SEND_BUFFER_SIZE - _Len_Send_buf;
			//拷贝数据
			memcpy(_Msg_Send_buf + _Len_Send_buf, pSendData, nCopyLen);
			//剩余数据位置与剩余数据长度
			pSendData += nCopyLen;
			nSendLen -= nCopyLen;
			//发送 
			ret = send(_sockfd, (const char*)_Msg_Send_buf, SEND_BUFFER_SIZE, 0);
			//发送后缓冲区归零 
			_Len_Send_buf = 0;
			//重置发送计时器
			ResetDtSend();
			//发送错误 
			if (SOCKET_ERROR == ret)
			{
				return ret;
			}
			times++;
		}
		else//否则就放入缓冲区 
		{
			//拷贝 
			memcpy(_Msg_Send_buf + _Len_Send_buf, pSendData, nSendLen);
			//数据尾部后移 
			_Len_Send_buf += nSendLen;
			break;
		}
	}
	return times;
}

void ClientSocket::ResetDtHeart()
{
	_dtHeart = 0;
}

bool ClientSocket::CheckHeart(time_t dt)
{
	_dtHeart += dt;
	if (_dtHeart >= CLIENT_HREAT_TIME)
	{
		printf("CheakHeart dead:%d,time=%lld\n",_sockfd,_dtHeart);
		return true;
	}
	return false;
}

void ClientSocket::ResetDtSend()
{
	_dtSend = 0;
}

bool ClientSocket::CheckSend(time_t dt)
{
	_dtSend += dt;
	if (_dtSend >= CLIENT_AUTOMATIC_SEND_TIME)
	{
		//printf("AutomaticSend:%d,time=%lld\n", _sockfd, _dtSend);
		return true;
	}
	return false;
}

int ClientSocket::SendAll()
{
	int ret = SOCKET_ERROR;
	if (_Len_Send_buf > 0 && SOCKET_ERROR != _sockfd)
	{
		//发送 
		ret = send(_sockfd, (const char*)_Msg_Send_buf, _Len_Send_buf, 0);
		//发送后缓冲区归零 
		_Len_Send_buf = 0;
		//重置发送计时器
		ResetDtSend();
		//发送错误 
		if (SOCKET_ERROR == ret)
		{
			printf("error 发送失败");
		}
	}
	return ret;
}
