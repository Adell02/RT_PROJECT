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
float pos_nave [3]= {0,0,0}, vel_rel[3]= {0,0,0}, acc_nave[3]={0,0,0};

// Medidas sonda
float vel_sonda[3] = {0,0,0};  // X,Y,Z
float lect_X=0,lect_Y=0,lect_Z=0;


int init_tasks()
{    
    PTASK control,init_cond;

    Task_create(&init_cond,"Initial Conditions",ini_cond_func,0,1000,1);
    Task_create(&control,"Control Task",task_control,0,1000,1);

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
    //send_msg_actuator(fd,"stop");
    
}

void task_off_actuator(char val)
{
    int fd=1;

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

    //send_msg_actuator(fd,"start");  // 50ms accelerating --> +0.01m/s
    send_msg_actuator(fd,"stop");
    
}

void task_control(void * a)
{

    float min_dist;


    ///////// X AXIS  //////////
    min_dist = 4*(vel_rel[0])*(vel_rel[0])/0.2; // Proveniente de: V(t) = Vo + a * dist/(Vo*4)   donde el 4 es un factor de seguridad
    if (lect_X < 0 && -lect_X > min_dist && vel_rel[0] > -MAX_SPEED*0.8)
    {
        task_on_actuator('E');
        task_off_actuator('W');
    }
    else if (lect_X > 0 && lect_X > min_dist && vel_rel[0] < MAX_SPEED*0.8)
    {
        task_on_actuator('W');
        task_off_actuator('E');
    }

 
    
    ///////// Y AXIS  //////////

    min_dist = 4*(vel_rel[1])*(vel_rel[1])/0.2;
    if (lect_Y < 0 && -lect_Y > min_dist && vel_rel[1] > -MAX_SPEED*0.8)
    {
        task_on_actuator('S');
        task_off_actuator('N');
    }
    else if (lect_Y > 0 && lect_Y > min_dist && vel_rel[1] < MAX_SPEED*0.8)
    {
        task_on_actuator('N');
        task_off_actuator('S');
    }

 
    
    ///////// Z AXIS  //////////
    
    min_dist = 4*(vel_rel[2])*(vel_rel[2])/0.2;
    if (lect_Z < 0 && -lect_Z > min_dist && vel_rel[2] > -MAX_SPEED*0.8)
    {
        task_on_actuator('D');
        task_off_actuator('U');
    }
    else if (lect_Z > 0 && lect_Z > min_dist && vel_rel[2] < MAX_SPEED*0.8)
    {
        task_on_actuator('U');
        task_off_actuator('D');
    }
    


    printf("Vel X\t\tVel Y\t\tVel Z\n%f\t%f\t%f\n",vel_rel[0],vel_rel[1],vel_rel[2]);


}

void ini_cond_func (void * a)
{
    
    task_read_socket('X',&pos_nave[0]);
    task_read_socket('Y',&pos_nave[1]);
    task_read_socket('Z',&pos_nave[2]);

    task_read_socket('X',&vel_rel[0]);
    task_read_socket('Y',&vel_rel[1]);
    task_read_socket('Z',&vel_rel[2]);
    
    // printf("%f\n",vel_nave[0]);
    // printf("%f\n",vel_nave[1]);
    // printf("%f\n\n",vel_nave[2]);

    // printf("%f\n",pos_nave[0]);
    // printf("%f\n",pos_nave[1]);
    // printf("%f\n\n",pos_nave[2]);
    lect_X = vel_rel[0];
    lect_Y = vel_rel[1];
    lect_Z = vel_rel[2];

    vel_rel[0]=(vel_rel[0]-pos_nave[0])/0.3;
    vel_rel[1]=(vel_rel[1]-pos_nave[1])/0.3;
    vel_rel[2]=(vel_rel[2]-pos_nave[2])/0.3;    

    if (vel_rel[0] > 0)
    {
        task_on_actuator('W');
        task_off_actuator('E');
        
    }
    else if(vel_rel[0] < 0)
    {
        task_on_actuator('E');
        task_off_actuator('W');
        
    }

    if (vel_rel[1] > 0)
    {
        task_on_actuator('N');
        task_off_actuator('S');
    }
    else if(vel_rel[1] < 0)
    {
        task_on_actuator('S');
        task_off_actuator('N');
    }

    if (vel_rel[2] > 0)
    {
        task_on_actuator('U');
        task_off_actuator('D');
    }
    else if(vel_rel[2] < 0)
    {
        task_on_actuator('D');
        task_off_actuator('U');
    }

    
    
}