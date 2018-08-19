#pragma  once 
#include<unordered_set>
#include<deque>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
typedef struct sockaddr scokaddr;
typedef struct sockaddr_in sockaddr_in;

namespace  client {

class Window{

public:

  Window();
  ~Window();
  // 绘制标题子窗口
  void DrawHeader();
  // 绘制输入窗
  void DrawInput();
  // 绘制输出窗
  void DrawOutput();
  // 绘制好友列表
  void DrawFriendList();
  //往窗口中写字符串
  void PutStrToWin(WINDOW* win,int y,int x,const std::string& str);
  //从窗口中读取字符串
  void GetStrFromWin(WINDOW* win,std::string* str);

  void AddMsg(const std::string& msg);
//添加好友
  void AddFriend(const std::string& friend_info_);
//删除好友
  void DelFriend(const std::string& friend_info_);

  WINDOW* header_win_;
  WINDOW* input_win_;
  WINDOW* output_win_;
  WINDOW* friend_list_win_;
private:

  std::deque<std::string> msgs_;//双端队列支持随即访问
  //当前好友列表
  std::unordered_set<std::string> friend_list_;
};

} //end client
