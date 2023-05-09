#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "actuators.h"
#include "sensors.h"

int fd_pumpN,fd_pumpS,fd_pumpE,fd_pumpW,fd_pumpUp,fd_pumpDown,error;

int init_actuators()
{
    
    error = connect_socket(20000,&fd_pumpN);
    if(error == -1)return -1;
    
    error = connect_socket(20001,&fd_pumpS);
    if(error == -1)return -1;

    error = connect_socket(20002,&fd_pumpE);
    if(error == -1)return -1;

    error = connect_socket(20003,&fd_pumpW);
    if(error == -1)return -1;

    error = connect_socket(20004,&fd_pumpUp);
    if(error == -1)return -1;

    error = connect_socket(20005,&fd_pumpDown);
    if(error == -1)return -1;

    return 0;
}

int send_msg_actuator(int fd,char * msg)
{
    char c=0;
    int length = strlen(msg)+1;
    send(fd,(char *)&length,sizeof(int),0);
    send(fd,msg,length,0);
    recv(fd,(char *)&c,sizeof(char),0);

    return(1);

}

