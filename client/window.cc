#include"window.h"
#include<locale.h>

namespace client{

Window::Window()
{
  //设置中文编码
setlocale(LC_ALL,""); 
//对整个ncurses进行初始化
initscr();
//隐藏光标
curs_set(0);
}
Window::~Window(){
 //对整个ncurses进行销毁
 //如果忘记调用这个函数 终端会显示混乱
  endwin();
}
void Window::DrawHeader(){

  //LINES 是ncurses 提供的宏 表示当前窗口的最大行数
  int h=LINES/5;
  // 表示当前窗口的最大列数   
  int w=COLS;
  int y=0;
  int x=0;
  header_win_= newwin(h,w,y,x);
  std::string title="我是一个简单的聊天系统";
  PutStrToWin(header_win_, h/2,w/2-title.size()/2, title);
  box(header_win_,'|','-');
  wrefresh(header_win_);

}
void Window::PutStrToWin(WINDOW* win,int y,int x,const std::string& str){
  mvwaddstr(win,y,x,str.c_str());
}

void Window::GetStrFromWin(WINDOW* win,std::string* str){
  char buf[1024*10]={0};
  wgetnstr(win,buf,sizeof(buf)-1);
  *str=buf;
  //可以改成深拷贝
}

void Window::DrawInput(){

  //LINES 是ncurses 提供的宏 表示当前窗口的最大行数
  int h=LINES/5;
  // 表示当前窗口的最大列数   
  int w=COLS;
  int y=LINES*4/5;
  int x=0;
  input_win_= newwin(h,w,y,x);
  std::string title="请输入消息";
  //此时的坐标其实是窗口左上角为原点的坐标系
  //从1 1开始填充是为了不与输入框冲突
  PutStrToWin(input_win_, 1, 2,title);
  box(input_win_,'|','-');
  wrefresh(input_win_);

}

void Window::DrawOutput(){

  int h=LINES *3/5;
  int w=COLS*3/5;
  int y=LINES/5;
  int x=0;
output_win_=newwin(h,w,y,x);
box(output_win_,'|','-');
for(size_t i=0;i<msgs_.size();++i)
{
  PutStrToWin(output_win_,i+1,2,msgs_[i]);
}
wrefresh(output_win_);

}
//消息太长超过显示框怎么办  


//好友名字太长怎么办 
void Window::DrawFriendList()
{
int h=LINES*3/5;
int w=COLS*2/5;
int y=LINES/5;
int x=COLS*3/5;
friend_list_win_=newwin(h,w,y,x);
box(friend_list_win_ ,'|','-');
//TODO 遍历好友列表 把好友信息显示出来
size_t i=0;
for(auto item:friend_list_){
  PutStrToWin(friend_list_win_,i+1,1,item);
  i++;
}
wrefresh(friend_list_win_);
} 

//添加好友
void Window::AddFriend(const std::string& friend_info_){
      
  friend_list_.insert(friend_info_);
}
//删除好友
void Window::DelFriend(const std::string& friend_info_){
 friend_list_.erase(friend_info_);
}

void Window::AddMsg(const std::string& msg){
  //此处由我们的窗口显示消息有限，不能无限插入
  //当msg包含的消息超过一定数目 就把就消息删除掉，
  //如果想实现一个消息历史记录 就要把每次插入的消息顺便保存到文件中
  //用什么格式来组织文件 保存消息内容 谁发的  时间
  msgs_.push_back(msg);
  int max_line=LINES*3/5-2;
    if(max_line<3){
      printf("max_line too small\n");
      exit(0);
    }
  if((int)msgs_.size()>max_line){
    msgs_.pop_front();
  }
}

}//end client 

/////////////////////////////////////////////////////////////
//以下为测试代码

#ifdef  TEST_WINDOW


#include<unistd.h>
int main(){


  client::Window win;
  win.DrawHeader();
  win.DrawOutput();
  win.DrawInput();
  win.DrawFriendList();
  sleep(100);

  return 0;
}

#endif 
