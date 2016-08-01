ROOT_PATH=$(shell pwd)
INCLUDE=-I$(ROOT_PATH)/mysql_lib/mysql/include
LIB_PATH=-L$(ROOT_PATH)/mysql_lib/mysql/lib
LDFLAGS=-lmysqlclient

mysql_connect:mysql_connect.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS) -D_DEBUG_
.PHONY:clean
clean:
	rm -rf mysql_connect
