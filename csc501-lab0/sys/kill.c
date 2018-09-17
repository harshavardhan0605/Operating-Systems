/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	unsigned long start;
        start = ctr1000;

	STATWORD ps;    
	struct	pentry	*pptr, *proc;		/* points to proc. table for pid*/
	int	dev;
	if(syscalltrace){
                proc = &proctab[currpid];
                proc->syscallstats[SYS_KILL_ID][0]++;
        }
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		if(syscalltrace){
			proc->syscallstats[SYS_KILL_ID][1] = proc->syscallstats[SYS_KILL_ID][1] + ctr1000 - start;
		}
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	
	if(syscalltrace){
	proc->syscallstats[SYS_KILL_ID][1] = proc->syscallstats[SYS_KILL_ID][1] + ctr1000 - start;
	}
	return(OK);
}
