#ifndef __LIST_H__
#define __LIST_H__

#include <pthread.h>
#include "queue.h"

//线程任务处理函数指针类型
typedef void *(*pFun_t)(void *);
//线程任务类型
typedef struct task
{
	char name[32];     //线程名称
	pthread_t tid;     //线程id
	pFun_t pfun;       //要执行的线程任务函数
	Queue_t *pque;     //共享数据时的队列
}Link_Data_t;

//链表结点类型
typedef struct link_node
{
	Link_Data_t data;
	struct link_node *pnext;
}Link_node_t;

//链表对对象类型
typedef struct link
{
	Link_node_t *phead;
	int clen;
	pthread_mutex_t mutex;
}Link_t;


extern Link_t *create_link();
extern int insert_head_link(Link_t *plink, Link_node_t *pnode);
extern Link_node_t *find_link(Link_t *plink, char *name);
extern Link_node_t *find_link_by_id(Link_t *plink, long int id);

#endif


