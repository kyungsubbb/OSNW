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

    int readfd;
    int f1[2], f2[2];
    int w = 0;
    int buf_len;
    

	char buf[MAXLINE];
    char mid[MAXLINE];
    char ret[MAXLINE];


	struct sockaddr_in client_addr, server_addr;


    memset(mid, 0x00, MAXLINE);
    memset(ret, 0x00, MAXLINE);

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
	if(pipe(fd1) == -1)
	{
		perror("pipe1 error");
		return 1;
	}
    if(pipe(fd2) == -1)
	{
		perror("pipe2 error");
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
			memset(buf, 0x00, MAXLINE);
			while((readn = read(client_fd, buf, MAXLINE)) > 0)
			{
				printf("Read Data %s(%d) : %s",
						inet_ntoa(client_addr.sin_addr),
						client_addr.sin_port,
						 buf);
                buf_len = strlen(buf);
                buf[buf_len-1] = ' ';
                strcat(mid, buf);
                printf("mid: %s\n", mid);
				write(fd2[1], mid, strlen(mid));
				
                if((readfd = read(fd1[0], ret, MAXLINE))>0){
                    printf("readfd : %d\n", readfd);
                    write(client_fd, ret, strlen(ret));
                    memset(buf, 0x00, MAXLINE);
                }

			}
			close(client_fd);
			return 0;
		}
		else if( pid > 0){
            close(client_fd);
            read(fd2[0],mid, MAXLINE);
            w++;
            printf("w : %d\n", w);
            if(w==3){
                for(int i = 0; i<3; i++){
                    write(fd1[1],mid,strlen(mid));
                    sleep(1);
                }
            }
        }
			
	}
	return 0;
}
