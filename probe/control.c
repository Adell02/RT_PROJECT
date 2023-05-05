#include "os.h"
#include <stddef.h>
#include <math.h>

#include "control.h"
#include "sensors.h"
#include "actuators.h"

#define MAX_SPEED 2

extern int fd_deltaX,fd_deltaY,fd_deltaZ;
extern int fd_pumpN,fd_pumpS,fd_pumpE,fd_pumpW,fd_pumpUp,fd_pumpDown;
extern PREADYQUEUE tasks_queue;
extern PSCHEDULER single_scheduler;

PTASK read_deltaX,read_deltaY,read_deltaZ,act_N,act_S,act_E,act_W,act_U,act_D;


float lect_X=0,lect_Y=0,lect_Z=0,dist = 0,last_dist =0,total_vel=0;

float vel[3] = {0,0,0};  // X,Y,Z

//FLAGS
int initial_read = 0;

int init_tasks()
{    
    PTASK control;

    Task_create(&control,"Control Task",task_control,0,1000,1);

    //Task_create(&read_deltaX,"Read sensor X",task_read_socket,(void *)('X'),0,300);
    //Task_create(&read_deltaY,"Read sensor Y",task_read_socket,(void *)('Y'),0,300);
    //Task_create(&read_deltaZ,"Read sensor Z",task_read_socket,(void *)('Z'),0,300);
    
    //Task_create(&act_N,"Pump North",task_on_actuator,(void *)('N'),0,100);
    //Task_create(&act_S,"Pump South",task_on_actuator,(void *)('S'),0,100);
    //Task_create(&act_E,"Pump East",task_on_actuator,(void *)('E'),0,100);
    //Task_create(&act_W,"Pump West",task_on_actuator,(void *)('W'),0,100);

    Readyqueue_enqueue(tasks_queue,control);
    

    return 0;
}

void task_read_socket(char val)
{

    if(val == 'X'){
        read_socket(fd_deltaX,&lect_X);
    }
    else if ( val == 'Y'){
        read_socket(fd_deltaY,&lect_Y);
    }
    else{
        read_socket(fd_deltaZ,&lect_Z);
    }

}

void task_on_actuator(char val)
{
    int fd;

    if(val == 'N'){
        fd = fd_pumpN;
        vel[1] = vel[1]-0.01;
    }
    else if ( val == 'S')
    {
        fd = fd_pumpS;
        vel[1] = vel[1]+0.01;
    }
    else if ( val == 'E')
    {
        fd = fd_pumpE;
        vel[0] = vel[0]-0.01;
    } 
    else if ( val == 'W')
    { 
        fd = fd_pumpW;
        vel[0] = vel[0]+0.01;
    }
    else if ( val == 'U'){
        fd = fd_pumpUp;
        vel[2] = vel[2]-0.01;
    }
    else if ( val == 'D'){ 
        fd = fd_pumpDown;
        vel[2] = vel[2]+0.01;
    }

    send_msg_actuator(fd,"start");  // 50ms accelerating --> +0.01m/s
    send_msg_actuator(fd,"stop");


}

void task_control(void * a)
{
    task_read_socket('X');
    task_read_socket('Y');
    task_read_socket('Z');

    total_vel = sqrt(pow(vel[0],2)+pow(vel[1],2)+pow(vel[2],2));
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


}