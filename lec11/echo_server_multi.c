#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 1024
#define PORTNUM 3600
#define SOCK_SETSIZE 1021

struct packet{
    int num;
    char string[MAXLINE];
};
int main(int argc, char **argv)
{
	struct packet p1[MAXLINE];
	int listen_fd, client_fd[MAXLINE];
	socklen_t addrlen;
	int fd_num;
	int sockfd;
	int maxfd = 0;
	int i= 0;
	int a = 0;
	int result = 0;

	char buf[MAXLINE];
	char res[MAXLINE];
	char sol[MAXLINE];
	char num[MAXLINE];
	
	memset(res,0x00, MAXLINE);
	memset(sol,0x00, MAXLINE);
	memset(num,0x00, MAXLINE);
	fd_set readfds, allfds;

	struct sockaddr_in server_addr, client_addr;

	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error");
		return 1;
	}   
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);
	
	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error");
		return 1;
	}   
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}   
	
	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);

	maxfd = listen_fd;
	while(1)
	{
		allfds = readfds;
		fd_num = select(maxfd + 1 , &allfds, (fd_set *)0, (fd_set *)0, NULL);

		if (FD_ISSET(listen_fd, &allfds))
		{
			addrlen = sizeof(client_addr);
			client_fd[a] = accept(listen_fd,(struct sockaddr *)&client_addr, &addrlen);

			FD_SET(client_fd[a],&readfds);

			if (client_fd[a] > maxfd)
				maxfd = client_fd[a];
			a++;

			continue;
		}
		for (i = 0; i <= maxfd; i++)
		{
			sockfd = i;
			if (FD_ISSET(sockfd, &allfds))
			{
				memset(buf, 0x00, MAXLINE);
				if(read(sockfd, res, MAXLINE)>0){
					if(strncmp(res, "quit",4) == 0){
					for (int j = 0 ; j < a; j ++){
					close(client_fd[j]);
					FD_CLR(client_fd[j], &readfds);
					}	
					
						break;
					}
						
					char *tmp1 = strtok(res, ", ");
					char *tmp2 = strtok(NULL, ", ");
					strcpy(p1[i].string,tmp1);
					p1[i].num = atoi(tmp2);
					for (int cnt = 0 ; cnt <= maxfd; cnt ++){
						strcat(sol, p1[cnt].string);
						
						result += p1[cnt].num;
						
					}
					sprintf(num, "%d", result);
					strcat(sol, ", ");
					strcat(sol, num);
				}
				else{
					break;
				}
				printf("%ld\n", strlen(sol));
				for (int j = 0 ; j < a; j ++){
					write(client_fd[j], sol, strlen(sol));
				}
				memset(sol, 0x00, MAXLINE);
				memset(num, 0x00, MAXLINE);
				result = 0;
				
			}
		}
	}
}

