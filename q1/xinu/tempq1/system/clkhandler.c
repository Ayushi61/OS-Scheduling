/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/

	/* Decrement the ms counter, and see if a second has passed */
	struct procent *ptr;
	ptr = &proctab[currpid];
	/* Decrement the ms counter, and see if a second has passed */
	//ptr->runtime=ptr->runtime+(ctr1000-ptr->runtimelast);
	
	ptr->turnaroundtime=ctr1000-ptr->prctr1000;
	if(ptr->prstate==PR_CURR)
	{
		++ptr->runtime;
	}
	if((--count1000) <= 0) {

		/* One second has passed, so increment seconds count */

		clktime++;
		ctr1000=clktime*1000;
		/* Reset the local ms counter for the next second */

		count1000 = 1000;
		
	}
	else{
		ctr1000=clktime*1000+(1000-count1000);
	}

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */
	if(ptr->burst_duration>0 && ptr->completed_burst==0 && ptr->prstate==PR_CURR && ptr->usr_proc==1 && ptr->numOfBurst>0)
	{
		//kprintf("checking\n");
		--ptr->burst_duration;
		if(ptr->burst_duration==0)
		{
		 ptr->completed_burst=1;
		}
	}
	if((--preempt) <= 0) {
		preempt = QUANTUM;
		resched();
	}
}
