#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>

/* --------------------------------------------------- */
#define HTTP_DEFAULT_PORT_NUM "80"
/* --------------------------------------------------- */
 
/* --------------------------------------------------- */
typedef struct _request_info {
	char* host_name;
	char* host_ip;
	int port_num;
	char* address;
	char* file_name;
} request_info;

typedef struct _proxy_url{
	char* protocol;
	char* domain;
	char* port_num;
	char* path;
} proxy_url;
/* --------------------------------------------------- */
void proxy_proces_url(const char* url_value);
/* --------------------------------------------------- */


