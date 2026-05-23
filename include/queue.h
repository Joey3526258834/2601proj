#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <time.h>
#include <pthread.h>
#include <semaphore.h>


//共享的数据的数据类型
typedef struct data
{
    int id;    //设备编号
    float temp;
    float hum;
    float light;
    float smoke;
    //xxx
    struct tm tim;
    char sendname[32];
    char recvname[32];
} Que_Data_t;

//队列的结点类型
typedef struct que_node
{
    Que_Data_t data;
    struct que_node *pnext;
}Que_node_t;

//队列的对象类型
typedef struct queue
{
    Que_node_t *phead;
    Que_node_t *ptail;
    int clen;
    pthread_mutex_t mutex;
    sem_t sem;
}Queue_t;

Queue_t *create_queue();
extern void destroy_queue(Queue_t *pque);
extern int is_empty_queue(Queue_t *pque);
extern int push_queue(Queue_t *pque, Que_Data_t data);
extern int pop_queue(Queue_t *pque, Que_Data_t *pdata);
#endif