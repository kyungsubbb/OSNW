#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXBUF 1024
int main(int argc, char **argv)
{
    int server_sockfd, client_sockfd0,client_sockfd1,client_sockfd2;
    int client_len, count = 0;
    char buf0[MAXBUF];
    char buf1[MAXBUF];
    char buf2[MAXBUF];
    struct sockaddr_in clientaddr, serveraddr;
    client_len = sizeof(clientaddr);
    if ((server_sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1)
    {
        perror("socket error : ");
        exit(0);
    }
    memset(&serveraddr, 0x00, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));
    bind (server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(server_sockfd, 5);
    while(1)
    {
        client_sockfd0 = accept(server_sockfd, (struct sockaddr *)&clientaddr,&client_len);
        printf("New Client Connect[0]: %s\n", inet_ntoa(clientaddr.sin_addr));
        client_sockfd1 = accept(server_sockfd, (struct sockaddr *)&clientaddr,&client_len);
        printf("New Client Connect[1]: %s\n", inet_ntoa(clientaddr.sin_addr));
        client_sockfd2 = accept(server_sockfd, (struct sockaddr *)&clientaddr,&client_len);
        printf("New Client Connect[2]: %s\n", inet_ntoa(clientaddr.sin_addr));
        memset(buf0, 0x00, MAXBUF);
        memset(buf1, 0x00, MAXBUF);
        memset(buf2, 0x00, MAXBUF);
        if ((read(client_sockfd0, buf0, MAXBUF)) <= 0)
        {
            close(client_sockfd0);
            continue;
        }
        if ((read(client_sockfd1, buf1, MAXBUF)) <= 0)
        {
            close(client_sockfd1);
            continue;
        }
        if ((read(client_sockfd2, buf2, MAXBUF)) <= 0)
        {
            close(client_sockfd2);
            continue;
        }
        strcat(buf0,buf1);
        strcat(buf0,buf2);
        for(int i = 0; buf0[i]!=0; i++){
            if(buf0[i] == '\n'){
                if(count>=2){
                    break;
                }
                else{
                    buf0[i] = ' ';
                    count ++;
                }
            }
            
        }
        if (write(client_sockfd0, buf0, MAXBUF) <=0)
        {
            perror("write error : ");
            close(client_sockfd0);
        }
        
        if (write(client_sockfd1, buf0, MAXBUF) <=0)
        {
            perror("write error : ");
            close(client_sockfd1);
        }
        
        if (write(client_sockfd2, buf0, MAXBUF) <=0)
        {
            perror("write error : ");
            close(client_sockfd2);
        }
        else{
            break;
        }
    
        close(server_sockfd);
    }
    close(server_sockfd);
    return 0;
}