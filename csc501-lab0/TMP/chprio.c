/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
extern int syscalltrace;
SYSCALL chprio(int pid, int newprio)
{	
	unsigned long start;
	start = ctr1000;

	STATWORD ps;    
	struct	pentry	*pptr, *proc;
	// trace syscall : update call count
	if(syscalltrace){
		proc = &proctab[currpid];
		proc->syscallstats[SYS_CHPRIO_ID][0]++;;
	}

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_CHPRIO_ID][1] = proc->syscallstats[SYS_CHPRIO_ID][1] + ctr1000 - start;
        	}	
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);

	//trace syscall : update call time
	if(syscalltrace){
		proc->syscallstats[SYS_CHPRIO_ID][1] = proc->syscallstats[SYS_CHPRIO_ID][1] + ctr1000 - start;
	}

	return(newprio);
}
