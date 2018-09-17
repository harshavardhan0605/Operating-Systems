#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
int syscalltrace;
const char *syscallname[MAX_SYS_CALL_TRACE] = {
                "sys_freemem",
                "sys_chprio",
                "sys_getpid",
                "sys_getprio",
                "sys_gettime",
                "sys_kill",
                "sys_receive",
                "sys_recvclr",
                "sys_recvtim",
                "sys_resume",
                "sys_scount",
                "sys_sdelete",
                "sys_send",
                "sys_setdev",
                "sys_setnok",
                "sys_screate",
                "sys_signal",
                "sys_signaln",
                "sys_sleep",
                "sys_sleep10",
                "sys_sleep100",
                "sys_sleep1000",
                "sys_sreset",
                "sys_stacktrace",
                "sys_suspend",
                "sys_unsleep",
                "sys_wait"
};

void printsyscallsummary()
{
	int i, j;
	struct pentry *p, *pr;
	int header=0;
	for(i = 0; i < NPROC; i++)
	{
		header = 1;
		p = &proctab[i];
		for(j = 0; j < MAX_SYS_CALL_TRACE; j++){
			if(p->syscallstats[j][0] > 0){
				if(header) {
				kprintf("Process [pid:%d], name : %s\n", i, p->pname);
				header =0;
				}	
			kprintf("\tSyscall: %s, count: %d, average execution time: %d\n", syscallname[j], p->syscallstats[j][0], p->syscallstats[j][1]/p->syscallstats[j][0] );
				}	
		}
	}

}


void syscallsummary_start(){
	resetsyscallstats();
	syscalltrace = 1;
}


void syscallsummary_stop(){
        syscalltrace = 0;
}

void resetsyscallstats(){
int i, j;
struct pentry *p;

	for(i = 0; i < NPROC; i++)
        {
                p = &proctab[i];
                for(j = 0; j < MAX_SYS_CALL_TRACE; j++){
                	p->syscallstats[j][0] = 0;
			p->syscallstats[j][1] = 0;
	}
	}
}
