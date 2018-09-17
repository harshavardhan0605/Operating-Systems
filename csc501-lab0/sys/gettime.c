/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>

extern int syscalltrace;
extern unsigned long ctr1000;
extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
        if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_GETTIME_ID][0]++;
        	proc->syscallstats[SYS_GETTIME_ID][1] = proc->syscallstats[SYS_GETTIME_ID][1] + ctr1000 - start;
	}
    return OK;
}
