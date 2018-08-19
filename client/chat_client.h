
#pragma once 

#include<string>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"../server/api.hpp"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


namespace client{

class ChatClient{

public:

  int Init(const std::string& ip,short port);//客户端初始化 主要是进行scoket 的初始化
  //设置用户信息 姓名和学校
  //让客户端启动的时候提示用户输入姓名学校
  //从标准输入中读取这些数据
  int SetUserInfo(const std::string& name,const std::string&school);
  void SendMsg(const std::string& msg);
  //接收消息 ，本质上接收到的消息内容耶稣json格式的内容
  void RecvMsg(server::Data*data);
private:
  //这个文件描述符用来保存和服务器交互的socket 
  //通过Init函数创建好，交给SendMSg RecvMsg 直接使用 
  int sock_;
  sockaddr_in server_addr_;
  std::string name_;
  std::string school_;

};


}//end client 
