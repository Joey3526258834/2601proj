#include "../include/mailbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Link_t *create_mailbox()
{
	Link_t *plink = create_link();
	if (NULL == plink)
	{
		return NULL;
	}
	return plink;
}

int register_thread_task(Link_t *pmail, pFun_t pfun, char *name)
{
	Link_node_t *pnode = malloc(sizeof(Link_node_t));
	if (NULL == pnode)
	{
		perror("malloc error register_thread_task");
		return -1;
	}
	printf("DEBUG register_thread_task malloc success\n");

	strcpy(pnode->data.name, name);
	pnode->data.pfun = pfun;
	pnode->data.pque = create_queue();

	insert_head_link(pmail, pnode);

	int ret = pthread_create(&(pnode->data.tid), NULL, pnode->data.pfun, NULL);
	if (ret != 0)
	{
		perror("pthread_create error");
		return -1;
	}

	return 0;
}

void destroy_mailbox(Link_t *pmail)
{
	Link_node_t *ptmp = pmail->phead;
	while (ptmp)
	{
		pthread_join(ptmp->data.tid, NULL);
		pthread_mutex_destroy(&(ptmp->data.pque->mutex));
		destroy_queue(ptmp->data.pque);
		ptmp = ptmp->pnext;
	}

	while (pmail->phead != NULL)
	{
		ptmp = pmail->phead;
		pmail->phead = ptmp->pnext;
		free(ptmp);
	}
	free(pmail);

	return;
}

int send_msg(Link_t *pmail, char *sendname, Que_Data_t data, char *recvname)
{
	strcpy(data.sendname, sendname);
	strcpy(data.recvname, recvname);

	printf("DEBUG send_msg\n");
	Link_node_t *pnode = find_link(pmail, recvname);
	if (NULL == pnode)
	{
		printf("Not found %s\n", recvname);
		return -1;
	}
	pthread_mutex_lock(&(pnode->data.pque->mutex));
	push_queue(pnode->data.pque, data);
	pthread_mutex_unlock(&(pnode->data.pque->mutex));
	sem_post(&(pnode->data.pque->sem)); //+1
	printf("DEBUG send_msg end\n");
	return 0;
}

int recv_msg(Link_t *pmail, Que_Data_t *pdata)
{
	pthread_t tid = pthread_self();
	Link_node_t *pnode = find_link_by_id(pmail, tid);
	if (NULL == pnode)
	{
		printf("Not found tid %lx thread\n", tid);
		return -1;
	}
	sem_wait(&(pnode->data.pque->sem)); // >0  ==0
	pthread_mutex_lock(&(pnode->data.pque->mutex));
	if (!is_empty_queue(pnode->data.pque))
	{
		pop_queue(pnode->data.pque, pdata);
		pthread_mutex_unlock(&(pnode->data.pque->mutex));
	}
	else
	{
		pthread_mutex_unlock(&(pnode->data.pque->mutex));
	}

	return 0;
}
