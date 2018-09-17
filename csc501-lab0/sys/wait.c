/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_WAIT_ID][0]++;
        }
	
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_WAIT_ID][1] = proc->syscallstats[SYS_WAIT_ID][1] + (ctr1000 - start);
                }
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
if(syscalltrace){
                proc->syscallstats[SYS_WAIT_ID][1] = proc->syscallstats[SYS_WAIT_ID][1] + (ctr1000 - start);
                }
		return pptr->pwaitret;
	}
	restore(ps);

	if(syscalltrace){
                proc->syscallstats[SYS_WAIT_ID][1] = proc->syscallstats[SYS_WAIT_ID][1] + (ctr1000 - start);
                }
	return(OK);
}
