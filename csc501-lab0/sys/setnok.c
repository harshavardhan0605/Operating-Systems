/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL	setnok(int nok, int pid)
{
	unsigned long start;
        start = ctr1000;
        struct  pentry *proc;
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_SETNOK_ID][0]++;;
        }

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
if(syscalltrace){
                proc->syscallstats[SYS_SETNOK_ID][1] = proc->syscallstats[SYS_SETNOK_ID][1] + ctr1000 - start;
                }
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);

	if(syscalltrace){
                proc->syscallstats[SYS_SETNOK_ID][1] = proc->syscallstats[SYS_SETNOK_ID][1] + ctr1000 - start;
                }
	return(OK);
}
