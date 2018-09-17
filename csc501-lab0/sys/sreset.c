/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL sreset(int sem, int count)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SRESET_ID][0]++;
        }

	STATWORD ps;    
	struct	sentry	*sptr;
	int	pid;
	int	slist;

	disable(ps);
	if (isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE) {
		restore(ps);
	if(syscalltrace){
                proc->syscallstats[SYS_SRESET_ID][1] = proc->syscallstats[SYS_SRESET_ID][1] + (ctr1000 - start);
                }			
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	while ((pid=getfirst(slist)) != EMPTY)
		ready(pid,RESCHNO);
	sptr->semcnt = count;
	resched();
	restore(ps);
	if(syscalltrace){
                proc->syscallstats[SYS_SRESET_ID][1] = proc->syscallstats[SYS_SRESET_ID][1] + (ctr1000 - start);
                }
	return(OK);
}
