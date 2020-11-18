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

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAXLINE 1024
#define PORTNUM 3500
union semun
{
	int val;
};
int main(int argc, char **argv)
{

	int listen_fd, client_fd;
	pid_t pid, pid1;
	socklen_t addrlen;
	int readn;
	char string[MAXLINE];
	char num[MAXLINE];
	int num1;
	char mid[MAXLINE];
	char buf[MAXLINE];
	char input[MAXLINE];
	char ret[MAXLINE];
	char number[MAXLINE];
	char abc[MAXLINE];
	struct sockaddr_in client_addr, server_addr;

	char *arr[10] = { NULL, };
	
	//semaphore

	int shmid;
	int semid;



	//int *cal_num;
	//char *cal_char[MAXLINE];
	void *shared_memory = {NULL,};
	union semun sem_union;
	int a = 1234;
	int b = 3477;
	struct sembuf semopen = {0, -1, SEM_UNDO};
	struct sembuf semclose = {0, 1, SEM_UNDO};
	memset(number, 0x00, MAXLINE);
	memset(ret, 0x00, MAXLINE);
	memset(mid, 0x00, MAXLINE);
	memset(string, 0x00, MAXLINE);
	memset(num, 0x00, MAXLINE);
	memset(buf, 0x00, MAXLINE);
	memset(abc, 0x00, MAXLINE);
	memset(input, 0x00, MAXLINE);


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
		
			pid1 = fork();
			if(pid1 == 0){ //C

				shmid = shmget((key_t)a, MAXLINE, 0666);
				if (shmid == -1)
				{
					perror("shmget failed : ");
					exit(0);
				}

				semid = semget((key_t)b, 0, 0666);
				if(semid == -1)
				{
					perror("semget failed : ");
					return 1;
				}

				shared_memory = shmat(shmid, NULL, 0);
				if (shared_memory == (void *)-1)
				{
					perror("shmat failed : ");
					exit(0);
				}
				

				while(1)
				{
					if(semop(semid, &semopen, 1) == -1)
					{
						perror("semop error : ");
					}

					sprintf( abc, "%s", (char *)shared_memory);
					sleep(1);
					write(client_fd, abc, strlen(abc));
					sleep(1);
					
					semop(semid, &semclose, 1);
				}
				return 1;
			}
			else if(pid1 > 0){ //P

				shmid = shmget((key_t)a, MAXLINE, 0666|IPC_CREAT);
				if (shmid == -1)
				{
					return 1;
				}

				semid = semget((key_t)b, 1, IPC_CREAT|0666);
				if(semid == -1)
				{
					return 1;
				}

				shared_memory = shmat(shmid, NULL, 0);
				if (shared_memory == (void *)-1)
				{
					return 1;
				}
				
				sem_union.val = 1;
				if ( -1 == semctl( semid, 0, SETVAL, sem_union))
				{
					return 1;
				}

				read(client_fd, string, MAXLINE);
				read(client_fd, num, MAXLINE);
				strcat(input, string);
				strcat(input, " and ");
				strcat(input, num);
				printf("Read Data %s(%d) : %s \n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port, input);

				sprintf(((char *)shared_memory), "%s", input);
				
				while(1)
				{
					int j = 0;
					if(semop(semid, &semopen, 1) == -1)
					{
						return 1;
					}

					sprintf(buf, "%s", (char *)shared_memory);

					char *ptr = strtok(buf, " ");
					while(ptr != NULL){
						arr[j] = ptr;
						j++;

						ptr = strtok(NULL, " ");
					}
					
					num1 = atoi(arr[2]);
					sprintf(ret, "%s", arr[0]);

					char tmp;
					
					for(int i=0; i<strlen(ret); i++){
						if(i==0) tmp = ret[0];
						if(ret[i+1] != 0){
							ret[i] = ret[i+1];
						}
						else{
							ret[i]= tmp;
						}
					}
					sprintf(number, "%d", ++num1);
					memset(buf, 0x00, MAXLINE);
					strcat(buf, ret);
					strcat(buf, " and ");
					strcat(buf, number);
					sprintf(((char *)shared_memory), "%s", buf);
					sleep(1);
					semop(semid, &semclose, 1);
				}
				return 1;
			}

			close(client_fd);
			return 0;
		}
		else if( pid > 0){
			a++;
			b++;
			close(client_fd);
		}
			
	}
	return 0;
}
