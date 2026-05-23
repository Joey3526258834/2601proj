#ifndef __DEMO_APP_H__
#define __DEMO_APP_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../include/queue.h"


extern int get_data_dht11(Que_Data_t *pdata);
extern int get_data_bh1750(Que_Data_t *pdata);
extern int sensor_mq135(Que_Data_t *pdata);
#endif