/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
	
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SETDEV_ID][0]++;
	}

	short	*nxtdev;

	if (isbadpid(pid)){
		if(syscalltrace){
                proc->syscallstats[SYS_SETDEV_ID][1] = proc->syscallstats[SYS_SETDEV_ID][1] + ctr1000 - start;
        	}
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;

	if(syscalltrace){
                proc->syscallstats[SYS_SETDEV_ID][1] = proc->syscallstats[SYS_SETDEV_ID][1] + ctr1000 - start;
                }
	return(OK);
}
