#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#define BYTES 1024
int main(int argc, char *argv[]){
	
	struct sockaddr_in server_address, client_addr;
	socklen_t sin_len= sizeof(client_addr);
	int  Socket,new_socket;
	int on = 1;
	char msg[10000], *request[3], data_to_send[BYTES], path[10000];
        int rec, fd, bytes_read;

	Socket = socket(AF_INET,SOCK_STREAM,0);

	if(Socket<0){
		perror("socket");
		exit(1);
	}

	setsockopt(Socket,SOL_SOCKET, SO_REUSEADDR,&on,sizeof(int));

	server_address.sin_family=AF_INET;
	server_address.sin_addr.s_addr=INADDR_ANY;
	server_address.sin_port=htons(8080);

	if(bind(Socket, (struct sockaddr *) &server_address,sizeof(server_address))==-1){
		perror("bind");
		close(Socket);
		exit(1);
	}

	if(listen(Socket,10)==-1){
		perror("listen");
		close(Socket);
		exit(1);
	}

	while(1){

	new_socket=accept(Socket,(struct sockaddr *) &client_addr,&sin_len);

	if(new_socket==-1){
		perror("connection faild");
		continue;
	}else{
			
		printf("got client connection \n");

	if(!fork()){
		rec=recv(new_socket, msg, 10000, 0);
			if(rec>0){
				printf("%s", msg);
				request[0] = strtok (msg, " \t\n");
			if ( strncmp(request[0], "GET\0", 4)==0 ){
				request[1] = strtok (NULL, " \t");
		
			if ( strncmp(request[1], "/\0", 2)==0 ){
				request[1] = "/server.html";     
							}
			strcpy(path, getenv("PWD"));
			strcpy(&path[strlen(getenv("PWD"))], request[1]);
			printf("file: %s\n", path);
			// File found
			if ( (fd=open(path, O_RDONLY))!=-1 ){    
				send(new_socket, "HTTP/1.1 200 OK\n\n", 17, 0);
				while ( (bytes_read=read(fd, data_to_send, 1024))>0 ){
					write (new_socket, data_to_send, bytes_read);
								}
			}else{
					
				write(new_socket, "HTTP/1.1 404 Not Found\n", 23);
				}   
			}
		}
	}
	close(new_socket);	
		} 
	}
	return 0;
}



