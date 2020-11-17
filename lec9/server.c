#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define MAXLINE 1024
#define PORTNUM 3600

int main(int argc, char **argv)
{
	int listen_fd, client_fd;
	pid_t pid;
	socklen_t addrlen;
	int readn;
	char string[MAXLINE];
	char num[MAXLINE];
	int num1;
	char mid[MAXLINE];
	char buf[MAXLINE];
	
	struct sockaddr_in client_addr, server_addr;
	
	memset(mid, 0x00, MAXLINE);
	if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return 1;
	}
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);

	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==-1)
	{
		perror("bind error");
		return 1;
	}
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}

	signal(SIGCHLD, SIG_IGN);
	while(1)
	{
		addrlen = sizeof(client_addr);
		client_fd = accept(listen_fd,
			(struct sockaddr *)&client_addr, &addrlen);
		if(client_fd == -1)
		{
			printf("accept error\n");
			break;
		}
		pid = fork();
		if(pid == 0)
		{
			close( listen_fd );
			memset(string, 0x00, MAXLINE);
			memset(num, 0x00, MAXLINE);
			memset(buf, 0x00, MAXLINE);
			read(client_fd, string, MAXLINE);
			read(client_fd, num, MAXLINE);
			printf("Read Data %s(%d) : %s and %s\n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port, string, num);
			num1 = atoi(num);
			for(int j=0; j<strlen(string); j++){
					buf[j] = string[j];
				}
			while(1){
				//int i = 0;
				char tmp;
				//memset(buf, 0x00, MAXLINE);
				

				for(int i=0; i<strlen(string); i++){
					if(i==0) tmp = buf[0];
					if(buf[i+1] != 0){
						buf[i] = buf[i+1];
					}
					else{
						buf[i]= tmp;
					}
				}

				
				
				num1 ++;
				sprintf(num, "%d", num1);

				strcat(mid, buf);
				strcat(mid, " and ");
				strcat(mid, num);

				write(client_fd, mid, strlen(mid));
				sleep(1);
				memset(mid, 0x00, MAXLINE);
			}
			
			close(client_fd);
			return 0;
		}
		else if( pid > 0)
			close(client_fd);
	}
	return 0;
}
