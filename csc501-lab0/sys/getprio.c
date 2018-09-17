/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	unsigned long start;
        start = ctr1000;
	STATWORD ps;    
	struct	pentry	*pptr, *proc;;
	
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_GETPRIO_ID][0]++;;
        }	

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_GETPRIO_ID][1] = proc->syscallstats[SYS_GETPRIO_ID][1] + ctr1000 - start;
        }
		return(SYSERR);
	}
	restore(ps);
	
	if(syscalltrace){
                proc->syscallstats[SYS_GETPRIO_ID][1] = proc->syscallstats[SYS_GETPRIO_ID][1] + ctr1000 - start;
        }
	return(pptr->pprio);
}
