#ifndef _CS_MESSAGE_H_
#define _CS_MESSAGE_H_ 1

#include "cs_common.h"

typedef struct message message;
struct message
{
    msg_seqid_t iseqid; 
    coroid_t sender;
    coroid_t receiver;
    struct mbuf *buf; 
};

rstatus_t msg_send(schduler *sched, message *msg);
message* msg_receive(coroutine *co);

#endif  //_CS_MESSAGE_H_ 
