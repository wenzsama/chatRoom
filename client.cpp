#include  "client.h"

//login interface
int Interface(){
	int operate_num;
	do{
		system("clear");
		printf("--------------------------------------\n");
		printf("    welcome to chatRoom            \n");
		printf("         1.login                                     \n");
		printf("         2.register                                \n ");
		printf("          3.exit                                       \n");
		printf("----------------------------------------\n");
		printf("pls choose:\n");
		setbuf(stdin,NULL);
		scanf("%d",&operate_num);
		setbuf(stdin,NULL);
	}while((operate_num!=1)&&(operate_num)!=2&&(operate_num!=3));
	return operate_num;
}
// help menu
void help(){
	printf("-----------------------------------------");
	printf("deer %s,welcome:\n",localname);
	printf("all$hello--------------say to all\n");
	printf("kobe$hello------------say to kobe\n");
	printf("view$------------------view online user\n");
	printf("exit$----------------------leave chat room\n");
	printf("if you are admin:\n");
	printf("admin_kick$---------------kick someone\n");
	printf("admin_screen$------------forbid/no speak");
}

//register module
int Register(struct message* a){
	char password_t[MAXLEN];
	do
	{
		printf("pls input your name(less than 10):\n");
		memset((*a).name,0,strlen((*a).name));
		setbuf(stdin,NULL);
		scanf("%s",(*a).name);
	}while(strlen((*a).name)>20);
	while(1)
	{
		printf("pls input your password(less than 20):\n");
		memset((*a).msg,0,strlen((*a).msg));
		setbuf(stdin,NULL);
		scanf("%s",(*a).msg);
		printf("pls input your password again(less than 20):\n");
		memset(password_t,0,strlen(password_t));
		setbuf(stdin,NULL);
		scanf("%s",password_t);
		if(strcmp((*a).msg,password_t) != 0 || strlen((*a).msg)>20 || strlen(password_t)>20)
		{
			printf("password error！\n");
		}
		else
		{
			break;
		}
	}
	strcpy((*a).flag,"reg");
	send(sockfd,a,sizeof(*a),0);
	printf("registering,pls wait……\n");
	recv(sockfd,a,sizeof(*a),0);
	printf("recv the message from server:%s\n",(*a).msg);
	sleep(3);

}
//login module
void log_user(struct message *a){
	do
	{
		printf("pls input your name:(less than 10):\n");
		memset((*a).name,0,strlen((*a).name));
		scanf("%s",(*a).name);
		strcpy(localname,(*a).name);
		printf("pls input your password:\n");
		memset((*a).msg,0,strlen((*a).msg));
		scanf("%s",(*a).msg);
	}while(strlen((*a).name)>20 || strlen((*a).msg)>20);
	strcpy((*a).flag,"login");
	send(sockfd,a,sizeof(*a),0);                 
	printf("logining,pls wait……\n");
	recv(sockfd,a,sizeof(*a),0);
	printf("recv the message from server:%s\n",(*a).msg);
}
//receive module
void handlerecvmsg(void * sockfd){
	int connfd = *sockfd;
	int nread; 
	char buf[1024];
	char str[1024];
	struct message recvmsg;
	time_t timep;                              
	if((fd=open("chat.txt",O_RDWR) < 0)  )            
	{
		printf("fail to open chat record!");
		exit(1);
	}
	while(1)
	{
		nread = recv(connfd,&recvmsg,sizeof(struct message),0);     
		if(nread == 0)
		{
			printf("You have detached from the server!\n");     
			close(fd);
			close(connfd);
			exit(0);
		}
		else if (strcmp(recvmsg.flag,"all") == 0)            
		{
			time (&timep);
			memset(str,0,strlen(str));
			sprintf(str,"%s%s say to all: %s\n",ctime(&timep),recvmsg.name,recvmsg.msg);
			printf("%s",str);
			write(fd,str,strlen(str));                       
		}
		else if (strcmp(recvmsg.flag,"personal") == 0)         
		{
			time (&timep);
			memset(str,0,strlen(str));
			sprintf(str,"%s%s say to you: %s\n",ctime(&timep),recvmsg.name,recvmsg.msg);
			printf("%s",str);
			write(fd,str,strlen(str));                         
		}
		else if (strcmp(recvmsg.flag,"sermsg") == 0)          
		{
			time (&timep);
			memset(str,0,strlen(str));
			sprintf(str,"%s system info: %s\n",ctime(&timep),recvmsg.msg);
			printf("%s",str);
			write(fd,str,strlen(str));
			continue;
		}
		else if (strcmp(recvmsg.flag,"view") == 0)      
		{
			time (&timep);
			memset(str,0,strlen(str));
			sprintf(str,"%s online user:\n%s\n",ctime(&timep),recvmsg.msg);
			printf("%s",str);
			continue;
		}
		else
		{
			if(strcmp(recvmsg.flag,"") == 0)
			{
				continue;
			}
		}
	}
}

//deal with request chat string
void cutStr(char str[],char left[], int n, char right[],int max, char c)
{
	int i,k,j;
	for(i = 0 ; i < n ;i++)
	{
		if(str[i] == c)                             
			break;
	}
	if(i == n)
	{
		i = -1;
	}
	else
	{
		memset(left,0,strlen(left));
		for(k = 0 ; k < i ; k++)
		{
			left[k] = str[k];         
		}
	}
	for(j = i+1 ; j < max;j++)
	{
		if(str[j] == '\0')
			break;
		right[j-i-1] = str[j];     
	}
	left[i] = '\0';
	if(j < max)
	{
		right[j-i-1] = '\0';
	}	
	else
	{
		right[max] = '\0';
	}
}


