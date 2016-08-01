ROOT_PATH=$(shell pwd)/..
INCLUDE=-I$(ROOT_PATH)/sql_connect -I$(ROOT_PATH)/sql_connect/mysql_lib/mysql/include
LIB_PATH=-L$(ROOT_PATH)/sql_connect/mysql_lib/mysql/lib
LDFLAGS=-lmysqlclient
REGISTER_SRC=cgi_register.cpp $(ROOT_PATH)/sql_connect/mysql_connect.cpp

.PHONY:all
all:cgi_math cgi_register

cgi_math:cgi_math.cpp
	g++ -o $@ $^ -gdwarf-2
cgi_register:cgi_register.cpp ../sql_connect/mysql_connect.cpp 
	g++ -o $@ $^ ${INCLUDE} ${LDFLAGS} ${LIB_PATH}
.PHONY:clean
clean:
	rm -f cgi_math cgi_register

.PHONY:output
output:
	cp cgi_math ../output/htdocs/cgi-bin
	cp cgi_register ../output/htdocs/cgi-bin
