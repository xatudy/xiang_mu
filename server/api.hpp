
#pragma  once 
#include<string>
#include<jsoncpp/json/json.h>

namespace server{
//此处的Data相当于服务器给客户端提供的api
//此处的Data相当于服务器给客户端提供的api
struct Data{
 std::string name;
 std::string school;
 std::string msg;
 //对于当前情况下cmd的取值要么为空字符串 要么为"quit
 //如果为quit 表示客户端要退出
 std::string cmd;

//实现序列化函数
 void Serialize(std::string* output){
  //char buf[1024]={0};
 //sprintf(buf,"{name:%s,school:%s,msg:%s,cmd:%s}",name.c_str(),
 //     school.c_str(),msg.c_str(),cmd.c_str());
 // *output=buf;
 // 
 // 可以把json::value 近似理解为一个unordered_map
   Json::Value value;
   value["name"]=name;
   value["school"]=school;
   value["msg"]=msg;
   value["cmd"]=cmd;
   Json::FastWriter writer;
   *output=writer.write(value);


   return;
 
 }
//fan xu  lie  hua
 void UnSerialize(const std::string& input){
   Json::Value value;
   Json::Reader reader;
   reader.parse(input,value);//完成反序列化的函数
   //为了让代码更严谨 可以用isstring/isint来判断Json对象中
   //存储的数据类型是否符合要求
   if(value["name"].isString()){
      name=value["name"].asString();
   }
   school=value["school"].asString();
   msg=value["msg"].asString();
   cmd=value["cmd"].asString();
   return;
 }
};
}
