/*
* 报文数据类型
* 2021/4/23
* 添加心跳报文类型
* 2021/5/5
*/
#ifndef _CMD_H_
#define _CMD_H_

//枚举类型记录命令 
enum cmd
{
	CMD_LOGIN,//登录 
	CMD_LOGINRESULT,//登录结果 
	CMD_LOGOUT,//登出 
	CMD_LOGOUTRESULT,//登出结果 
	CMD_NEW_USER_JOIN,//新用户登入 
	CMD_C2S_HEART,//心跳 client to server
	CMD_S2C_HEART,//心跳 server to client
	CMD_ERROR//错误 
};
//定义数据包头 
struct DataHeader
{
	short cmd;//命令
	short date_length;//数据的长短	
};
//包1 登录 传输账号与密码
struct Login : public DataHeader
{
	Login()//初始化包头 
	{
		this->cmd = CMD_LOGIN;
		this->date_length = sizeof(Login);
	}
	char UserName[32];//用户名 
	char PassWord[32];//密码 
};
//包2 登录结果 传输结果
struct LoginResult : public DataHeader
{
	LoginResult()//初始化包头 
	{
		this->cmd = CMD_LOGINRESULT;
		this->date_length = sizeof(LoginResult);
	}
	int Result;
};
//包3 登出 传输用户名 
struct Logout : public DataHeader
{
	Logout()//初始化包头 
	{
		this->cmd = CMD_LOGOUT;
		this->date_length = sizeof(Logout);
	}
	char UserName[32];//用户名 
};
//包4 登出结果 传输结果
struct LogoutResult : public DataHeader
{
	LogoutResult()//初始化包头 
	{
		this->cmd = CMD_LOGOUTRESULT;
		this->date_length = sizeof(LogoutResult);
	}
	int Result;
};
//包5 新用户登入 传输通告 
struct NewUserJoin : public DataHeader
{
	NewUserJoin()//初始化包头 
	{
		this->cmd = CMD_NEW_USER_JOIN;
		this->date_length = sizeof(NewUserJoin);
	}
	char UserName[32];//用户名 
};
//包6 心跳 client to server
struct C2S_Heart : public DataHeader
{
	C2S_Heart()//初始化包头 
	{
		this->cmd = CMD_C2S_HEART;
		this->date_length = sizeof(C2S_Heart);
	}
};
//包7 心跳 server to client
struct S2C_Heart : public DataHeader
{
	S2C_Heart()//初始化包头 
	{
		this->cmd = CMD_S2C_HEART;
		this->date_length = sizeof(S2C_Heart);
	}
};

#endif
