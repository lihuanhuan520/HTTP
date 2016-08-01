#pragma once
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<ctype.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/sendfile.h>
#include<fcntl.h>

#define _BANK_LOG_ 5
#define _BLOCK_SIZE_ 2048

#define MAIN_PAGE "index.html"
