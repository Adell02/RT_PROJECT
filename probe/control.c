#include "os.h"
#include <stddef.h>
#include <math.h>
#include <stdio.h>

#include "control.h"
#include "sensors.h"
#include "actuators.h"

#define MAX_SPEED 1

extern int fd_deltaX,fd_deltaY,fd_deltaZ;
extern int fd_pumpN,fd_pumpS,fd_pumpE,fd_pumpW,fd_pumpUp,fd_pumpDown;
extern PREADYQUEUE tasks_queue;
extern PSCHEDULER single_scheduler;

PTASK read_deltaX,read_deltaY,read_deltaZ,act_N,act_S,act_E,act_W,act_U,act_D;


// medidas de la nave
float pos_nave [3], vel_nave[3], acc_nave[3]={0,0,0};

// Medidas sonda
float vel_sonda[3] = {0,0,0};  // X,Y,Z
float lect_X=0,lect_Y=0,lect_Z=0;


int init_tasks()
{    
    PTASK control,init_cond;

    Task_create(&init_cond,"Initial Conditions",ini_cond_func,0,0,3000);
    Task_create(&control,"Control Task",task_control,0,1500,100);

    Readyqueue_enqueue(tasks_queue,init_cond);
    Readyqueue_enqueue(tasks_queue,control);
    

    return 0;
}

void task_read_socket(char val, float *lect)
{

    if(val == 'X'){
        read_socket(fd_deltaX,lect);
    }
    else if ( val == 'Y'){
        read_socket(fd_deltaY,lect);
    }
    else{
        read_socket(fd_deltaZ,lect);
    }

}

void task_on_actuator(char val)
{
    int fd=1;

    if(val == 'N'){
        fd = fd_pumpN;
        vel_sonda[1] = vel_sonda[1]+0.01;
    }
    else if ( val == 'S')
    {
        fd = fd_pumpS;
        vel_sonda[1] = vel_sonda[1]-0.01;
    }
    else if ( val == 'E')
    {
        fd = fd_pumpE;
        vel_sonda[0] = vel_sonda[0]-0.01;
    } 
    else if ( val == 'W')
    { 
        fd = fd_pumpW;
        vel_sonda[0] = vel_sonda[0]+0.01;
    }
    else if ( val == 'U'){
        fd = fd_pumpUp;
        vel_sonda[2] = vel_sonda[2]+0.01;
    }
    else if ( val == 'D'){ 
        fd = fd_pumpDown;
        vel_sonda[2] = vel_sonda[2]-0.01;
    }

    send_msg_actuator(fd,"start");  // 50ms accelerating --> +0.01m/s
    send_msg_actuator(fd,"stop");
    
}

void task_control(void * a)
{

    task_read_socket('X',&lect_X);
    task_read_socket('Y',&lect_Y);
    task_read_socket('Z',&lect_Z);

    float min_dist;

    ///////// X AXIS  //////////

    min_dist = 4*(vel_sonda[0]-vel_nave[0])*(vel_sonda[0]-vel_nave[0])/0.2; // Proveniente de: V(t) = Vo + a * dist/(Vo*4)   donde el 4 es un factor de seguridad
    if (lect_X < 0 && -lect_X > min_dist)
    {
        task_on_actuator('E');
    }
    else if (lect_X < 0 && -lect_X < min_dist && vel_sonda[0] < vel_nave[0])
    {
        task_on_actuator('W');
    }


    if (lect_X > 0 && lect_X > min_dist)
    {
        task_on_actuator('W');
    }
    else if (lect_X > 0 && lect_X < min_dist && vel_sonda[0] > vel_nave[0])
    {
        task_on_actuator('E');
    }
 
    
    ///////// Y AXIS  //////////

    min_dist = 4*(vel_sonda[1]-vel_nave[1])*(vel_sonda[1]-vel_nave[1])/0.2;

    if (lect_Y < 0 && -lect_Y > min_dist)
    {
        task_on_actuator('S');
    }
    else if (lect_Y < 0 && -lect_Y < min_dist && vel_sonda[1] < vel_nave[1])
    {
        task_on_actuator('N');
    }
 

    if (lect_Y > 0 && lect_Y > min_dist)
    {
        task_on_actuator('S');
    }
    else if (lect_Y > 0 && lect_Y < min_dist && vel_sonda[1] > vel_nave[1])
    {
        task_on_actuator('N');
    }
 
    
    ///////// Z AXIS  //////////
    
    min_dist = 4.2*(vel_sonda[2]-vel_nave[2])*(vel_sonda[2]-vel_nave[2])/0.2;
    
    if (lect_Z < 0 && -lect_Z > min_dist)
    {
        task_on_actuator('D');
    }
    else if (lect_Z < 0 && -lect_Z < min_dist && vel_sonda[2] < vel_nave[2])
    {
        task_on_actuator('U');
    }

    if (lect_Z > 0 && lect_Z > min_dist )
    {
        task_on_actuator('U');
    }
    else if (lect_Z > 0 && lect_Z < min_dist  && vel_sonda[2] > vel_nave[2])
    {
        task_on_actuator('D');
    }


    printf("Vel X\t\tVel Y\t\tVel Z\n%f\t%f\t%f\n",vel_sonda[0],vel_sonda[1],vel_sonda[2]);


}

void ini_cond_func (void * a)
{
    
    task_read_socket('X',&pos_nave[0]);
    task_read_socket('Y',&pos_nave[1]);
    task_read_socket('Z',&pos_nave[2]);

    task_read_socket('X',&vel_nave[0]);
    task_read_socket('Y',&vel_nave[1]);
    task_read_socket('Z',&vel_nave[2]);
    
    printf("%f\n",vel_nave[0]);
    printf("%f\n",vel_nave[1]);
    printf("%f\n\n",vel_nave[2]);

    printf("%f\n",pos_nave[0]);
    printf("%f\n",pos_nave[1]);
    printf("%f\n\n",pos_nave[2]);

    vel_nave[0]=(vel_nave[0]-pos_nave[0])/0.3;
    vel_nave[1]=(vel_nave[1]-pos_nave[1])/0.3;
    vel_nave[2]=(vel_nave[2]-pos_nave[2])/0.3;    

    task_read_socket('X',&acc_nave[0]);
    task_read_socket('Y',&acc_nave[1]);
    task_read_socket('Z',&acc_nave[2]);

    printf("%f\n",vel_nave[0]);
    printf("%f\n",vel_nave[1]);
    printf("%f\n",vel_nave[2]);
    
}