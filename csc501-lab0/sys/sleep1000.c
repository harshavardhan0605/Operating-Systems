/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep1000(int n)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SLEEP1000_ID][0]++;;
        }

	STATWORD ps;    

	if (n < 0  || clkruns==0){
		if(syscalltrace){
                proc->syscallstats[SYS_SLEEP1000_ID][1] = proc->syscallstats[SYS_SLEEP1000_ID][1] + (ctr1000 - start);
                } 
	        return(SYSERR);

	}
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(syscalltrace){
                proc->syscallstats[SYS_SLEEP1000_ID][1] = proc->syscallstats[SYS_SLEEP1000_ID][1] + (ctr1000 - start);
                }
	return(OK);
}
