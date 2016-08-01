#include"mysql_connect.h"
#include<malloc.h>
sql_connect::sql_connect(const string &host,\
		                 const string &user,\
						 const string &password,\
						 const string &db)
:_host(host),_user(user),_password(password),_db(db)
{
	_mysql_base=mysql_init(NULL);
	_res=NULL;
}
bool sql_connect::connect_mysql()
{
	if(!mysql_real_connect(_mysql_base,_host.c_str(),_user.c_str(),_password.c_str(),_db.c_str(),3306,NULL,0)){
		cout<<"connect error"<<endl;
		return false;
	}else{
		cout<<"connece success"<<endl;
		return true;
	}
}
bool sql_connect::insert_info(string &_info)
{
	string sql_cmd="INSERT INTO base_info (name,sex,school,hobby) values";
	sql_cmd+="(";
	sql_cmd+=_info;
	sql_cmd+=")";
	if(mysql_query(_mysql_base,sql_cmd.c_str())){
		cout<<"insert failed"<<endl;
		return false;
	}else{
		cout<<"insert success"<<endl;
		return true;
	}
}
bool sql_connect::select_info()
{
	string sql_cmd="select * from base_info";
	if(mysql_query(_mysql_base,sql_cmd.c_str())!=0){
		cout<<"select  failed"<<endl;
		return false;
	}
	_res=mysql_store_result(_mysql_base);
	if(_res){
		int lines=mysql_num_rows(_res);
		int cols=mysql_num_fields(_res);
		cout<<"lines=:"<<lines<<endl;
		cout<<"cols=:"<<cols<<endl;

		string cols_name[5];
		int i=0;
		MYSQL_FIELD *fd=NULL;
		for(;fd=mysql_fetch_field(_res);)//get cols name
		{
			cols_name[i++]=fd->name;
		}
		for(int i=0;i<5;++i){
			cout<<cols_name<<endl;
			if(i!=4){
				cout<<"     ";
			}
		}
		cout<<endl;

		i=0;
		for(;i<lines;++i){
			MYSQL_ROW _row=mysql_fetch_row(_res);
			for(int j=0;j<cols;++j){
				cout<<_row[j];
				if(j!=cols){
					cout<<"           ";
				}
			}
			cout<<endl;
		}
		free(_res);
		return true;
	}
	return false;
}
void sql_connect::show_info()
{
	cout<<mysql_get_client_info()<<endl;
}
void sql_connect::close_mysql()
{
	mysql_close(_mysql_base);
}
sql_connect::~sql_connect()
{
	close_mysql();
	cout<<"~construct"<<endl;
}
#ifdef _DEBUG_
int main()
{
	sql_connect _conn;
	_conn.connect_mysql();
	string info="'huanhaun','w','xian','working'";
	_conn.insert_info(info);
	cout<<"MYSQL base info: ";
	_conn.show_info();
	return 0;
}
#endif
