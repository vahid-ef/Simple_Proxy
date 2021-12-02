#include "../inc/proxy_lib.h"

/* ---------- Error Messages ---------------------------------------------------------------------------------------------------- */
static const char* error_msg_syntax = "[Error]: Invalid URL syntax. Usage: <protocol>://<domain>[<:port_number>]/<path_to_file>";
static const char* error_msg_empty_filename = "[Error]: Invalid URL syntax, empty filename";
static const char* error_msg_empty_portnumber = "[Error]: Invalid URL syntax, empty <port_number>";
static const char* error_msg_invalid_portnumber = "[Error]: Invalid port number";
/* ------------------------------------------------------------------------------------------------------------------------------ */

/* ---------- Static Functions -------------------------------------------------------------------------------------------------- */

/* parse the information from url -------------------- 
	Description:
		Extracts the information from given URL string. In case of any error, all the fields for "p_url" are set to NULL. 
	Parameter(s):
		[0]: char pointer to the URL string
		[1]: pointer to proxy_url(aka, struct _proxy_url)
   	Return:
	   	[0]: returns '0' on successful parsing 
		[1]: returns '1' for syntax violation error
		[2]: returns '2' for unsuccessful memory allocation
		[3]: returns '3' for empty file_name error
		[4]: returns '4' for empty port_number 
		[5]: returns '5' for invalid port_number 	   	
 --------------------------------------------------------------- */
