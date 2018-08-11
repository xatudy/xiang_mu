#pragma once 

#include <string>
#include <unordered_map>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "block_queue.hpp"
#include "api.hpp"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

namespace server{

struct Context{
    std::string str;
    sockaddr_in addr;
};

class ChatServer{
public:
   
   int Start(const std::string& ip,short port);
    
 
   int RecvMsg();//接受消息
   
   int BroadCast();//广播消息


private:

   static void* Consume(void*);//消费在线程  三种关系 生生之间互斥关系  消消之间互斥关系 生消之间同步互斥关系
   static void* Product(void*);//生产者线程   为萨是static  pthread_create 必须传静态

   void AddUser(sockaddr_in addr,const std::string& name);
   void DelUser(sockaddr_in addr,const std::string& name);
   void SendMsg(const std::string& str,sockaddr_in addr);
   //key内容用户身份标示 ，ip+昵称
   //value ip+端口号（struct sockaddr_in）
   std::unordered_map<std::string,sockaddr_in> online_friend_list_;//类的私有成员_ 后缀
   //实现一个队列作为交易场所 Queue 需要一个人阻塞队列来作为生产者消费者模型的交易场所
  
   //暂时使用一个阻塞队列当交易
   //暂定队列元素类型为std::string 
  // BlockQueue<std::string> queue_;
   BlockQueue<Context> queue_;

   int sock_ ; //服务器进行绑定的文件描述符；

};

}//end server 
