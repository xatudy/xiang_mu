#include "chat_server.h"
#include "api.hpp"
#include <sstream>

namespace server{


int ChatServer::Start(const std::string& ip,short port){
  //启动服务去并且进入事件循环
  //使用udp作为我们的通信协议
  sock_=socket(AF_INET,SOCK_DGRAM,0);
  if(sock_<0){
    perror("socket");
    return -1;
  }
  sockaddr_in addr;
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=inet_addr(ip.c_str());
  addr.sin_port=htons(port);
  int ret=bind(sock_,(sockaddr*)&addr,sizeof(addr));
  if(ret<0){
    perror("bind");
    return -1;
  }
  printf("打印一个日志\n");
  //LOG(INFO)<<"server start OK\n";
  pthread_t productor,comsumer;
  pthread_create(&productor,NULL,Product,this);
  pthread_create(&comsumer,NULL,Consume,this);
  pthread_join(productor,NULL);
  pthread_join(comsumer,NULL);
  return 0;
  }
void*ChatServer::Product(void*arg){
  //生产者线程，主要做的事情读取socket中的数据，并且写入到blockQueue
  ChatServer * server=reinterpret_cast<ChatServer*>(arg); 
  while(true){
  //RecvMsg 读取一次数据并且写道blockQueue中
  server->RecvMsg();
  }
  return NULL;
}
void*ChatServer::Consume(void*arg){
  // 消费者线程
  ChatServer* server =reinterpret_cast<ChatServer*>(arg);
  while(true){
    server->BroadCast();
  }
  return NULL;
  }

int ChatServer::RecvMsg(){
  //1 从socket中读取数据
  char buf[1024*5]={0}; //5k
  sockaddr_in peer;
  socklen_t len=sizeof(peer);
  ssize_t read_size=recvfrom(sock_,buf,sizeof(buf)-1,0,(sockaddr*)&peer,
      &len);
  if(read_size<0){
    perror("recvfrom");
    return -1;
  }
  buf[read_size]= '\0';

  // 把数据插入到blockQueue中
  Context context;
  context.str=buf;
  context.addr=peer;
  queue_.PushBack(context);
  return 0;
}

int ChatServer::BroadCast() { 
  //1.从队列中读取元素
  Context context;
  queue_.PopFront(&context);
  //2.对取出的字符串数据进行反序列化
  Data data; 
  data.UnSerialize(context.str);
  //3.根据这个消息跟新我们的在在线好友列表
  if(data.cmd=="quit"){

  // 如果这个成员是一个下线的消息 （command是一个特殊直），
  // 把这样的一个成员从好友列表中删除;
  
    DelUser(context.addr,data.name);
  
  }else{
  
  // 如果成员不再好友列表 加入进来
   //准备使用[]方法来操作 在线好友列表
  
    AddUser(context.addr,data.name);
 
  }
  // 4.遍历在线好友列表，把这个数据一次发送给每一个客户端。
  //   （由于当前发送消息的用户也存在当前列表中，因此在便利列表时u也会给自己发消息，从而达到发送者能够看到自己发送的消息的效果，但是这种实现方式比较蠢，完全可以控制客户端，不过经过网络传输就实现这个功能）
  
  for(auto item :online_friend_list_) {
    SendMsg(context.str,item.second);
  }
  return 0; 
}

void ChatServer::AddUser(sockaddr_in addr,const std::string& name){
 //这里的key相当于对我门的IP地址和用户名进行拼接
  //之所以 用name 和 IP 共同进行拼接 本质上是因为使用的ip可能会重复
  std::stringstream ss;
  //dengyu:1234
  ss<< name <<":"<< addr.sin_addr.s_addr;
  //[]map unordered_map ,
  //1.如果不存在，就插入
  //2.如果存在就修改
  //ValueType& operator[](const KeyType& key)
  //const ValueType& opertaor[(const KeyType& key)const]
  
  //insert  返回i直类型 迭代器 插入成功后的位置；bool 成功与失败
  online_friend_list_[ss.str()]=addr;
  return;
}

void ChatServer::DelUser(sockaddr_in addr,const std::string& name){
  std::stringstream ss;
  ss<<name<<":"<< addr.sin_addr.s_addr;
  online_friend_list_.erase(ss.str());
  return;
}
void ChatServer::SendMsg(const std::string& data ,sockaddr_in addr){
  // 2.把这个数据通过sendto发送给客户端
  
  sendto(sock_,data.data(),data.size(),0,
      (sockaddr*)&addr,sizeof(addr));
  
}


}//end server 
