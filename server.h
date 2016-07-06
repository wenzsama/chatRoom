#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sqlite3.h>

#define MAXLEN 1000
#define PORT 6666
#define MAX_LINE 80
#define REG_OK 1
#define REG_FAULT 0
#define LOG_USER 0
#define LOG_ADMIN 1

//传递消息结构体
struct message{
	char flag[15];       //消息标志，用于确定消息用途
	char name[20];   //用户名
	char msg[MAXLEN]; //消息内容
	char addressto[20];  //目的用户
	int size;             //消息内容大小
};

//服务器服务的用户结构体
typedef struct _clientinf{
	char name[20];        //用户名
	struct sockaddr_in addr_in; //套接字地址结构
	int decr;      //此客户端的sock文件描述符
	int speak;   //禁言标志
}clientinf;

//链表结构，用来维护当前的在线用户，用于查找，添加，删除等
typedef struct _listNode{
	clientinf data;
	struct _listNode * next;
}listNode;
