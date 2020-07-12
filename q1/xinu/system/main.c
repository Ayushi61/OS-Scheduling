#include <xinu.h>
#include <stdio.h>

void sync_printf(char *fmt, ...)
{
        intmask mask = disable();
        void *arg = __builtin_apply_args();
        __builtin_apply((void*)kprintf, arg, 100);
        restore(mask);
}

int main() {
	pid32 prA, prB;

	sync_printf("\n");
	sync_printf("=== TESTCASE 1:: 1 process with burst execution - context switches ======\n");	
	prA = create_user_process(burst_execution, 1024, "burst_execution", 3, 4, 40, 40);
	
	resume(prA);
	receive();
	sleepms(20); //wait for user process to terminate	
	kprintf("\nprocess %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prA, proctab[prA].runtime, proctab[prA].turnaroundtime, proctab[prA].num_ctxsw);
	sync_printf("=========================================================================\n\n");	
	sync_printf("printing readylist \n");
	print_ready_list();
	return OK;
}