static int proxy_url_parse(const char* url, proxy_url* p_url){

	p_url->protocol = NULL;
	p_url->domain = NULL;
	p_url->port_num = NULL;
	p_url->path = NULL;
	
	/* locate the ':' to extract the protocol value */
	char* index = strchr(url, ':');
	if(index == NULL){
		printf("%s\n", error_msg_syntax);
		return 1;
	}

	/* allocate space */
	size_t size = (size_t)(index - url);
	if((p_url->protocol = (char*)malloc(sizeof(char) * (size + 1))) == NULL){
		printf("[Error]: unable to allocate space");
		return 2;
	}

	/* write data to destination */
	memcpy((void*)(p_url->protocol), (void*)(url), size);
	*(p_url->protocol + size) = '\0';

	/* check the syntax to match "<protocol>://" format */
	if((strncmp(index, "://", 3) != 0) || ((*(index + 3) == '/') || ((*(index + 3) == '\0')))){
		printf("%s\n", error_msg_syntax);
		//undo the protocol assignment
		free(p_url->protocol);
		p_url->protocol = NULL;
		return 1;
	}

	/* locate the <domain> ending */
	for(size=0; *(index+3+size) != ':' && *(index+3+size) != '/' && *(index+3+size) != '\0'; size++);
	if(size == 0){
		printf("%s\n", error_msg_syntax);
		//undo the protocol assignment
		free(p_url->protocol);
		p_url->protocol = NULL;
		return 1;
	}

	/* allocate space*/
	if((p_url->domain = (char*)malloc(sizeof(char) * (size + 1))) == NULL){
		printf("[Error]: unable to allocate space");
		return 2;
	}

	/* write data to destination */
	memcpy((void*)(p_url->domain), (void*)(index+3), size);
	*(p_url->domain + size) = '\0';

	/* update the index in URL */
	index = index+3+size;

	/* check if a port_number exists in the URL */
	if(*(index) == ':'){
		char* tmp = strchr(index, '/');
		if(tmp == NULL){
			printf("%s\n", error_msg_syntax);
			//undo the protocol assignment
			free(p_url->protocol);
			free(p_url->domain);
			//----------------------------
			p_url->protocol = NULL;
			p_url->domain = NULL;
			return 1;

		}
		if(*(tmp+1) == '\0'){
			printf("%s\n", error_msg_empty_filename);
			//undo the protocol assignment
			free(p_url->protocol);
			free(p_url->domain);
			//----------------------------
			p_url->protocol = NULL;
			p_url->domain = NULL;
			return 3;
		}

		size = (size_t)(tmp-index);
		if(size == 1){
			printf("%s\n", error_msg_empty_portnumber);
			//undo the protocol assignment
			free(p_url->protocol);
			free(p_url->domain);
			//----------------------------
			p_url->protocol = NULL;
			p_url->domain = NULL;
			return 4;
		}

		/* allocate space*/
		if((p_url->port_num = (char*)malloc(sizeof(char) * (size))) == NULL){
			printf("[Error]: unable to allocate space");
			return 2;
		}

		/* write data to destination */
		memcpy((void*)p_url->port_num, (void*)(index + 1), (size -1));
		*(p_url->port_num + size) = '\0';

		for(int i=0; i<strlen(p_url->port_num); i++){
			if(*(p_url->port_num+i) < '0' || *(p_url->port_num+i) > '9'){
			printf("%s\n", error_msg_invalid_portnumber);
			//undo the protocol assignment
			free(p_url->protocol);
			free(p_url->domain);
			free(p_url->port_num);
			//----------------------------
			p_url->protocol = NULL;
			p_url->domain = NULL;
			p_url->port_num = NULL;
			return 5;
		}
		}

		/* update the index in URL */
		index = tmp;
	}

	/* setup the file path*/
	if(*(index) == '/'){
		if(p_url->port_num == NULL){
			
			// to be implemented -------------------------
			//proxy_set_default_port_num(p_url->port_num);

			p_url->port_num = HTTP_DEFAULT_PORT_NUM;
			//--------------------------------------------
		}
		size = strlen(index);

		if(size == 1 || (*(index+size-1)=='/')){
			printf("%s\n", error_msg_empty_filename);
			//undo the protocol assignment
			free(p_url->protocol);
			free(p_url->domain);
			//free(p_url->port_num);
			//----------------------------
			p_url->protocol = NULL;
			p_url->domain = NULL;
			p_url->port_num = NULL;
			return 3;
		}
		
		/* allocate space*/
		if((p_url->path = (char*)malloc(sizeof(char) * (size + 1))) == NULL){
			printf("[Error]: unable to allocate space");
			return 2;
		}

		/* write data to destination */
		memcpy((void*)p_url->path, (void*)(index), size);
		*(p_url->path + size) = '\0';
	}

	if(*(index) == '\0'){
		printf("sda%s\n", error_msg_empty_filename);
			//undo the protocol assignment
			free(p_url->protocol);
			free(p_url->domain);
			free(p_url->port_num);
			//----------------------------
			p_url->protocol = NULL;
			p_url->domain = NULL;
			p_url->port_num = NULL;
			return 3;
	}
	return 0;
}

/* print the proxy_url --------------------
	Description:
		Prints the p_url(aka, struc _proxy_url) filed values to stdout 
	Parameter(s):
		[0]: pointer to proxy_url (aka, struct _proxy_url
   	Return:
	   	void
 --------------------------------------------------------------- */
static inline void proxy_url_info(const proxy_url* request){
	printf("<protocol>: %s\n", request->protocol != NULL ? request->protocol:"empty");
	printf("<domain>: %s\n", request->domain != NULL ? request->domain:"empty");
	printf("<port_numeber>: %s\n", request->port_num != NULL ? request->port_num:"empty");
	printf("<file_path>: %s\n", request->path != NULL ? request->path:"empty");
	return;
}

/* convert the domain to IP -------------------- 
	Description:
		Convert the proxy_url, "domain" field into corresponding IP address
	Parameter(s):
		[0]: pointer to proxy_url(aka, struct _proxy_url)
   	Return:
	   	[0]: returns pointer to IP string on successful parsing
		[1]: returns NULL on error 	
 --------------------------------------------------------------- */
