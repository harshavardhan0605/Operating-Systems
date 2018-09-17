/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(int pid, WORD msg)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
	
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SEND_ID][0]++;
	}

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || ( (pptr= &proctab[pid])->pstate == PRFREE)
	   || pptr->phasmsg != 0) {
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_SEND_ID][1] = proc->syscallstats[SYS_SEND_ID][1] + ctr1000 - start;
        }
		return(SYSERR);
	}
	pptr->pmsg = msg;
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV)	/* if receiver waits, start it	*/
		ready(pid, RESCHYES);
	else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	restore(ps);

	if(syscalltrace){
                proc->syscallstats[SYS_SEND_ID][1] = proc->syscallstats[SYS_SEND_ID][1] + ctr1000 - start;
        }
	return(OK);
}
