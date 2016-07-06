#include "server.h"
//����ȫ������ά�������û�
listNode * clientlist=NULL;

//------------------------------------------
//������ҡ�ɾ������Ӻ���ʵ��
//����
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
 //ɾ��
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
 //���
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
 //�������������ʵ��
 //----------------------------------------------------
 //1.ע�����---------
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
	 //�����ݿ�
	 rc = sqlite3_open("chat.db",&db);
	 if(rc)
	 { 
		 fprintf(stderr,"can't open databse:%s",sqlite3_errmsg(db));
		 sqlite3_close(db);
		 return REG_FAULT;
	 }      
	 //��������˳�
	 memset(sql,0,sizeof(sql));
	 sprintf(sql,"select * from member where name='%s';",(recievemsg->name));
	 rc = sqlite3_prepare(db,sql,strlen(sql),&stmt,&tail);
	 if(rc != SQLITE_OK)
	 {
		 fprintf(stderr,"SQLerror:%s",sqlite3_errmsg(db));
	 }
	 //ִ��
	 rc = sqlite3_step(stmt);
	 ncols = sqlite3_column_count(stmt);
	 while (rc == SQLITE_ROW)
	 {
		 rc = sqlite3_step(stmt);

		 sqlite3_finalize(stmt);
		 sqlite3_close(db);   
		 return REG_FAULT;
	 }
	 //����stmt
	 sqlite3_finalize(stmt);
	 //ע������ݿ�
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

	 //�ر�
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

 //2.��¼���񣺰�����ͨ�û���¼�����Ա��¼------------
 //��ֹ�ظ���¼
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
 //�鿴���ݿ⣬���Ƿ����û���Ϣ
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

	 //�����ݿ�
	 rc = sqlite3_open("chat.db",&db);     
	 if(rc)
	 { 
		 fprintf(stderr,"can't open databse:%s",sqlite3_errmsg(db));
		 sqlite3_close(db);
		 return LOG_FAULT;
	 }      
	 //����û�������
	 memset(sql,0,sizeof(sql));
	 sprintf(sql,"select * from member where name='%s' and password='%s';",(recievemsg->name),(recievemsg->msg));
	 rc = sqlite3_prepare(db,sql,strlen(sql),&stmt,&tail);
	 if(rc != SQLITE_OK)
	 {
		 fprintf(stderr,"SQLerror:%s",sqlite3_errmsg(db));
	 }
	 //ִ��
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
	 //����stmt
	 sqlite3_finalize(stmt);
	 //�ر����ݿ�
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
 //�û��뿪������
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
	 
	 //��֪���������û�
	 cur=clientlist->next;
	 sprintf((*a).msg,"%s leave the chat room!",(*clientNode).name);
	 while(cur != NULL)
	 {
		 send(cur->data.decr,a,sizeof(struct message),0);
		 cur=cur->next;
	 }
 }

 //���칦�ܣ���Ϊ˽�ĺ�Ⱥ��----------------------------
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

 //�鿴��ǰ�����û�
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
			 sprintf(str,"%d. %s ��no speak��\n",i,cur->data.name);
			 strcat(buf,str);
		 }
		 cur = cur->next;
		 i++;
	 }
	 strcpy((*a).name,(*clientNode).name);
	 strcpy((*a).msg,buf);
	 send((*clientNode).decr,a,sizeof(struct message),0);
 }

 //����Ա���� �����---------------------------------------------
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


 //���������������
 int main(){
	 int re;   //recv������returnֵ
	 int fd;  //file ������
	 struct message a;  //��Ϣ�ṹ
	 clientinf e;
	 char buf[MAXLEN],str[MAXLEN];
	 time_t timep;
	 clientinf clientNode;
	 clientNode.speak = 1;
	 strcpy(clientNode.name,"***");
	 clientlist=(listNode*)malloc(sizeof(struct _listNode));//��ʼ������ͷ
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
	 int client[FD_SETSIZE];  //�ͻ������ӵ��׽������������� 
	 int maxi;
	 int maxfd;                        // ��������� 
	 fd_set rset;
	 fd_set allset;
	 socklen_t addr_len;         //��ַ�ṹ���� 
	 char buffer[MAX_LINE];
	 int i;
	 int n;
	 int len;
	 int opt = 1;   //�׽���ѡ�� 
	 char addr_p[20];
	 //��server_addr_in  �ṹ���и�ֵ  
	 bzero(&sin,sizeof(struct sockaddr_in));   // ������ 
	 sin.sin_family=AF_INET;
	 sin.sin_addr.s_addr=htonl(INADDR_ANY);  //��ʾ�����κ�ip��ַ   ��ip��ַת���������ֽ���
	 sin.sin_port=htons(PORT);         //���˿ں�ת���������ֽ���
	 // ����socket��������һ��TCPЭ���׽ӿ� 
	 if((lfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
	 {
		 fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
		 exit(1);

		 //�����׽���ѡ�� ʹ��Ĭ��ѡ��
		 setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		 //����bind���� ��serer_addr�ṹ�󶨵�sockfd�� 
		 if(bind(lfd,(struct sockaddr *)(&sin),sizeof(struct sockaddr))==-1)
		 {
			 fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
			 exit(1);
		 }
		 //��ʼ�����˿�   �ȴ��ͻ������� 
		 if(listen(lfd,20)==-1)
		 {
			 fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
			 exit(1);
		 }
		 printf("connect.......\n");

		 maxfd = lfd;      //������ļ����������г�ʼ��
		 maxi = -1;
		 //��ʼ���ͻ�����������������
		 for(i = 0;i < FD_SETSIZE;i++)
		 {
			 client[i] = -1;
		 }
		 FD_ZERO(&allset);                     // ����ļ����������� 
		 FD_SET(lfd,&allset);                 // �������������ڼ����� 
		 //��ʼ����������ѭ�� 
		 while(1)
		 {
			 rset = allset;
			 //�õ���ǰ���Զ����ļ���������
			 rdy = select(maxfd + 1, &rset, NULL, NULL, NULL);
			 if(FD_ISSET(lfd, &rset))
			 {
				 addr_len = sizeof(sin);

				 //���ܿͻ��˵����� 
				 if((cfd=accept(lfd,(struct sockaddr *)(&cin),&addr_len))==-1)
				 {
					 fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
					 exit(1);
				 }
				 //����һ������λ��
				 for(i = 0; i<FD_SETSIZE; i++)
				 {
					 if(client[i] <= 0)
					 {
						 client[i] = cfd;   // ������ÿͻ��˵������׽������õ���λ�� 
						 break;
					 }
				 }
				 //̫��Ŀͻ�������   �������ܾ�������  ����ѭ�� 
				 if(i == FD_SETSIZE)
				 {
					 printf("too many clients");
					 exit(1);
				 }
				 FD_SET(cfd, &allset);     // �������Ӽ��� 
				 if(cfd > maxfd)                  //�µ����������� 
				 {
					 maxfd = cfd;
				 }
				 if(i > maxi)
				 {
					 maxi = i;
				 }
				 if(--rdy <= 0)                //����һ������������ 
				 {
					 continue;
				 }
			 }
			 //��ÿһ������������������ 
			 for(i = 0;i< FD_SETSIZE;i++)
			 {
				 if((sfd = client[i]) < 0)
				 {
					 continue;
				 }

				 if(FD_ISSET(sfd, &rset))
				 {
					 //���û�п��Զ����׽���   �˳�ѭ��
					 if(--rdy < 0)
					 {
						 break;
					 }
					 re = recv(sfd,&a,sizeof(a)+1,0);
					 clientNode.decr = sfd;
					 strcpy(clientNode.name,a.name);
					 if(re == 0)                                //�ͻ��˶Ͽ�����
					 {
						 server_exit(&a, &clientNode) ;
						 fflush(stdout);                                    // ˢ�� ����ն� 
						 close(sfd);
						 FD_CLR(sfd, &allset);                        //�����������������
						 client[i] = -1;
					 }
					 else
					 {
						 if(strcmp(a.flag,"login") == 0)      //�ͻ��������¼
						 {
							 if(overlap(a) == 0)               //�����û���������
							 {
								 server_login(&a,&clientNode);    //�����¼����
							 }
							 else                                 //�����û���������
							 {
								 strcpy(a.msg,"overlap");
								 send(clientNode.decr,&a,sizeof(struct message),0);/*����������ʾ*/
							 }
							 continue;
						 }
						 else if(strcmp(a.flag,"reg") == 0)   //�ͻ�������ע��
						 {
							 server_reg(&a,&clientNode);       //����ע������
							 continue;
						 }
						 else if (strcmp(a.flag,"all") == 0)    //�ͻ�������㲥
						 {
							 if(server_all(&a,&clientNode) == 1)   //����㲥����
							 {
								 memset(str,0,strlen(str));
								 time (&timep);
								 sprintf(str,"%s%s TO %s: %s\n",ctime(&timep),a.name,a.flag,a.msg);
								 printf("%s",str);
								 write(fd,str,strlen(str));      //���㲥д�������¼
							 }
							 continue;
						 }
						 else if(strcmp(a.flag,"personal") == 0)  //�ͻ�������˽��
						 {
							 if(server_personal(&a,&clientNode) == 1) //����˽������
							 {
								 memset(str,0,strlen(str));
								 time (&timep);
								 sprintf(str,"%s%s TO %s: %s\n",ctime(&timep),a.name,a.addressee,a.msg);
								 printf("%s",str);
								 my_write(fd,str,strlen(str));       /*��˽��д�������¼*/
							 }
						 }
						 else if(strcmp(a.flag,"view") == 0)    /*�ͻ�������鿴�����û�*/
						 {
							 server_view(&a,&clientNode);        /*����鿴�����û�����*/
							 continue;
						 }
						 else if(strcmp(a.flag,"admin_kick") == 0) //����Ա��������
						 {
							 int closefd;
							 closefd = server_admin_kick(&a,&clientNode);  //�������Ա��������
							 FD_CLR(closefd, &allset);                      //�����������������
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
						 else if(strcmp(a.flag,"admin_screen") == 0)//����Ա��������
						 {
							 server_admin_screen(&a,&clientNode);//�������Ա��������
							 continue;
						 }

					 }
				 }
			 }
		 }
		 close(lfd);
		 return 0;


	 }







 


 


