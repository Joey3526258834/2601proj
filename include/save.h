#ifndef __SAVE_H__
#define __SAVE_H__


#define _GNU_SOURCE
#include<time.h>
#include "../include/sqlite3.h"
#include<stdio.h>
#include"queue.h"
#include<stdlib.h>


extern void isaday(time_t sec_num);
extern void save_sqlite3(Que_Data_t data);
extern void init_save_module(void);
extern void deinit_save_module(void);

#endif