#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <sys/stat.h>
#include <stdio.h>      
#include <string.h>    
#include <unistd.h>     
#include <sys/time.h>
#include <stdlib.h>

#define MAXLINE    1024

int main(int argc, char **argv)
{
    struct timeval timeout;
    fd_set reads, alls;
    struct sockaddr_in serveraddr;
    int server_sockfd;
    int client_len, n, m;
    char buf[MAXLINE];
    char rbuf[MAXLINE];
    char num[MAXLINE];
    int cnt = 0;
    int fd_num, maxfd=0;

    memset(num, 0x00, MAXLINE);
    memset(rbuf, 0x00, MAXLINE);
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("error :");
        return 1;
    }

 
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(3600);

    client_len = sizeof(serveraddr);


    if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len)  == -1)
    {
        perror("connect error :");
        return 1;
    }


    FD_ZERO(&reads);
    FD_SET(cnt, &reads);
    FD_SET(server_sockfd, &reads);
    if(cnt < server_sockfd)
    {
            maxfd = server_sockfd;
        }
    else
    {
            maxfd = cnt;
        }
    
    while(1) 
   {
      alls = reads;
      fd_num = select(maxfd + 1 , &alls, (fd_set *)0, (fd_set *)0, NULL);
      memset(rbuf, 0x00, MAXLINE);
      memset(buf, 0x00, MAXLINE);
      if (FD_ISSET(cnt, &alls))
      {
         //memset(buf, 0x00, MAXLINE);
         read(0, buf, MAXLINE);
         if (write(server_sockfd, buf, MAXLINE) <= 0)
            {
               perror("write error : ");
               return 1;
            }
         if(strncmp(buf, "quit", 4) == 0){
            
            break;
         } 

      }
      else if(FD_ISSET(server_sockfd, &alls))
      {
         if (read(server_sockfd, rbuf, MAXLINE) <= 0)
         {
            perror("read error : ");
            return 1;
         }
         printf("read : %s\n", rbuf);
      }
   }
   close(server_sockfd);
   return 0;
}
