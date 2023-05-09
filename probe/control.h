#ifndef __CONTROL_H__
#define __CONTROL_H__

int init_tasks();
void task_read_socket(char, float *);
void task_control(void *);
void task_on_actuator(char);


#endif
