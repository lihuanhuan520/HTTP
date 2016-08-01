#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<string>
#include"mysql_connect.h"
using namespace std;

void my_register(char* _data)
{
	string arr[4];
	int i=3;
	int index=strlen(_data)-1;
	while(index>=0){
		if(_data[index]=='&'){
			_data[index]='\0';
		}
		if(_data[index]=='='){
			arr[i--]=&_data[index+1];
		}
		index--;
	}
	string _info="";
	for(int j=0;j<4;++j){
		_info+="'";
		_info+=arr[j];
		_info+="'";
		if(j!=3){
			_info+=",";
		}
	}
	cout<<_info<<endl;

	sql_connect _conn;
	_conn.connect_mysql();
	_conn.insert_info(_info);
	_conn.select_info();
	return;
}

int main()
{
	string method;
	string query_string;
	string content_length;
	char buf[1024];
	memset(buf,'\0',sizeof(buf));

	if(getenv("REQUEST_METHOD")==NULL){
		exit(1);
	}
	method=getenv("REQUEST_METHOD");
	if(strcasecmp("GET",method.c_str())==0){
		if(getenv("QUERY_STRING")==NULL){
			exit(1);
		}
		query_string=getenv("QUERY_STRING");
		char buf[1024];
		memset(buf,'\0',sizeof(buf));
		strcpy(buf,query_string.c_str());
		my_register(buf);
	}else if(strcasecmp("POST",method.c_str())==0){
		if(getenv("CONTENT_LENGTH")==NULL){
			exit(1);
		}
		content_length=getenv("CONTENT_LENGTH");
		int len=atoi(content_length.c_str());
		int i=0;
		for(;i<len;++i){
			read(0,&buf[i],1);
		}
		cout<<method<<endl;
		my_register(buf);
	}else{
		exit(1);
	}
	return 0;
}
