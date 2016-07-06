#include "server.h"
//构建全局链表，维护在线用户
listNode * clientlist=NULL;

//------------------------------------------
//链表查找、删除、添加函数实现
//查找
 listNode* findlist(listNode* phead,clientinf e){
	listNode* cur=phead->next;
	while(cur!=NULL){
		if(strcmp(cur->data.name,e.name)==0){
			return cur;
		}
		else
			cur=cur->next;
	}
	return cur;
 }
 //删除
 void deletelist(listNode* phead,clientinf e){
	 listNode* pre=phead;
	 listNode* cur=cur->next;
	 while(cur!=NULL){
		 if(strcmp(cur->data.name,e.name)!=0){
		   pre=cur;
		   cur=cur->next;
		 }
		 else
			 break;
	 }
	 listNode* temp=cur;
	  pre->next=cur->next;
	  free(temp);
 }
 //添加
 void insertend(listNode* phead,clientinf e){
	 listNode* cur=phead;
	 while(cur->next!=NULL)
		 cur=cur->next;
	 listNode* insert=(listNode*)malloc(sizeof(struct _listNode));
	 insert->next=NULL;
	 insert->data=e;
	 cur->next=insert;
 }

 //-------------------------------------------------------------------------------
 //服务器各项服务实现
 //----------------------------------------------------
 //1.注册服务---------
 int reg_check(struct message *recievemsg){
	 int fd;
	 int read_size,write_size;
	 struct message cmpmsg;
	 char *p_name;
	 char *p_passwore;
	 int rc,i,ncols;
	 sqlite3 *db;
	 sqlite3_stmt *stmt;
	 char sql[128] ;
	 const char *tail;
	 if(strlen(recievemsg->name)>20 || strlen(recievemsg->msg)>20 )
	 {
		 return REG_FAULT;
	 }
	 if(strcmp(recievemsg->name,"adminer")==0)
	 {
		 return REG_FAULT;
	 }
	 if(strcmp(recievemsg->name,"all")==0)
	 {
		 return REG_FAULT;
	 }
	 if(strcmp(recievemsg->name,"reg")==0)
	 {
		 return REG_FAULT;
	 }
	 if(strcmp(recievemsg->name,"login")==0)
	 {
		 return REG_FAULT;
	 }
	 if(strcmp(recievemsg->name,"trans")==0)
	 {
		 return REG_FAULT;
	 }
	 //打开数据库
	 rc = sqlite3_open("chat.db",&db);
	 if(rc)
	 { 
		 fprintf(stderr,"can't open databse:%s",sqlite3_errmsg(db));
		 sqlite3_close(db);
		 return REG_FAULT;
	 }      
	 //检查重名退出
	 memset(sql,0,sizeof(sql));
	 sprintf(sql,"select * from member where name='%s';",(recievemsg->name));
	 rc = sqlite3_prepare(db,sql,strlen(sql),&stmt,&tail);
	 if(rc != SQLITE_OK)
	 {
		 fprintf(stderr,"SQLerror:%s",sqlite3_errmsg(db));
	 }
	 //执行
	 rc = sqlite3_step(stmt);
	 ncols = sqlite3_column_count(stmt);
	 while (rc == SQLITE_ROW)
	 {
		 rc = sqlite3_step(stmt);

		 sqlite3_finalize(stmt);
		 sqlite3_close(db);   
		 return REG_FAULT;
	 }
	 //销毁stmt
	 sqlite3_finalize(stmt);
	 //注册进数据库
	 memset(sql,0,sizeof(sql));
	 sprintf(sql,"insert into member (name,password) values(?,?);");
	 rc = sqlite3_prepare(db,sql,strlen(sql),&stmt,&tail);
	 if(rc != SQLITE_OK)
	 {
		 fprintf(stderr,"SQLerror:%s",sqlite3_errmsg(db));
	 }
	 p_name = recievemsg->name;
	 sqlite3_bind_text(stmt,1,p_name,strlen(p_name),NULL);
	 p_passwore = recievemsg->msg;
	 sqlite3_bind_text(stmt,2,p_passwore,strlen(p_passwore),NULL);  
	 rc = sqlite3_step(stmt);
	 sqlite3_finalize(stmt);

	 //关闭
	 sqlite3_close(db);
	 return REG_OK;
 }
 int server_reg(struct message *a, clientinf *clientNode)
 {
	 int i;
	 char buf[MAXLEN];
	 i = reg_check(a);
	 if(i == 1)
	 {
		 printf("%s succeed to register!\n",(*a).name);
		 strcpy((*a).msg,"register,success!");
		 send((*clientNode).decr,a,sizeof(struct message),0);
	 }
	 else
	 {
		 printf("% fail to register\n",(*a).name);
		 strcpy((*a).msg,"register,failure!");
		 send((*clientNode).decr,a,sizeof(struct message),0);
	 }
 }

 //2.登录服务：包含普通用户登录与管理员登录------------
 //禁止重复登录
 int overlap(struct message a){
	listNode * cur=clientlist->next;
	while(cur!=NULL){
	   if(strcmp(cur->data.name,a.name)==0)
		   return 1;
	   else
		   cur=cur->next;
	}
	return 0;
 }
 //查看数据库，看是否有用户信息
 int login_check(struct message *recievemsg){
	 int fd;
	 int read_size,write_size;
	 struct message cmpmsg;
	 char *p_name;
	 char *p_passwore;
	 int rc,i,ncols;
	 sqlite3 *db;
	 sqlite3_stmt *stmt;
	 char sql[128] ;
	 const char *tail;

	 //打开数据库
	 rc = sqlite3_open("chat.db",&db);     
	 if(rc)
	 { 
		 fprintf(stderr,"can't open databse:%s",sqlite3_errmsg(db));
		 sqlite3_close(db);
		 return LOG_FAULT;
	 }      
	 //检查用户名密码
	 memset(sql,0,sizeof(sql));
	 sprintf(sql,"select * from member where name='%s' and password='%s';",(recievemsg->name),(recievemsg->msg));
	 rc = sqlite3_prepare(db,sql,strlen(sql),&stmt,&tail);
	 if(rc != SQLITE_OK)
	 {
		 fprintf(stderr,"SQLerror:%s",sqlite3_errmsg(db));
	 }
	 //执行
	 rc = sqlite3_step(stmt);
	 ncols = sqlite3_column_count(stmt);
	 while (rc == SQLITE_ROW)
	 {
		 rc = sqlite3_step(stmt);
		 sqlite3_finalize(stmt);
		 sqlite3_close(db);
		 if(strcmp(recievemsg->name,"admin") == 0)
		 {
			 return LOG_ADMIN;
		 }
		 else
		 {
			 return LOG_USER;
		 }

	 }
	 //销毁stmt
	 sqlite3_finalize(stmt);
	 //关闭数据库
	 sqlite3_close(db);
	 return LOG_FAULT;
 }
 int server_login(struct message *a, clientinf *clientNode){
	 int i;
	 char buf[MAXLEN];
	 i = login_check(a);
	 if(i == 1)
	 {
		 printf("admin succeed to login!\n");
		 strcpy((*a).msg,"hello,admin!");
		 strcpy((*clientNode).name,(*a).name);
		 insertend(clientlist,*clientNode);
		 send((*clientNode).decr,a,sizeof(struct message),0);
	 }
	 else
	 {
		 if(i == 0)
		 {
			 printf("%s succeed to login\n",(*a).name);
			 memset((*a).msg,0,strlen((*a).msg));
			 strcpy((*a).msg,"login,success!");
			 strcpy((*clientNode).name,(*a).name);
			 insertend(clientlist,*clientNode);
			 send((*clientNode).decr,a,sizeof(struct message),0);

			listNode* cur=clientlist->next;
			 strcpy((*a).flag,"sermsg");
			 sprintf((*a).msg,"welcome %s to the chat room",(*a).name);
			 while(cur!= NULL)
			 {
				 send(cur->data.decr,a,sizeof(struct message),0);
				 cur=cur->next;
			 }
		 }
		 else
		 {
			 printf("%s fail to login!\n",(*a).name);
			 strcpy((*a).msg,"login,failure!");
			 send((*clientNode).decr,a,sizeof(struct message),0);
		 }
	 }
 }
 //用户离开聊天室
 int server_exit(struct message *a, clientinf *clientNode)  {
   listNode* cur=clientlist->next;
	 while(cur!= NULL)
	 {
		 if(cur->data.decr == (*clientNode).decr)
		 {						
			 strcpy((*clientNode).name,cur->data.name);
			 break;
		 }
		 else
		 {
			 cur= cur->next;
		 }
	 }
	 strcpy((*a).flag,"sermsg");
	 printf("%s leave the chat room\n",(*clientNode).name);
	 deletelist(clientlist ,(*clientNode));
	 
	 //告知其他在线用户
	 cur=clientlist->next;
	 sprintf((*a).msg,"%s leave the chat room!",(*clientNode).name);
	 while(cur != NULL)
	 {
		 send(cur->data.decr,a,sizeof(struct message),0);
		 cur=cur->next;
	 }
 }

 //聊天功能，分为私聊和群聊----------------------------
 int server_all(struct message *a, clientinf *clientNode){
	 clientinf e;
	 strcpy(e.name,(*a).name);
	 listNode* cur = findlist(clientlist,e);
	 if(cur!= NULL)
	 {
		 if(cur->data.speak == 0)
		 {
			 strcpy((*a).flag,"sermsg");
			 sprintf((*a).msg,"sorry,you are not allowed to speak by the admin!");
			 send(cur->data.decr,a,sizeof(struct message),0);
			 return 0;
		 }
	 }
	 if (strcmp((*a).msg,"") != 0)
	 {
		 cur=clientlist->next;
		 strcpy((*a).name,(*clientNode).name);
		 while(cur != NULL)
		 {
			 send(cur->data.decr,a,sizeof(struct message),0);
			 cur=cur->next;
		 }
	 }
	 return 1;
 }

 int server_personal(struct message *a, clientinf *clientNode){
	 clientinf e;
	 strcpy(e.name,(*a).name); 
	 listNode* cur = findlist(clientlist,e);
	 if(cur!= NULL)
	 {
		 if(cur->data.speak == 0)
		 {
			 strcpy((*a).flag,"sermsg");
			 sprintf((*a).msg,"sorry,you are not allowed to speak by the admin!");
			 send(cur->data.decr,a,sizeof(struct message),0);
			 return 0;
		 }
	 }
	 cur=clientlist->next;
	 strcpy(e.name,(*a).addressto);
	 cur= findlist(clientlist,e);
	 if(cur == NULL)
	 {
		 strcpy((*a).flag,"sermsg");
		 sprintf((*a).msg,"the user is offline\n");
		 send((*clientNode).decr,a,sizeof(struct message),0);
		 return 0;
	 }
	 else
	 {
		 if (strcmp((*a).msg,"") != 0)
		 {
			 send(cur->data.decr,a,sizeof(struct message),0);
		 }
	 }
	 return 1;
 }

 //查看当前在线用户
 int server_view(struct message *a, clientinf *clientNode){
	 char str[MAXLEN],buf[MAXLEN];
	 int i = 1;
	 listNode* cur=clientlist->next;
	 memset(buf,0,strlen(buf));
	 while(cur != NULL)
	 {
		 memset(str,0,strlen(str));
		 if(cur->data.speak==1)
		 {
			 sprintf(str,"%d. %s (online)\n",i,cur->data.name);
			 strcat(buf,str);
		 }
		 else
		 {
			 sprintf(str,"%d. %s （no speak）\n",i,cur->data.name);
			 strcat(buf,str);
		 }
		 cur = cur->next;
		 i++;
	 }
	 strcpy((*a).name,(*clientNode).name);
	 strcpy((*a).msg,buf);
	 send((*clientNode).decr,a,sizeof(struct message),0);
 }

 //管理员踢人 与禁言---------------------------------------------
 int server_admin_kick(struct message *a, clientinf *clientNode)
 {
	 int closefd;
	 clientinf e;
	 strcpy(e.name,(*a).msg);
	
	 listNode* cur= findlist(clientlist,e);
	 if(cur == NULL)
	 {
		 strcpy((*a).flag,"sermsg");
		 sprintf((*a).msg,"the user is offline\n");
		 send((*clientNode).decr,a,sizeof(struct message),0);
		 return 0;
	 }
	 else
	 {
		 closefd=cur->data.decr;
		 close(closefd);
		 deletelist(clientlist,e);
		cur=clientlist->next;
		 strcpy((*a).flag,"sermsg");
		 sprintf((*a).msg,"%s is kicked out from the chat by admin!",(*a).msg);
		 while(cur != NULL)
		 {
			 send(cur->data.decr,a,sizeof(struct message),0);
			 cur=cur->next;
		 }
	 }   
	 return closefd;
 }
 int server_admin_screen(struct message *a, clientinf *clientNode)
 {
	 clientinf e;
	 strcpy(e.name,(*a).msg); 
	 listNode* cur= findlist(clientlist,e);
	 if(cur == NULL)
	 {
		 strcpy((*a).flag,"sermsg");
		 sprintf((*a).msg,"the user is offline\n");
		 send((*clientNode).decr,a,sizeof(struct message),0);
		 return 0;
	 }
	 else
	 {
		 cur->data.speak = (cur->data.speak+1)%2;
		 strcpy((*a).flag,"sermsg");
		 if(cur->data.speak == 0)
		 {
			 sprintf((*a).msg,"%s  is forbid to talk by the admin!",(*a).msg);
		 }
		 else
		 {
			 sprintf((*a).msg,"%s can chat again now!\n",(*a).msg);
		 }
		 cur=clientlist->next;
		 while(cur != NULL)
		 {
			 send(L->data.decr,a,sizeof(struct message),0);
			cur=cur->next;
		 }
	 }
	 return 1;
 }


 //聊天服务器主程序
 int main(){
	 int re;   //recv（）的return值
	 int fd;  //file 描述符
	 struct message a;  //消息结构
	 clientinf e;
	 char buf[MAXLEN],str[MAXLEN];
	 time_t timep;
	 clientinf clientNode;
	 clientNode.speak = 1;
	 strcpy(clientNode.name,"***");
	 clientlist=(listNode*)malloc(sizeof(struct _listNode));//初始化链表头
	 if((fd=open("chat_log_server.txt",O_RDWR|O_CREAT|O_APPEND,0777)) < 0)
	 {
		 printf("failure to open chat_log_server!");
		 exit(1);
	 }
	 int lfd; //server socket
	 int cfd;//client socket
	 int sfd;  //current server client socket
	 int rdy; //select() return,socket read happen
	 struct sockaddr_in sin; //server
	 struct sockaddr_in cin; //client
	 int client[FD_SETSIZE];  //客户端连接的套接字描述符数组 
	 int maxi;
	 int maxfd;                        // 最大连接数 
	 fd_set rset;
	 fd_set allset;
	 socklen_t addr_len;         //地址结构长度 
	 char buffer[MAX_LINE];
	 int i;
	 int n;
	 int len;
	 int opt = 1;   //套接字选项 
	 char addr_p[20];
	 //对server_addr_in  结构进行赋值  
	 bzero(&sin,sizeof(struct sockaddr_in));   // 先清零 
	 sin.sin_family=AF_INET;
	 sin.sin_addr.s_addr=htonl(INADDR_ANY);  //表示接受任何ip地址   将ip地址转换成网络字节序
	 sin.sin_port=htons(PORT);         //将端口号转换成网络字节序
	 // 调用socket函数创建一个TCP协议套接口 
	 if((lfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
	 {
		 fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
		 exit(1);

		 //设置套接字选项 使用默认选项
		 setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		 //调用bind函数 将serer_addr结构绑定到sockfd上 
		 if(bind(lfd,(struct sockaddr *)(&sin),sizeof(struct sockaddr))==-1)
		 {
			 fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
			 exit(1);
		 }
		 //开始监听端口   等待客户的请求 
		 if(listen(lfd,20)==-1)
		 {
			 fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
			 exit(1);
		 }
		 printf("connect.......\n");

		 maxfd = lfd;      //对最大文件描述符进行初始化
		 maxi = -1;
		 //初始化客户端连接描述符集合
		 for(i = 0;i < FD_SETSIZE;i++)
		 {
			 client[i] = -1;
		 }
		 FD_ZERO(&allset);                     // 清空文件描述符集合 
		 FD_SET(lfd,&allset);                 // 将监听字设置在集合内 
		 //开始服务程序的死循环 
		 while(1)
		 {
			 rset = allset;
			 //得到当前可以读的文件描述符数
			 rdy = select(maxfd + 1, &rset, NULL, NULL, NULL);
			 if(FD_ISSET(lfd, &rset))
			 {
				 addr_len = sizeof(sin);

				 //接受客户端的请求 
				 if((cfd=accept(lfd,(struct sockaddr *)(&cin),&addr_len))==-1)
				 {
					 fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
					 exit(1);
				 }
				 //查找一个空闲位置
				 for(i = 0; i<FD_SETSIZE; i++)
				 {
					 if(client[i] <= 0)
					 {
						 client[i] = cfd;   // 将处理该客户端的连接套接字设置到该位置 
						 break;
					 }
				 }
				 //太多的客户端连接   服务器拒绝俄请求  跳出循环 
				 if(i == FD_SETSIZE)
				 {
					 printf("too many clients");
					 exit(1);
				 }
				 FD_SET(cfd, &allset);     // 设置连接集合 
				 if(cfd > maxfd)                  //新的连接描述符 
				 {
					 maxfd = cfd;
				 }
				 if(i > maxi)
				 {
					 maxi = i;
				 }
				 if(--rdy <= 0)                //减少一个连接描述符 
				 {
					 continue;
				 }
			 }
			 //对每一个连接描述符做处理 
			 for(i = 0;i< FD_SETSIZE;i++)
			 {
				 if((sfd = client[i]) < 0)
				 {
					 continue;
				 }

				 if(FD_ISSET(sfd, &rset))
				 {
					 //如果没有可以读的套接字   退出循环
					 if(--rdy < 0)
					 {
						 break;
					 }
					 re = recv(sfd,&a,sizeof(a)+1,0);
					 clientNode.decr = sfd;
					 strcpy(clientNode.name,a.name);
					 if(re == 0)                                //客户端断开连接
					 {
						 server_exit(&a, &clientNode) ;
						 fflush(stdout);                                    // 刷新 输出终端 
						 close(sfd);
						 FD_CLR(sfd, &allset);                        //清空连接描述符数组
						 client[i] = -1;
					 }
					 else
					 {
						 if(strcmp(a.flag,"login") == 0)      //客户端请求登录
						 {
							 if(overlap(a) == 0)               //若该用户名不在线
							 {
								 server_login(&a,&clientNode);    //处理登录请求
							 }
							 else                                 //若该用户名已在线
							 {
								 strcpy(a.msg,"overlap");
								 send(clientNode.decr,&a,sizeof(struct message),0);/*返回重名提示*/
							 }
							 continue;
						 }
						 else if(strcmp(a.flag,"reg") == 0)   //客户端请求注册
						 {
							 server_reg(&a,&clientNode);       //处理注册请求
							 continue;
						 }
						 else if (strcmp(a.flag,"all") == 0)    //客户端请求广播
						 {
							 if(server_all(&a,&clientNode) == 1)   //处理广播请求
							 {
								 memset(str,0,strlen(str));
								 time (&timep);
								 sprintf(str,"%s%s TO %s: %s\n",ctime(&timep),a.name,a.flag,a.msg);
								 printf("%s",str);
								 write(fd,str,strlen(str));      //将广播写入聊天记录
							 }
							 continue;
						 }
						 else if(strcmp(a.flag,"personal") == 0)  //客户端请求私聊
						 {
							 if(server_personal(&a,&clientNode) == 1) //处理私聊请求
							 {
								 memset(str,0,strlen(str));
								 time (&timep);
								 sprintf(str,"%s%s TO %s: %s\n",ctime(&timep),a.name,a.addressee,a.msg);
								 printf("%s",str);
								 my_write(fd,str,strlen(str));       /*将私聊写入聊天记录*/
							 }
						 }
						 else if(strcmp(a.flag,"view") == 0)    /*客户端请求查看在线用户*/
						 {
							 server_view(&a,&clientNode);        /*处理查看在线用户请求*/
							 continue;
						 }
						 else if(strcmp(a.flag,"admin_kick") == 0) //管理员替人请求
						 {
							 int closefd;
							 closefd = server_admin_kick(&a,&clientNode);  //处理管理员替人请求
							 FD_CLR(closefd, &allset);                      //清空连接描述符数组
							 int j;
							 for(j = 0;j< FD_SETSIZE;j++)
							 {
								 if(closefd == client[j])
								 {
									 client[j]=-1;
									 break;
								 }
							 }
							 continue;
						 }
						 else if(strcmp(a.flag,"admin_screen") == 0)//管理员禁言请求
						 {
							 server_admin_screen(&a,&clientNode);//处理管理员禁言请求
							 continue;
						 }

					 }
				 }
			 }
		 }
		 close(lfd);
		 return 0;


	 }







 


 


