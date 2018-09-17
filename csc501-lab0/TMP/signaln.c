/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SIGNALN_ID][0]++;;
        }

	STATWORD ps;    
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_SIGNALN_ID][1] = proc->syscallstats[SYS_SIGNALN_ID][1] + ctr1000 - start;
                }
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);

	if(syscalltrace){
                proc->syscallstats[SYS_SIGNALN_ID][1] = proc->syscallstats[SYS_SIGNALN_ID][1] + ctr1000 - start;
                }
	return(OK);
}
