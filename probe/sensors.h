#ifndef __SENSORS_H__
#define __SENSORS_H__

int init_sensors();
int connect_socket(int,int *);
int read_socket(int, float*);

#endif
