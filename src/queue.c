#include "../include/queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue_t *create_queue()
{
    Queue_t *pque = malloc(sizeof(Queue_t));
    if (NULL == pque)
    {
        perror("malloc error create_queue");
        return NULL;
    }
    pque->phead = NULL;
    pque->ptail = NULL;
    pque->clen = 0;

    printf("DEBUG create_queue malloc success\n");

    pthread_mutex_init(&(pque->mutex), NULL);
    sem_init(&(pque->sem), 0, 0);

    return pque;
}

void destroy_queue(Queue_t *pque)
{
    while (pque->phead != NULL)
    {
        Que_node_t *ptmp = pque->phead;
        pque->phead = ptmp->pnext;
        free(ptmp);
    }
    free(pque);
}

int is_empty_queue(Queue_t *pque)
{
    return NULL == pque->phead;
}

int push_queue(Queue_t *pque, Que_Data_t data)
{
    Que_node_t *pnode = malloc(sizeof(Que_node_t));
    if (NULL == pnode)
    {
        perror("malloc error push_queue");
        return -1;
    }
    pnode->data = data;
    pnode->pnext = NULL;

    if (is_empty_queue(pque))
    {
        pque->phead = pnode;
        pque->ptail = pnode;
    }
    else
    {
        pque->ptail->pnext = pnode;
        pque->ptail = pnode;
    }
    pque->clen++;

    return 0;
}


int pop_queue(Queue_t *pque, Que_Data_t *pdata)
{
    if (is_empty_queue(pque))
    {
        return -1;
    }

    Que_node_t *ptmp = pque->phead;
    if (pdata != NULL)
    {
        *pdata = ptmp->data;
    }
    pque->phead = ptmp->pnext;
    if (NULL == pque->phead)
    {
        pque->ptail = NULL;
    }
    free(ptmp);
    pque->clen--;

    return 0;
}
