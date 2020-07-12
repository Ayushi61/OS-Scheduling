#include <xinu.h>
#include <stdio.h>
void timed_execution(uint32 runtime){
	    while(proctab[currpid].runtime<runtime);
}

void sync_printf(char *fmt, ...)
{
        intmask mask = disable();
        void *arg = __builtin_apply_args();
        __builtin_apply((void*)kprintf, arg, 100);
        restore(mask);
}

int main() {
	pid32 prA, prB, prC;

	sync_printf("\n");

		
	sync_printf("=== TESTCASE 2::  2 processes - context switches ===============\n");
	int i;
	prA = create_user_process(timed_execution, 1024, "timed_execution", 1, 1);
	prB = create_user_process(timed_execution, 1024, "timed_execution", 1, 1);
	set_tickets(prA, 100);
	set_tickets(prB, 100);
	resume(prA);
	resume(prB);
	receive();
	receive();
	sleepms(50); //wait for user processes to terminate	
	kprintf("\nprocess %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prA, proctab[prA].runtime, proctab[prA].turnaroundtime, proctab[prA].num_ctxsw);
        kprintf("process %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prB, proctab[prB].runtime, proctab[prB].turnaroundtime, proctab[prB].num_ctxsw);
	
	sync_printf("================================================================\n\n");	
	for(i=100;i<=10000;i*=2)	
	{	
	prA = create_user_process(timed_execution, 1024, "timed_execution", 1, i);
	prB = create_user_process(timed_execution, 1024, "timed_execution", 1, i);
	set_tickets(prA, 100);
	set_tickets(prB, 100);
	resume(prA);
	resume(prB);
	receive();
	receive();
	sleepms(50); //wait for user processes to terminate	
	kprintf("\nprocess %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prA, proctab[prA].runtime, proctab[prA].turnaroundtime, proctab[prA].num_ctxsw);
        kprintf("process %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prB, proctab[prB].runtime, proctab[prB].turnaroundtime, proctab[prB].num_ctxsw);
	
	sync_printf("================================================================\n\n");	
	}	
	return OK;
}
