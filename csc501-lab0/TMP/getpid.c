/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	unsigned long start;
        start = ctr1000;
	struct  pentry *proc;
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_GETPID_ID][0]++;
	
                proc->syscallstats[SYS_GETPID_ID][1] = proc->syscallstats[SYS_GETPID_ID][1] + (ctr1000 - start);
        }	
	return(currpid);
}
