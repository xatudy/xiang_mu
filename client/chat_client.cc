
#include "chat_client.h"
#include "../server/api.hpp"
#include <iostream>
namespace client{

  //由于版本比较low  ip  port 是通过命令行参数来传的
  //如果需要改进从配置文件中读取ip  port 如何修改
  // 例如 可以使用json 来组织配置格式
  // {
  //   ip:"127.0.0.1"
  //   port:9090
  // }
 int ChatClient::Init(const std::string& server_ip,short server_port){
     sock_=socket(AF_INET,SOCK_DGRAM,0);
     if(sock_< 0){
       perror("socket");
       return -1;
     }
    server_addr_.sin_family=AF_INET ;
    server_addr_.sin_port=htons(server_port);
    server_addr_.sin_addr.s_addr=inet_addr(server_ip.c_str());
    return 0;

  }

 int ChatClient::SetUserInfo(const std::string& name,
                       const std::string&school){
     name_=name;
     school_=school;
     return 0;
  }
 // 用户发 hehe  msg="hehe"
void ChatClient::SendMsg(const std::string& msg){
  //按照下面这种方式来发送是错误的
  //客户端和服务器交互的接口要严格按照服务器提供的 api来操作
  //这样的约定就相当于一个自定制应用层协议
  //sendto(sock_,msg.c_str(),msg.size(),0,(sockaddr*)&server_addr_,
       // sizeof(server_addr_));
  server::Data data;
  data.name=name_;
  data.school=school_;
  data.msg=msg;
  //以下的逻辑 相当于约定了用户输入什么样的内容要进行下线
  if(data.msg=="exit"||data.msg=="quit" || data.msg=="Q"){
    data.cmd ="quit";
  }
  std::string str;
  data.Serialize(&str);

  //真正的发送数据
  sendto(sock_,str.c_str(),str.size(),0,
      (sockaddr*)&server_addr_,sizeof(server_addr_));
  return;
  }


#include<stdio.h>
void Print(const std::string& log){
  FILE* fd=fopen("./log.txt","a");
  fwrite(log.c_str(),log.size(),1,fd);
  fclose(fd);
}


void ChatClient:: RecvMsg(server::Data*data){
  char buf[1024*10]={10};
  //由于对端的ip端口号就是服务器的ip端口号 ，所以此处
  //没有要再获取一遍对端的ip 这部分内容已经知道了
  ssize_t read_size=recvfrom(sock_,buf,sizeof(buf)-1,0,NULL,NULL);
  if(read_size<0){
    perror("recvfrom");
    return ;
  }
  buf[read_size]='\0';
  Print(buf); 
  data->UnSerialize(buf);
  return ;
}

}


/////////////////////////////////////////////////////
//线实现一个简易版本的客户端，通过这个简易i版本来测试客户和
//服务器的代码
// 
//////////////////////////////////////////////////


#ifdef  TEST_CHAT_CLIENT

void* Send(void* arg){
  client::ChatClient* client=reinterpret_cast<client::ChatClient*>(arg);
  //循环发送数据
  while(true){
   std::string str;
   std::cin>>str;
   client->SendMsg(str);
  }
  return NULL;
}

void* Recv(void* arg){
  
  //循环接收数据
  client::ChatClient* client=reinterpret_cast<client::ChatClient*>(arg);
  while(true){
    server::Data data;
    client->RecvMsg(&data);
    std::cout<<"["<< data.name << "|"<< data.school<< "]"
             << data.msg <<"\n";
  }
  return NULL;
}


int main(int argc,char* argv[]){
 
  if(argc!=3){
    printf("usage ./client [ip][port]\n");
    return 1;
  }
  
  client::ChatClient client;
  client.Init(argv[1],atoi(argv[2]));
  std::string name,school;
  std::cout<<"输入用户名:";
  std::cin>>name;
  std::cout<<"输入学校:";
  std::cin>>school;
  client.SetUserInfo(name,school);
  
  //创建两个线程，分别用于发送数据和接收数据
  pthread_t stid, rtid;
  pthread_create(&stid,NULL,Send,&client);
  pthread_create(&rtid,NULL,Recv,&client);
  pthread_join(stid,NULL);
  pthread_join(rtid,NULL);

  return 0;
}

#endif
