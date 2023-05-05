#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sensors.h"

int fd_deltaX,fd_deltaY,fd_deltaZ,error;
int init_sensors()
{    
    
    error = connect_socket(20015,&fd_deltaX);
    if(error == -1)return -1;
    
    error = connect_socket(20016,&fd_deltaY);
    if(error == -1)return -1;

    error = connect_socket(20017,&fd_deltaZ);
    if(error == -1)return -1;
    

    return 0;
}

int connect_socket(int port, int*fd)
{
    struct sockaddr_in serv_addr;
    int try;

    if((*fd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        printf("\n Socket Creation Error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0)
    {
        printf("\n Invalid Address / Address not supported \n");
        return -1;
    }

    try = 0;

    while((connect(*fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0 && (try<5)))
    {
        try ++;
        usleep(500000);
    }

    if(try == 5) return-1;
    
    return 0;

}

int read_socket(int fd, float *value)
{
    recv(fd, value, sizeof(float),0);
    return 0;
}