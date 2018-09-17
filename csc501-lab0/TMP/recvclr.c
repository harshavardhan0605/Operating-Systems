/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern unsigned long ctr1000;
extern int getutim(unsigned long *);
/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{
	unsigned long start;
        start = ctr1000;
	struct  pentry *proc;
	
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_RECVCLR_ID][0]++;
	}

	STATWORD ps;    
	WORD	msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	
if(syscalltrace){
	 proc->syscallstats[SYS_RECVCLR_ID][1] = proc->syscallstats[SYS_RECVCLR_ID][1] + ctr1000 - start;
        }
	return(msg);
}
