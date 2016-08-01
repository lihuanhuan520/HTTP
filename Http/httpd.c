#include"httpd.h"

static void usage(const char* proc)
{
	printf("Usage: %s [PROT]\n",proc);
}

void not_found(int client)
{
	char buf[1024];
	sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"Content-type: text/html\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"\r\n ");
	send(client,buf,strlen(buf),0);

	sprintf(buf,"<html><title>NOT FOUND</title>\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"<body><p>file is not exist</p></body>\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"</html>\r\n ");
	send(client,buf,strlen(buf),0);
}

void printf_debug(const char* msg)
{
#ifdef _DEBUG_
	printf("%s\n",msg);
#endif
}

void bad_request(int client)
{

	char buf[_BLOCK_SIZE_];

	memset(buf,'\0',sizeof(buf));
	sprintf(buf,"HTTP/1.0 400 BAD REQUEST\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"Content-type: text/html\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"\r\n ");
	send(client,buf,strlen(buf),0);

	sprintf(buf,"<html><title>BAD REQUEST</title>\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"<body></br><p>your enter message is a bad request</p></br></body>\r\n ");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"</html>\r\n ");
	send(client,buf,strlen(buf),0);

}

void return_errno_client(int client, int status_code)
{
	switch(status_code){
		case 400:
			bad_request(client);
			break;
		case 404:
			not_found(client);
			break;
        default:
			break;
	}
}

void print_log(const char *fun,int line,int err_no,const char* err_str)
{
	printf("[%s: %d] [%d] [%s]\n",fun,line,err_no,err_str);
}

int get_line(int client,char buf[],int size)
{
	if(!buf||size==0)
	{
		return -1;
	}
	int i=0;
	char c='\0';
	int n=0;
	while((i<size-1)&&(c!='\n')){
		n=recv(client,&c,1,0);
	if(n>0){
			if(c=='\r'){
				n=recv(client,&c,1,MSG_PEEK);
		    	if(n>0 && c=='\n'){
				recv(client,&c,1,0);
		    	}
			else{// \r
				c='\n';
		    	}
			}
		buf[i++]=c;
	}
	else{
		c='\n';
	}
	}
	buf[i]='\0';
	return i;
}

int startup(int port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		print_log(__FUNCTION__, __LINE__, errno, strerror(errno));
		exit(2);
	}
	int flags=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&flags,sizeof(flags));
    struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0){
		print_log(__FUNCTION__, __LINE__, errno, strerror(errno));
		exit(3);
	}
	if(listen(sock,_BANK_LOG_)<0){
		print_log(__FUNCTION__, __LINE__, errno, strerror(errno));
		exit(4);
	}
	return sock;
}

void clear_header(int client)
{
	char buf[_BLOCK_SIZE_];
	int ret=-1;
	do{
		ret=get_line(client,buf,sizeof(buf)-1);
	}while(ret>0&& strcmp(buf,"\n")!=0);
}

void echo_html(int client, char *path,size_t size)
{
	clear_header(client);
	int fd=open(path,O_RDONLY,0);
	if(fd<0){
	//	return_errno_client(client,error);
		return;
	}else{
		char buf[_BLOCK_SIZE_];
		memset(buf,'\0',sizeof(buf));
		sprintf(buf,"HTTP/1.0 200 OK\r\n\r\n");
		send(client,buf,strlen(buf),0);
		sendfile(client,fd,NULL,size);
	}
	close(fd);
}

