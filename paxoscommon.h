#ifndef __PAXOSCOMMON_H
#define __PAXOSCOMMON_H

#include "cruntime.h"

typedef enum {
    UNKNOWN = 0,        //unknown
    PREPARE_REQS=1,     //Phase 1a, P->A
    PREPARE_ACKS=2,     //Phase 1b, A->P
    ACCEPT_REQS=4,      //Phase 2a, P->A
    ACCEPT_ACKS=8,      //Phase 2b, A->L
    REPEAT_REQS=16,     //For progress, L -> A
    SUBMIT=32,          //Clients to leader
    LEADER_ANNOUNCE=64, //Oracle to proposers
    ALIVE_PING=65       //Proposers to oracle
}PAXOS_MSG_CODE;


#define MAX_N_OF_PROPOSERS 10

/* Configuration */
struct paxos_config
{ 
    int verbosity;
    
    /* Learner */
    int learner_instances;
    int learner_catch_up;
    
    /* Proposer */
    uint32_t proposer_timeout;
    int proposer_preexec_window;
    
    /* Acceptor */
};

extern struct paxos_config paxos_config;

int paxos_quorum(int acceptors);

#endif

