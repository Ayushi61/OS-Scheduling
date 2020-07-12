#include <xinu.h>
void burst_execution(uint32 number_burst, uint32 burst_duration,uint32 sleep_duration){
	struct	procent *prptr=&proctab[currpid];
	//int i;
	//**sync_printf("%d sleep duration \n",sleep_duration);
	//**sync_printf("%d burst number \n",number_burst);
	//**sync_printf("%d burst duration \n",burst_duration);

	prptr->burst_duration=burst_duration;
	prptr->numOfBurst=number_burst;
	prptr->completed_burst=0;
	prptr->sleept_dur=sleep_duration;
	//**kprintf("set durations to %d burst dur, %d completed_burst,%d sleep_dur , %d number of burst \n",prptr->burst_duration,prptr->completed_burst,prptr->sleept_dur,prptr->numOfBurst);
	while(prptr->numOfBurst)
	{
		//**kprintf("in while\n");
		while(1)
		{	
			if(prptr->completed_burst==1)
			{
				//**kprintf("cycle %d\n",prptr->numOfBurst);
				prptr->completed_burst=0;
				prptr->burst_duration=burst_duration;
				break;
			}
		}
		//if(flag_print_debug_logs==TRUE)
			//sync_printf("out for sleep %d proc %d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",prptr->numOfBurst,currpid);
		sleepms(sleep_duration);
		--prptr->numOfBurst;
	
	}
	//if(flag_print_debug_logs==TRUE)
		//sync_printf("End of process@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ %d\n",currpid);
	
	//}
}
