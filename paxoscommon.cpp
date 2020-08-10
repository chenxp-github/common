#include "paxoscommon.h"
#include "syslog.h"

struct paxos_config paxos_config =
{
    LOG_LEVEL_DEBUG,   /* verbosity */
    2048,              /* learner_instances */
    1,                 /* learner_catchup */
    2000,              /* proposer_timeout */
    16,               /* proposer_preexec_window */
};

int paxos_quorum(int acceptors)
{
    return (acceptors/2)+1;
}
