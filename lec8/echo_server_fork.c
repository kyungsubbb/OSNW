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
#include <fcntl.h>
#define MAXLINE 1024
#define PORTNUM 3600
int main(int argc, char **argv)
{
int listen_fd,client_fd;
pid_t pid;
socklen_t addrlen;
int readn;
    int readfd;
    int rfd, wfd;
    int cnt = 0;
char buf[MAXLINE];
    char buf1[MAXLINE];
    char buf2[MAXLINE];
struct sockaddr_in client_addr, server_addr;
    memset(buf1, 0x00, MAXLINE);
    memset(buf2, 0x00, MAXLINE);
if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
return 1;
}
memset((void *)&server_addr, 0x00, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
server_addr.sin_port = htons(PORTNUM);
if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))
 ==−1)
{
perror("bind error");
return 1;
}
if(listen(listen_fd, 5) == −1)
{
perror("listen error");
return 1;
}
mkfifo("/tmp/myfifo_r", S_IRUSR|S_IWUSR);
mkfifo("/tmp/myfifo_w", S_IRUSR|S_IWUSR);
if( (rfd = open("/tmp/myfifo_r", O_RDWR)) == −1)
{
perror("rfd error");
return 0;
}
if ( (wfd = open("/tmp/myfifo_w", O_RDWR)) == −1)
{
perror("wfd error");
return 0;
}
signal(SIGCHLD, SIG_IGN);
while(1)
{
addrlen = sizeof(client_addr);
client_fd = accept(listen_fd,
(struct sockaddr *)&client_addr, &addrlen);
if(client_fd == −1)
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
printf("Read Data %s(%d) : %s", inet_ntoa(client
_addr.sin_addr),client_addr.sin_port, buf);
buf[strlen(buf)−1] = ’ ’;
strcat(buf1, buf);
write(wfd, buf1, strlen(buf1));
if((readfd = read(rfd, buf2, MAXLINE)) >0){
write(client_fd, buf2, strlen(buf2));
memset(buf, 0x00, MAXLINE);
}
close(client_fd);
return 0;
}
}
else if( pid > 0){
close(client_fd);
read(wfd, buf1, MAXLINE);
cnt ++;
if(cnt == 3){
for(int i =0 ; i < 3 ; i++){
write(rfd, buf1, strlen(buf1));
                    sleep(1);
}
}
}
}
return 0;
}

