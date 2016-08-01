#pragma once
//#include"mysql.h"
#include<iostream>
#include<string>
using namespace std;
#include"mysql.h"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_USER "root"
#define DEFAULT_PWD  ""
#define DEFAULT_DB "6_students"

class sql_connect
{
public:
	sql_connect(const string &host=DEFAULT_HOST,\
			    const string &user=DEFAULT_USER,\
				const string &password=DEFAULT_PWD,\
				const string &db=DEFAULT_DB);
	~sql_connect();
	bool connect_mysql();
	bool insert_info(string &_info);

	bool select_info();
	void show_info();
	void close_mysql();
private:
	MYSQL_RES *_res;
	MYSQL* _mysql_base;
	string _host;
	string _user;
	string _password;
	string _db;
};

