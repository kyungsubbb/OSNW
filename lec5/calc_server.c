#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define PORT 3600
struct cal_data
{
int s_num;
char name[80];
int left_num;
int right_num;
char op;
int result;
short int error;
};
void inttohex(int data){
int fix = 0;
char str[32], space_str[32];
sprintf(str, "%08x", data);
space_str[0] = str[0];
for (int i=1; i<8; i++){
if((i%2)==0){
space_str[i+fix]=' ';
fix ++;
}
space_str[i+fix]=str[i];
}
printf("%s ", space_str);
}
int main(int argc, char **argv)
{
struct sockaddr_in client_addr, sock_addr;
int listen_sockfd, client_sockfd;
int addr_len;
struct cal_data rdata;
int left_num, right_num, s_num, cal_result;
short int cal_error;
if( (listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
{
perror("Error ");
return 1;
}
memset((void *)&sock_addr, 0x00, sizeof(sock_addr));
sock_addr.sin_family = AF_INET;
sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
sock_addr.sin_port = htons(PORT);
if( bind(listen_sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
{
perror("Error ");
return 1;
}
if(listen(listen_sockfd, 10) == -1)
{
perror("Error ");
return 1;
}
for(;;)
{
addr_len = sizeof(client_addr);
client_sockfd = accept(listen_sockfd,
(struct sockaddr *)&client_addr, &addr_len);
if(client_sockfd == -1)
{
perror("Error ");
return 1;
}
printf("New Client Connect : %s\n", inet_ntoa(client_addr.sin_addr));
read(client_sockfd, (void *)&rdata, sizeof(rdata));
cal_result = 0;
cal_error = 0;
s_num = ntohl(rdata.s_num);
left_num = ntohl(rdata.left_num);
right_num = ntohl(rdata.right_num);
switch(rdata.op)
{
case '+':
cal_result = left_num + right_num;
break;
case '-':
cal_result = left_num - right_num;
break;
case '*':
cal_result = left_num * right_num;
break;
case '/':
if(right_num == 0)
{
cal_error = 2;
break;
}
cal_result = left_num / right_num;
break;
default:
cal_error = 1;
}
rdata.result = htonl(cal_result);
rdata.error = htons(cal_error);
printf("%d %s\n", ntohl(rdata.s_num), rdata.name);
inttohex(left_num);
inttohex(right_num);
inttohex(rdata.op);
inttohex(rdata.result);
printf("\n");
write(client_sockfd, (void *)&rdata, sizeof(rdata));
close(client_sockfd);
}
close(listen_sockfd);
return 0;
}