void exe_cgi(int client,const char* path,const char* method,const char* query_string)
{
	int numchars=-1;
	char buf[_BLOCK_SIZE_];
	int content_length=-1;
	int cgi_input[2]={0,0};
	int cgi_output[2]={0,0};
	pid_t id;

	if(strcasecmp(method,"GET")==0){//GET
		clear_header(client);
	}else{//CGI  && POST
		do{
			memset(buf,'\0',sizeof(buf));
			numchars=get_line(client,buf,sizeof(buf)-1);
			if(strncasecmp(buf,"Content-Length:",15)==0){
				content_length=atoi(&buf[16]);
			}
		}while(numchars>0 && strcmp("\n",buf)!=0);
		if(content_length==-1){
	//		return_errno_client();
			return;
		}
	}
	sprintf(buf,"HTTP/1.0 200 OK\r\n\r\n");
	send(client,buf,strlen(buf),0);
	if(pipe(cgi_input)<0){
	//	return_errno_client();
		return;
	}
	if(pipe(cgi_output)<0){
		close(cgi_input[0]);
		close(cgi_input[1]);
	//	return_errno_client();
		return;
	}
	id = fork();
	if(id < 0){
		close(cgi_input[0]);
		close(cgi_input[1]);
		close(cgi_output[0]);
		close(cgi_output[1]);
	//	return_errno_client();
		return;
	}else if(id==0){//child
		char meth_env[_BLOCK_SIZE_/10];
		char query_env[_BLOCK_SIZE_];
		char content_len_env[_BLOCK_SIZE_/5];
		memset(meth_env,'\0',sizeof(meth_env));
		memset(query_env,'\0',sizeof(query_env));
		memset(content_len_env,'\0',sizeof(content_len_env));
		close(cgi_input[1]);
		close(cgi_output[0]);
		//0->input[0]
		dup2(cgi_input[0],0);
		//1->output[1]
		dup2(cgi_output[1],1);
		sprintf(meth_env,"REQUEST_METHOD=%s",method);
		putenv(meth_env);

		if(strcasecmp(method,"GET")==0)//GET
		{
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);
		}else{
			sprintf(content_len_env,"CONTENT_LENGTH=%d",content_length);
			putenv(content_len_env);
		}
		execl(path,path,NULL);
	}else{//father
		close(cgi_input[0]);
		close(cgi_output[1]);
		char ch;
		int i=0;
		if(strcasecmp(method,"POST")==0){
			for(;i<content_length;++i)
			{
				recv(client,&ch,1,0);
				write(cgi_input[1],&ch,1);
			}
		}
//		
		while(read(cgi_output[0],&ch,1)>0){
			send(client,&ch,1,0);	
		}
		close(cgi_input[1]);
		close(cgi_output[0]);
		waitpid(id,NULL,0);//wait child
	}
}
void* accept_request(void *arg)
{
	printf("debug : get a conntct...\n ");
	pthread_detach(pthread_self());
	int client=(int)arg;
	int cgi=0;
	char* query_string=NULL;

	char method[_BLOCK_SIZE_/10];
	char url[_BLOCK_SIZE_*2];
	char buf[_BLOCK_SIZE_];
	char path[_BLOCK_SIZE_];
   
	memset(method,'\0',sizeof(method));
	memset(url,'\0',sizeof(url));
	memset(path,'\0',sizeof(path));
	memset(buf,'\0',sizeof(buf));
#ifdef _DEBUG
   printf("in DEBUG mode\n");
while(get_line(client,buf,sizeof(buf))>0){
	printf("%s");
}
#endif
if(get_line(client,buf,sizeof(buf)-1)<0){
//	return_errno_client();
	close(client);
	return (void*)-1;
}
//GET / HTTP/1.1
//获取方法
int i=0;
int j=0;
while(!isspace(buf[j])&&i<sizeof(method)-1&&j<sizeof(buf)){
	method[i]=buf[j];
	++i;
	++j;
}

//GET && POST
//检查方法
if(strcasecmp(method,"GET")&&(strcasecmp(method,"POST"))){
//	return_errno_client();
	return (void*)-1;
}

if(strcasecmp(method,"POST")==0){
	cgi=1;
}	
//skip space
while(isspace(buf[j])&&j<sizeof(buf)){
	++j;
}
//获取路径，先要越过空格
i=0;
while(!isspace(buf[j])&&i<sizeof(url)-1&&j<sizeof(buf)){
	url[i]=buf[j];
	++i;
	++j;
}
//分析url，路径和参数分开
if(strcasecmp(method,"GET")==0){
	query_string=url;
	while(*query_string!='?'&&*query_string!='\0'){
		query_string++;
	}
	if(*query_string=='?'){
		*query_string='\0';
		query_string++;
		cgi=1;
	}
}
	sprintf(path,"htdocs%s",url);//path/
	if(path[strlen(path)-1]=='/'){
		strcat(path,MAIN_PAGE);
	}
	struct stat st;
	if(stat(path,&st)<0){//not found
//	return_errno_client();
		return (void *)-1;
	}else{
		if(S_ISDIR(st.st_mode)){
			strcat(path,"/");
			strcat(path,MAIN_PAGE);
		}else if((st.st_mode & S_IXUSR)||(st.st_mode & S_IXGRP)||(st.st_mode & S_IXOTH)){
			cgi=1;
		}else{
			NULL;		
		}

		if(cgi){
			exe_cgi(client,path,method,query_string);
		}else{
			echo_html(client,path,st.st_size);
		}
	}
    close(client);
	return NULL;
}
int main(int argc,char *argv[])
{
	if(argc!=2){
		usage(argv[0]);
		exit(1);
	}
	struct sockaddr_in client;
	socklen_t len=sizeof(client);

	int port=atoi(argv[1]);
	int listen_sock=startup(port);
	while(1){
		int new_client=accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_client<0){
			print_log(__FUNCTION__, __LINE__, errno, strerror(errno));
			continue;
		}
		printf("debug : get a conntct...\n ");
		pthread_t id;
		pthread_create(&id,NULL,accept_request,(void*)new_client);
	}
	close(listen_sock);
	return 0;
}




