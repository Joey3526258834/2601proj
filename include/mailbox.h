#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#include "list.h"


extern Link_t *create_mailbox();
extern int register_thread_task(Link_t *pmail, pFun_t pfun, char *name);
extern void destroy_mailbox(Link_t *pmail);
extern int recv_msg(Link_t *pmail, Que_Data_t *pdata);
extern int send_msg(Link_t *pmail, char *sendname, Que_Data_t data, char *recvname);

#endif