static char* proxy_resolve_domain(const char* domain){
	char* ip_address = (char*)malloc(sizeof(char) * 100);
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
		
	if ((he = gethostbyname(domain)) == NULL) 
	{
		return NULL;
	}
	
	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i = 0; addr_list[i] != NULL; i++) 
	{
		//Return the first one;
		//strcpy(ip_address , inet_ntoa(*addr_list[i]) );

		memcpy((void*)ip_address, (void*)inet_ntoa(*addr_list[i]), strlen(inet_ntoa(*addr_list[i])));
		*(ip_address + strlen(ip_address)) = '\0';
	}
	
	return ip_address;
}

/* Creat local database directories -------------------- 
	Description:
		Checks that the local DB has an entry for URL request and creats in case there is no such directory.
		example: http://test.com/a/b/c/samle has "test/a/b/c/" entry in local database tha holds "sample" object.
	Parameter(s):
		[0]: pointer to domain string
		[1]: pointer to file_path string
   	Return:
	   	[0]: returns '0' when there is no such entry in local DB and able to creat it.
		[1]: returns '1' when there is no such entry in local DB and fails to creat it.
		[2]: returns '2' when entry exists. 	
 --------------------------------------------------------------- */
static int proxy_update_local_db(char* domain, char* file_path){

	char* index = strchr(domain, '.');
	char* parent_dir = NULL;

	if(index != NULL){
		size_t size = (size_t)(index - domain);
		parent_dir = (char*)malloc(sizeof(char) * (size +1));
		memcpy((void*)parent_dir, (void*)domain, size);
		*(parent_dir + size) = '\0';
	}
	else{
		size_t size = strlen(domain);
		parent_dir = (char*)malloc(sizeof(char) * (size + 1));
		memcpy((void*)parent_dir, (void*)domain, size);
		*(parent_dir + size) = '\0';
	}

	index = strrchr(file_path, '/');
	size_t size = index - file_path;
	size++;

	char* proxy_dir = (char*)malloc(sizeof(char) * (strlen(parent_dir) + size + 1));
	memcpy((void*)proxy_dir, (void*)parent_dir, strlen(parent_dir));
	memcpy((void*)(proxy_dir+strlen(parent_dir)), (void*)file_path, size);
	*(proxy_dir+strlen(parent_dir)+strlen(file_path)) = '\0';

	free(parent_dir);

    struct stat proxy_dir_found_stat;
    int proxy_dir_found = stat(proxy_dir, &proxy_dir_found_stat);
    
    if(proxy_dir_found != 0){
        const char* command = "mkdir -p ";
        char* command_to_system_call = (char*)malloc(sizeof(char)*(strlen(command)+strlen(proxy_dir)+1));
        
        memcpy(command_to_system_call, command, strlen(command));
        memcpy(command_to_system_call+strlen(command), proxy_dir, strlen(proxy_dir));
        *(command_to_system_call+strlen(command)+strlen(proxy_dir)) = '\0';

		free(proxy_dir);
        
        if(system(command_to_system_call) == -1){
            free(command_to_system_call);
            printf("%s\n", "[Warning] -> unable to create local DB path");
            return 2;
        }
        free(command_to_system_call);
		return 0;
    }
	free(proxy_dir);
    return 1;
} 

/* Send a simple HTTP GET request -------------------- 
	Description:
		Creats and sends a simple HTTTP GET request and writes back the received response in the corresponding file
		according to the path in local DB. It doesn't chekck the HTTP response status, it assumes 'OK'.
	Parameter(s):
		[0]: pointer to p_url
   	Return:
	   	[0]: returns '0' on success.
		[1]: returns '1' on domain error.
		[2]: returns '2' on socket error.
		[3]: returns '3' on connection error.	
		[4]: returns '4' on sending error.
 --------------------------------------------------------------- */
