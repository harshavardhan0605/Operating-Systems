/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep10(int n)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SLEEP10_ID][0]++;;
        }

	STATWORD ps;    
	if (n < 0  || clkruns==0){
	if(syscalltrace){
                proc->syscallstats[SYS_SLEEP10_ID][1] = proc->syscallstats[SYS_SLEEP10_ID][1] + (ctr1000 - start);
                } 
	        return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(syscalltrace){
                proc->syscallstats[SYS_SLEEP10_ID][1] = proc->syscallstats[SYS_SLEEP10_ID][1] + (ctr1000 - start);
        }

	return(OK);
}
