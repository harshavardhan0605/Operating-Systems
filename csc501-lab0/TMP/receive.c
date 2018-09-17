/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int syscalltrace;
extern unsigned long ctr1000;
/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
SYSCALL	receive()
{
	unsigned long start;
        start = ctr1000;
	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	disable(ps);
	pptr = &proctab[currpid];
	if(syscalltrace){
                pptr->syscallstats[SYS_RECEIVE_ID][0]++;
	}

	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);

	if(syscalltrace){
		 pptr->syscallstats[SYS_RECEIVE_ID][1] = pptr->syscallstats[SYS_RECEIVE_ID][1] + ctr1000 - start;
        }
	return(msg);
}
