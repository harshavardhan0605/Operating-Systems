#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long    *esp;

void printprocstks(int priority){
	struct pentry *p;
	int i;
	for(i = 1 ; i < NPROC; i++){
		p = &proctab[i];
		if(p->pprio < priority && p->pstate != PRFREE){
			kprintf("Process [%s]\n", p->pname );
			kprintf("\tpid: %d\n", i);
			kprintf("\tpriority: %d\n", p->pprio);
			kprintf("\tbase: 0x%08x\n",p->pbase);
			kprintf("\tlimit: 0x%08x\n",p->plimit);
			kprintf("\tlen: %d\n",p->pstklen);
			if(p->pstate == PRCURR){
				asm("movl %esp,esp");
				kprintf("\tpointer: 0x%08x\n", esp);
			} else {
			kprintf("\tpointer: 0x%08x\n", p->pesp);
			}
		}
			
	}
}

