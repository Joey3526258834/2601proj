#include "../include/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Link_t *create_link()
{
	Link_t *plink = malloc(sizeof(Link_t));
	if (NULL == plink)
	{
		perror("malloc error");
		return NULL;
	}

	plink->phead = NULL;
	plink->clen = 0;
	pthread_mutex_init(&(plink->mutex), NULL);

	return plink;
}

int insert_head_link(Link_t *plink, Link_node_t *pnode)
{
	if (NULL == plink || NULL == pnode)
	{
		return -1;
	}
	pnode->pnext = plink->phead;
	plink->phead = pnode;
	plink->clen++;
	printf("DEBUG insert_head_link\n");
	return 0;
}


Link_node_t *find_link(Link_t *plink, char *name)
{
	Link_node_t *ptmp = plink->phead;
	while (ptmp != NULL)
	{
		if (0 == strcmp(ptmp->data.name, name))
		{
			return ptmp;
		}
		ptmp = ptmp->pnext;
	}

	return NULL;
}

Link_node_t *find_link_by_id(Link_t *plink, long int id)
{
	Link_node_t *ptmp = plink->phead;
	while (ptmp != NULL)
	{
		if (ptmp->data.tid == id)
		{
			return ptmp;
		}
		ptmp = ptmp->pnext;
	}

	return NULL;
}