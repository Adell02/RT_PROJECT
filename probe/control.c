#include "os.h"
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "control.h"
#include "sensors.h"
#include "actuators.h"

#define MAX_SPEED 2

extern int fd_deltaX,fd_deltaY,fd_deltaZ;
extern int fd_pumpN,fd_pumpS,fd_pumpE,fd_pumpW,fd_pumpUp,fd_pumpDown;
extern PREADYQUEUE tasks_queue;
extern PSCHEDULER single_scheduler;

float pos [3] = {0,0,0}, vel [3] = {0,0,0}, acc [3] = {0,0,0} ,dist, vel;


int init_tasks()
{    
    PTASK control;

    Task_create(&control,"Control Task",task_control,0,3000,1);



    Readyqueue_enqueue(tasks_queue,control);
    

    return 0;
}

void task_read_socket(char val,float *pos)
{

    if(val == 'X'){
        read_socket(fd_deltaX,pos);
    }
    else if ( val == 'Y'){
        read_socket(fd_deltaY,pos);
    }
    else{
        read_socket(fd_deltaZ,pos);
    }

}

void task_on_actuator(char val)
{
    int fd = 1;

    if(val == 'N'){
        fd = fd_pumpN;
    }
    else if ( val == 'S')
    {
        fd = fd_pumpS;
    }
    else if ( val == 'E')
    {
        fd = fd_pumpE;
    } 
    else if ( val == 'W')
    { 
        fd = fd_pumpW;
    }
    else if ( val == 'U'){
        fd = fd_pumpUp;
    }
    else if ( val == 'D'){ 
        fd = fd_pumpDown;
    }

    send_msg_actuator(fd,"start");  // 50ms accelerating --> +0.01m/s
    send_msg_actuator(fd,"stop");


}

void task_control(void * a)
{
    float posX [3],posY [3], posZ [3];
    task_read_socket('X',&posX[0]);
    task_read_socket('X',&posX[1]);
    task_read_socket('X',&posX[2]);
    
    
    pos[0] = posX[2];
    vel[0] = (posX[2]-posX[1])/0.3;
    acc[0] = ((vel[0])-((posX[1]-posX[0])/0.3))/0.3;
    
    task_read_socket('Y',&posY[0]);
    task_read_socket('Y',&posY[1]);
    task_read_socket('Y',&posY[2]);

    pos[1] = posY[2];
    vel[1] = (posY[2]-posY[1])/0.3;
    acc[1] = ((vel[1])-((posY[1]-posY[0])/0.3))/0.3;
    
    task_read_socket('Z',&posZ[0]);
    task_read_socket('Z',&posZ[1]);
    task_read_socket('Z',&posZ[2]);

    pos[2] = posZ[2];
    vel[2] = (posZ[2]-posZ[1])/0.3;
    acc[2] = ((vel[2])-((posZ[1]-posZ[0])/0.3))/0.3;

    float dist = sqrt(pow(pos[0],2)+pow(pos[1],2)+pow(pos[2],2));

    

    //total_vel = sqrt(pow(vel[0],2)+pow(vel[1],2)+pow(vel[2],2));
    /*
    if (!initial_read)
    {
        last_dist = sqrt(pow(lect_X,2)+pow(lect_Y,2)+pow(lect_Z,2));
        initial_read = 1;
    }
    else
    {
        dist = sqrt(pow(lect_X,2)+pow(lect_Y,2)+pow(lect_Z,2));

        if(dist < last_dist)
        {

        }
    }*/

    /*
    if(lect_X < 0)
    {
        task_on_actuator('E');            
    }
    else if(lect_X > 0)
    {
        task_on_actuator('W');
    }
    if(lect_Y < 0)
    {
        task_on_actuator('S');
    }
    else if(lect_Y > 0)
    {
        task_on_actuator('N');
    }
    if(lect_Z < 0)
    {
        task_on_actuator('D');
    }
    else if (lect_Z > 0)
    {
        task_on_actuator('U');
    }
    */

}