#include<iostream>

#include"../server/api.hpp"
#include<signal.h>
#include"chat_client.h"
#include"window.h"

//man函数里的代码少的好处
// 意味着所有逻辑在其他函数里 可以很方便的编译成静态库或者动态库方便
// 别人使用

client::ChatClient* g_client=NULL;
client::Window* g_window=NULL;
pthread_mutex_t g_lock;

void* Send(void* arg){
  //发送线程 ：读取输入数据 发送给服务器
  (void)arg;
  while(1){
    std::string msg;
    
    pthread_mutex_lock(&g_lock);
    g_window->DrawInput();
    pthread_mutex_unlock(&g_lock);
    
    g_window->GetStrFromWin(g_window->input_win_,&msg);
    g_client->SendMsg(msg);
  }
  return NULL;
}

void* Recv(void* arg){
  //接收线程：从服务器中读取数据 显示到界面上
  (void)arg;
  while(1){
    //此处绘制两个窗口 因为受到一条数据后
    //output窗口需要更新，好友列表窗口也可能需要更新
   
    pthread_mutex_lock(&g_lock);
    g_window->DrawOutput();
    g_window->DrawFriendList();
    pthread_mutex_unlock(&g_lock);
    
    server::Data data;
    g_client->RecvMsg(&data);
    if(data.cmd=="quit"){
      //删除的方式和插入的方式能够对应上
      g_window->DelFriend(data.name+ "|"+data.school);
    }else{
      //相当于好友列表的显示格式如下
      //邓禹|西工院
  
      g_window->AddFriend(data.name+"|"+data.school);
      g_window->AddMsg(data.name+":"+data.msg);
    }
  }
  return NULL;
}

void  Quit(int sig){
  (void)sig;
  //此处delete最主要的目的是为了执行析构函数尤其是
  //g_window 的析构函数 因为这个函数对ncurses进行了销毁
  //如果不执行这个动作，很可能导致终端显示混乱
  g_client->SendMsg("quit");
  delete  g_window;
  delete  g_client;
  exit(0);
}

//google 常见的C++开源库
//protobuf 用来序列化
//glog 打日志
//gflag:配置项管理
//grpc:RPC框架
//gtest:单元测试框架
void Run(const std::string& ip,short port)
{
  //0.捕捉SIGINT
  signal(SIGINT,Quit); 
 pthread_mutex_init(&g_lock,NULL);
  //1.初始化客户端核心模块 
  g_client=new client::ChatClient();
  int ret=g_client->Init(ip,port);
  if(ret<0){
    printf("client Init failed\n");
    return;
  }

  //2.提示用户输入 用户名 和学校
  std::string name,school;
  std::cout<<"输入用户名"<<std::endl;
  std::cin>>name;
  std::cout<<"输入学校"<<std::endl;
  std::cin>>school;
  g_client->SetUserInfo(name,school);
  //3.初始化用户界面模块
  g_window=new client::Window();
  g_window->DrawHeader();
  //4创建线程两个线程 ：
  pthread_t stid,rtid;

  // (1) 发送线程：读取输入数据，发送给服务器
  pthread_create(&stid,NULL,Send,NULL);
  // （2）接收线程：从服务器中读取数据 显示到界面上
  pthread_create(&rtid,NULL,Recv,NULL);
  pthread_join(stid,NULL);
  pthread_join(rtid,NULL);
  //5.对进程退出好似进行处理
  // g_client->SendMsg("quit");
  delete g_window;
  delete g_client;
  pthread_mutex_destroy(&g_lock);

}

int main(int argc,char* argv[]){

  if(argc!=3){
    printf("Usage ./chat_client  [ip] [port]\n");
    return 1;
  }
  //一个是ip地址 一个是端口号
  Run(argv[1],atoi(argv[2]));
  return 0;
}