static int proxy_send_http_get_request(proxy_url* request){

	char* resolved_domain = proxy_resolve_domain(request->domain);
	if(resolved_domain == NULL){
		printf("Unable to resolve domain\n");
		return 1;
	}

	/* setup socket connection */
	int socket_desc;
	struct sockaddr_in server;
	char *message , *server_reply;

	server_reply = (char*)malloc(sizeof(char) * 1024 * 1024);
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
		return 2;
	}
	
	server.sin_addr.s_addr = inet_addr(resolved_domain);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(request->port_num));

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		herror("Connection error");
		return 3;
	}


		char* request_msg = (char*)malloc(sizeof(char) * (strlen("GET ") + strlen(" HTTP/1.0\r\nHost: ") + strlen("\r\n\r\n")+ strlen(request->domain)+strlen(strchr(request->path, '/'))+1));

		memcpy((void*)request_msg, (void*)"GET ", strlen("GET "));
		memcpy((void*)(request_msg+strlen("GET ")), (void*)strchr(request->path, '/'), strlen(strchr(request->path, '/')));
		memcpy((void*)(request_msg+strlen("GET ")+strlen(strchr(request->path, '/'))), (void*)" HTTP/1.0\r\nHost: ", strlen(" HTTP/1.0\r\nHost: "));
		memcpy((void*)(request_msg+strlen("GET ")+strlen(strchr(request->path, '/'))+strlen(" HTTP/1.0\r\nHost: ")), (void*)request->domain, strlen(request->domain));
		memcpy((void*)(request_msg+strlen("GET ")+strlen(strchr(request->path, '/'))+strlen(" HTTP/1.0\r\nHost: ")+strlen(request->domain)), (void*)"\r\n\r\n\0", strlen("\r\n\r\n\0"));

	if( send(socket_desc , request_msg , strlen(request_msg) , 0) < 0)
	{
		puts("Send failed");
		return 4;
	}
	puts("Data Send\n");
	
	//Receive a reply from the server

	size_t NB_received;
	if((NB_received = recv(socket_desc, server_reply , 1024*1024, 0)) < 0)
	{
		puts("recv failed");
	}
	puts("Reply received\n");

	if(NB_received > 1024*1024){
		char* tmp = server_reply;
		server_reply = (char*)realloc((void*)server_reply, NB_received);
		if(server_reply == NULL){
			printf("%s\n", "[Warning]: unable to fit the data, out of memory");
			server_reply = tmp;
		}
		if((NB_received = recv(socket_desc, server_reply , 1024*1024, 0)) < 0){
			printf("%s\n", "[Warning]: unable to retrieve data completely");
			server_reply = tmp;
		}
	}

	free(request_msg);

	char* msg_body = strstr(server_reply, "\r\n\r\n");

	if(proxy_update_local_db(request->domain, request->path) == 2){
		printf("%s\n", "[Error]: Unable to creat entry in local DB");
	}

	char* index = strchr(request->domain, '.');
	char* parent_dir = NULL;

	if(index != NULL){
		size_t size = (size_t)(index - request->domain);
		parent_dir = (char*)malloc(sizeof(char) * (size +1));
		memcpy((void*)parent_dir, (void*)request->domain, size);
		*(parent_dir + size) = '\0';
	}
	else{
		size_t size = strlen(request->domain);
		parent_dir = (char*)malloc(sizeof(char) * (size + 1));
		memcpy((void*)parent_dir, (void*)request->domain, size);
		*(parent_dir + size) = '\0';
	}

	char* file_address = (char*)malloc(sizeof(char) * (strlen(parent_dir) + strlen(request->path)+1));

	memcpy((void*)file_address, (void*)parent_dir, strlen(parent_dir));
	memcpy((void*)file_address+strlen(parent_dir), (void*)request->path, strlen(request->path));
	*(file_address+strlen(parent_dir)+strlen(request->path)) = '\0';

	free(parent_dir);

	FILE* buff = fopen(file_address, "w");
	fwrite(msg_body+4, 1, NB_received, buff);

	shutdown(socket_desc, SHUT_RDWR);


	return 0;
 }

void proxy_proces_url(const char* url){

	proxy_url p_url;

	if(proxy_url_parse(url, &p_url) != 0){
		return;
	}

	proxy_url_info(&p_url);

	proxy_send_http_get_request(&p_url);
	
	return;
}