//login success, send request
int login_success(struct message *a)
{
	char str[MAXLEN];
	char buf[MAXLEN];
	time_t timep;
	pthread_t pid;
	
	if((fd=open("chat.txt",O_RDWR)) < 0)
	{
		printf("failed to open chat record!");
		exit(1);
	}
	pthread_create(&pid,NULL,(void*)handlerecvmsg,(void *)&sockfd);         //创建接受消息线程
	setbuf(stdin,NULL);
	strcpy((*a).flag,"all");
	printf("welcome deer %s,input (help$) if need help",localname);
	while(1)
	{
		memset((*a).msg,0,strlen((*a).msg));
		memset(str,0,strlen(str));
		usleep(100000);
		setbuf(stdin,NULL);
		fgets(str,1024,stdin);
        strcpy((*a).name,locname);
	    strcpy(buf,(*a).flag);
	   cutStr(str,(*a).flag,15,(*a).msg,MAXLEN,'$');   
	    if(strcmp((*a).flag,"help")==0){
			help();
			continue;
		}
	if(strcmp((*a).flag,"exit") == 0)
	{
		return FAULT;
	}
	if(strcmp((*a).flag,"view") == 0)           
	{
		send(sockfd,a,sizeof((*a)),0);       
		strcpy((*a).flag,buf);
		continue;
	}
	if(strcmp((*a).flag,"all") == 0)
	{
	send(sockfd,a,sizeof(*a),0);        
		continue;
	}
	else
	{
		strcpy(buf,(*a).flag);
		strcpy((*a).addressto,(*a).flag);
		strcpy((*a).flag,"personal");
		send(sockfd,a,sizeof(*a),0);             
		strcpy((*a).flag,buf);
		time (&timep);
		memset(str,0,strlen(str));
		sprintf(str,"% you say to %s: %s\n",ctime(&timep),(*a).flag,(*a).msg);
		printf("%s",str);
		write(fd,str,strlen(str));              
	}
}
return OK;
}
//admin login success and send request
int login_admin(struct message *a){
	char str[MAXLEN];
	char buf[MAXLEN];
	time_t timep;
	pthread_t pid;

	if((fd=open("chat.txt",O_RDWR)) < 0)
	{
		printf("failed to open chat record!");
		exit(1);
	}
	pthread_create(&pid,NULL,(void*)handlerecvmsg,(void *)&sockfd);         //创建接受消息线程
	setbuf(stdin,NULL);
	strcpy((*a).flag,"all");
	printf("welcome deer %s,input (help$) if need help",localname);
	while(1)
	{
		memset((*a).msg,0,strlen((*a).msg));
		memset(str,0,strlen(str));
		usleep(100000);
		setbuf(stdin,NULL);
		fgets(str,1024,stdin);
		strcpy((*a).name,locname);
		strcpy(buf,(*a).flag);
		cutStr(str,(*a).flag,15,(*a).msg,MAXLEN,'$');   
		if(strcmp((*a).flag,"help")==0){
			help();
			continue;
		}
		if(strcmp((*a).flag,"exit") == 0)
		{
			return FAULT;
		}
		if(strcmp((*a).flag,"view") == 0)           
		{
			send(sockfd,a,sizeof((*a)),0);       
			strcpy((*a).flag,buf);
			continue;
		}
		if(strcmp((*a).flag,"all") == 0)
		{
			send(sockfd,a,sizeof(*a),0);        
			continue;
		}
		else if(strcmp((*a).flag,"admin_kick")==0){
			send(sockfd,a,sizeof(*a),0);
			continue;
		}
		else if(strcmp((*a).flag,"admin_screen")==0){
			send(sockfd,a,sizeof(*a),0);
			strcpy((*a).flag,buf);
			continue;
		}
		else
		{
			strcpy(buf,(*a).flag);
			strcpy((*a).addressto,(*a).flag);
			strcpy((*a).flag,"personal");
			send(sockfd,a,sizeof(*a),0);             
			strcpy((*a).flag,buf);
			time (&timep);
			memset(str,0,strlen(str));
			sprintf(str,"% you say to %s: %s\n",ctime(&timep),(*a).flag,(*a).msg);
			printf("%s",str);
			write(fd,str,strlen(str));              
		}
	}
	return OK;

}


//client main

int main(){
	int ret;
	int operate_num;
	char str[MAXLEN];
	char buf[MAXLEN];
	struct message msg;
	struct message a;
	struct sockaddr_in serv_addr;
	time_t timep;
	enum action {log=1,reg,ex};
	struct hostent* host;

	while(1){
		if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)){
			exit(-1);
		}

		memset(&serv_adr,0,sizeof(serv_addr));
		serv_addr.sin_family=AF_INET;
		serv_addr.sin_port=htons(PORT);
		serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
		if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
			printf("connecting the server,pls waiting..\n");
			exit(1);
		}
		operate_num=Interface();
		switch(operate_num){
		case log:{
				 int count=3;
				 while(count){
					 log_user(&a);
					 if(strcmp(a.msg,"hello,admin!")==0){
						 if(login_admin(&a)==0)
							 return FAULT;
					 }
					 if(strcmp(a.msg,"login,success!")==0){
						 if(login_success(&a)==0)
							 return FAULT;
					 }
					 else{
						 count--;
						 printf("another %d chance to try\n",count);
					 }
				 }
				 close(sockfd);
				 exit(3);
				 break;

				 }
		case reg:{
			Register(&a);
			break;
				 }
		case ex:{
		   
		   break;
	   }
	   default:break;

		}
	}
	close(sockfd);
	return 0;
}

