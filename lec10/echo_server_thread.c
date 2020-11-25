#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 1024
#define PORTNUM 3500
pthread_mutex_t m_lock;

char res[MAXLINE][MAXLINE];

void * thread_func(void *data)
{
	int sockfd = *((int *)data);
	int readn;
	char res_num[MAXLINE];
	char res_string[MAXLINE];
	char buf[MAXLINE];

	memset(res_num, 0x00, MAXLINE);
	memset(res_string, 0x00, MAXLINE);

	int number=0;
	char tmp ;
	socklen_t addrlen;
	struct sockaddr_in client_addr;
	addrlen = sizeof(client_addr);
	getpeername(sockfd, (struct sockaddr *)&client_addr, &addrlen);
	read(sockfd, res_string, MAXLINE);
	read(sockfd, res_num, MAXLINE);
	
	printf("Read Data %s(%d) : %s and %s \n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port, res_string, res_num);
	
	while(1)
	{
		pthread_mutex_lock(&m_lock);

		memset(buf, 0x00, MAXLINE);
		number = atoi(res_num);
		number += 1;
		sprintf(res_num, "%d", number);

		for(int i=0; i<strlen(res_string); i++){
			if(i==0) tmp = res_string[0];
			if(res_string[i+1] != 0){
				res_string[i] = res_string[i+1];
			}
			else{
				res_string[i]= tmp;
			}
		}

		strcat(buf, res_num);
		strcat(buf,",");
		strcat(buf,res_string);
		sprintf(res[sockfd],"%s",buf);
		sleep(0.5);

		pthread_mutex_unlock(&m_lock);
		sleep(1);
	}
	close(sockfd);
	return 0;
}

void * con_func(void *data)
{
	int sockfd = *((int *)data);
	int readn;
	socklen_t addrlen;
	char rebuf[MAXLINE];
	
	struct sockaddr_in client_addr;

	addrlen = sizeof(client_addr);
	getpeername(sockfd, (struct sockaddr *)&client_addr, &addrlen);
	while(1)
	{
		
		pthread_mutex_lock(&m_lock);
		memset(rebuf, 0x00, MAXLINE);
		sprintf(rebuf, "%s", res[sockfd]);
		write(sockfd, rebuf, strlen(rebuf));
		sleep(0.5);
		pthread_mutex_unlock(&m_lock);
		sleep(1);
	}
	close(sockfd);
	return 0;
}

int main(int argc, char **argv)
{
	int listen_fd, client_fd;
	socklen_t addrlen;
	int readn;
	char buf[MAXLINE];
	pthread_t thread_id;

	struct sockaddr_in server_addr, client_addr;

	if(pthread_mutex_init(&m_lock, NULL) != 0) 
	{
		perror("Mutex Init failure");
		return 1;
	}

	if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return 1;
	}
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);

	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==-1	)
	{
		perror("bind error");
		return 1;
	}
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}

	while(1)
	{
		addrlen = sizeof(client_addr);
		client_fd = accept(listen_fd,(struct sockaddr *)&client_addr, &addrlen);
		//printf("new client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		if(client_fd == -1)
		{
			printf("accept error\n");
		}
		else
		{
			pthread_create(&thread_id, NULL, thread_func, (void *)&client_fd);
			pthread_create(&thread_id, NULL, con_func, (void *)&client_fd);
			pthread_detach(thread_id);
			printf("%ld", thread_id);
			//pthread_detach(thread_id[1]);
		}
	}
	return 0;
}

