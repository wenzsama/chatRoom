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

//������Ϣ�ṹ��
struct message{
	char flag[15];       //��Ϣ��־������ȷ����Ϣ��;
	char name[20];   //�û���
	char msg[MAXLEN]; //��Ϣ����
	char addressto[20];  //Ŀ���û�
	int size;             //��Ϣ���ݴ�С
};

//������������û��ṹ��
typedef struct _clientinf{
	char name[20];        //�û���
	struct sockaddr_in addr_in; //�׽��ֵ�ַ�ṹ
	int decr;      //�˿ͻ��˵�sock�ļ�������
	int speak;   //���Ա�־
}clientinf;

//����ṹ������ά����ǰ�������û������ڲ��ң���ӣ�ɾ����
typedef struct _listNode{
	clientinf data;
	struct _listNode * next;
}listNode;
