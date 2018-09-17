/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SLEEP_ID][0]++;;
        }
	STATWORD ps;    
	if (n<0 || clkruns==0){
		if(syscalltrace){
                proc->syscallstats[SYS_SLEEP_ID][1] = proc->syscallstats[SYS_SLEEP_ID][1] + (ctr1000 - start);
                }
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		if(syscalltrace){
                proc->syscallstats[SYS_SLEEP_ID][1] = proc->syscallstats[SYS_SLEEP_ID][1] + (ctr1000 - start);
                }
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);

	if(syscalltrace){
                proc->syscallstats[SYS_SLEEP_ID][1] = proc->syscallstats[SYS_SLEEP_ID][1] + (ctr1000 - start);
                }
	return(OK);
}
