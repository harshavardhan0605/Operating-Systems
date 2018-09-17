/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*---------------------------------------------------------------------n
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int class =0,pid;
int epoch=0;
void setschedclass(int sched_class)
{
	class = sched_class;
}

int getschedclass()
{
	return class;
}



int resched()
{
	register struct pentry  *optr;  /* pointer to old process entry */
	register struct pentry  *nptr;  /* pointer to new process entry */
	if(getschedclass() ==1)
	{
		register struct qent *q1;
		int sum=0,k=0;
		optr = &proctab[currpid];
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		q1 = &q[rdyhead];
		q1 = &q[q1->qnext];
		while(q1 !=&q[rdytail])
		{	
			sum = sum + q1->qkey;
			q1 = &q[q1->qnext];
		}
		if(sum!=0){
			k = rand() % sum;
		}
		q1= &q[rdytail];
		q1 =&q[q1 ->qprev];
		if(sum!=0){
			while(k> q1->qkey)
			{
				k= k-(q1->qkey);
				q1=&q[q1->qprev];
			} }

		currpid= (int)(q[q1->qnext].qprev);
		dequeue(currpid);
		nptr = &proctab[currpid];
		nptr->pstate = PRCURR;

#ifdef  RTCLOCK
		preempt = QUANTUM;              /* reset preemption counter     */
#endif


		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		return OK;
	}	
	else if(getschedclass()==2)
	{	
		register struct pentry *p1;
		int i;
		optr = &proctab[currpid];
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);}
		if(epoch>0)
		{
			epoch = epoch - (optr->quantumleft - preempt);
			optr->goodness -= (optr->quantumleft - preempt) ;
			if(preempt==0){
				optr->goodness=0;
			}
			optr->quantumleft = preempt;
		}

       newepoch:	if(epoch==0)
		{
			epoch=0;
			/*new epoch creation */
			for(i=0;i<NPROC;i++)
			{
				p1 = &proctab[i];
				if(p1->pstate != PRFREE)
				{
					p1->quantum = (p1->quantumleft/2) + p1->pprio;
					p1->quantumleft= p1->quantum;
					p1->goodness = p1->quantumleft + p1->pprio;
					epoch += p1->quantum;
				}
			}
		}
		int max=0;
		for(i=q[rdytail].qprev; i!=rdyhead;i=q[i].qprev)
		{
			if(max< proctab[i].goodness)
			{
				max = proctab[i].goodness;
				pid=i;
			}
		}	
		currpid=pid;	
		if(currpid!=0 && max!=0 ){
			dequeue(currpid);
			nptr = &proctab[currpid];
			nptr->pstate = PRCURR;
#ifdef  RTCLOCK
			preempt = nptr->quantumleft;
#endif		
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

			return OK;}
			else if(currpid!=0 && max==0){
			epoch=0;
			goto newepoch;		
			}
			else if(currpid==0){
			epoch=0;		
			dequeue(currpid);
			nptr = &proctab[currpid];
			nptr->pstate = PRCURR;
#ifdef  RTCLOCK
			preempt =QUANTUM;
#endif
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

			return OK;}

	}


	else{

		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */

		/* no switch needed if current process priority higher than next*/

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
				(lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}

		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
#endif

		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		/* The OLD process returns here when resumed. */
		return OK;
	}
}

