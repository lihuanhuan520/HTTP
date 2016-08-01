#!/bin/bash
ROOT_PATH=$(pwd)
LIB_PATH=${ROOT_PATH}/sql_connect/mysql_lib/lib

service iptables stop
service mysqld start

export LD_LIBRARY_PATH=${LIB_PATH}; ./httpd 8080
