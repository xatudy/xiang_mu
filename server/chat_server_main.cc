
#include"chat_server.h"

int main(int argc,char* argv[]){

  if(argc!=3){
    printf("Usage: ./chat_server[ip][port]\n");
    return 1;
  }
  server::ChatServer server;
  server.Start(argv[1],atoi(argv[2]));
  return 0;
}
