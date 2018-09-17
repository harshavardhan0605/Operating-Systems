/* recvtim.c - recvtim */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
SYSCALL	recvtim(int maxwait)
{
	unsigned long start;
        start = ctr1000;
	
	STATWORD ps;    
	struct	pentry	*pptr, *proc;
	int	msg;
	
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_RECVTIM_ID][0]++;
	}

	if (maxwait<0 || clkruns == 0){
		if(syscalltrace){
			 proc->syscallstats[SYS_RECVTIM_ID][1] = proc->syscallstats[SYS_RECVTIM_ID][1] + ctr1000 - start;
        	}	

		return(SYSERR);
	}
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait		*/
	        insertd(currpid, clockq, maxwait*1000);
		slnempty = TRUE;
		sltop = (int *)&q[q[clockq].qnext].qkey;
	        pptr->pstate = PRTRECV;
		resched();
	}
	if ( pptr->phasmsg ) {
		msg = pptr->pmsg;	/* msg. arrived => retrieve it	*/
		pptr->phasmsg = FALSE;
	} else {			/* still no message => TIMEOUT	*/
		msg = TIMEOUT;
	}
	restore(ps);
	if(syscalltrace){
	 proc->syscallstats[SYS_RECVTIM_ID][1] = proc->syscallstats[SYS_RECVTIM_ID][1] + ctr1000 - start;
	}
	return(msg);
}
