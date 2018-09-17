/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
SYSCALL resume(int pid)
{
	unsigned long start;
        start = ctr1000;
	STATWORD ps;    
	struct	pentry	*pptr, *proc;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_RESUME_ID][0]++;
	}
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		if(syscalltrace){
		 proc->syscallstats[SYS_RESUME_ID][1] = proc->syscallstats[SYS_RESUME_ID][1] + ctr1000 - start;       
 		}	
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);

	if(syscalltrace){
                 proc->syscallstats[SYS_RESUME_ID][1] = proc->syscallstats[SYS_RESUME_ID][1] + ctr1000 - start;
	}
	return(prio);
}
