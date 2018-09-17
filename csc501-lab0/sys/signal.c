/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SIGNAL_ID][0]++;
        }
	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_SIGNAL_ID][1] = proc->syscallstats[SYS_SIGNAL_ID][1] + ctr1000 - start;
                }
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);

if(syscalltrace){
                proc->syscallstats[SYS_SIGNAL_ID][1] = proc->syscallstats[SYS_SIGNAL_ID][1] + ctr1000 - start;
                }
	return(OK);
}
