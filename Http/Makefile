ROOT_PATH=$(shell pwd)
SERVER_BIN=httpd
CLIENT_BIN=demo_client
CGI_PATH=cgi-bin

CC=gcc
FLAGS=#-D_DEBUG
LDFLAGS=-lpthread

SERVER_SRC=httpd.c
CLIENT_SRC=demo_client.c

.PHONY:all 
all: $(SERVER_BIN) $(CLIENT_BIN) cgi 

$(SERVER_BIN):$(SERVER_SRC)
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS) -gdwarf-2
$(CLIENT_BIN):$(CLIENT_SRC)
	 $(CC) -o $@ $^ $(FLAGS) $(LDFLAGS) -gdwarf-2
.PHONY:cgi
cgi:
	@for name in `echo $(CGI_PATH)`;\
	do\
	   cd $$name;\
	   make;\
	   cd -;\
	done
.PHONY:clean
clean:
	rm -rf $(SERVER_BIN) $(CLIENT_BIN) output
	@for name in `echo $(CGI_PATH)`;\
	do\
	   cd $$name;\
	   make clean;\
	   cd -;\
	done
	
.PHONY:output
output:all
	mkdir output
#	mkdir -p output/htdocs/cgi-bin
	cp httpd output
	cp demo_client output
	cp start.sh output
	cp -rf htdocs output
	cp -rf cgi-bin output/htdocs
	cp -rf conf output
	cp -rf log output
	cp -rf sql_connect output
#	cp -rf htdocs/* output/htdocs
	@for name in 'echo $(CGI_PATH)';\
	do\
	   cd $$name;\
	   make output;\
	   cd -;\
	done

