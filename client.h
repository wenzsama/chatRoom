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
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 6666
#define MAXLEN 1000
#define OK 1
#define FAULT 0

struct message{
	char flag[15];
	char name[20];
	char msg[MAXLEN];
	char addressto[20];
	int size;
};
int qid=-1;
int fd=-1;
int sockfd=-1;
char localname[20];

