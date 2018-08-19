//单例模式



//饿汉模式的单例
    //xxxx;
class Singleton{
private:
  class Object{

private:
    int data;
    //xxxx;
};
    static Object obj;
public:
 static Object* Instance(){
    return  &obj;
  }
};
