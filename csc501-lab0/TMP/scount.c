/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SCOUNT_ID][0]++;
        }	

extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
		if(syscalltrace){
                proc->syscallstats[SYS_SCOUNT_ID][1] = proc->syscallstats[SYS_SCOUNT_ID][1] + ctr1000 - start;
        }
		return(SYSERR);
	}
	
	if(syscalltrace){
                proc->syscallstats[SYS_SCOUNT_ID][1] = proc->syscallstats[SYS_SCOUNT_ID][1] + ctr1000 - start;
        }
	return(semaph[sem].semcnt);
}
