/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void priority_boost()
{
	struct procent *ptold;
	pid32 qelem;
	ptold = &proctab[currpid];
	qid16 array_userlist[3]={user_priority_list1,user_priority_list2,user_priority_list3};
	int lengthOfArray=(int) (sizeof(array_userlist)/sizeof(array_userlist[0]));
	
		//kprintf("performing boost---------- \n");
		boost=0;
		if(ptold->usr_proc==1)
		{
			ptold->time_used=0;
			ptold->qnum=1;
			//if(ptold->prstate==PR_CURR)
			//{
				//kprintf("in here");
				//ptold->prstate=PR_READY;
			//kprintf()
				//enqueue(currpid,array_userlist[(ptold->qnum-1)]);
			//}
			
		}
		//kprintf("\n");
		//printf("passed boost\n");
		int i;
		for(i=0;i<lengthOfArray;i++)
		{
			if(nonempty(array_userlist[i])) // dequeue and enqueue and reset queue number and time used
			{
				qelem=dequeue(array_userlist[i]);
				proctab[qelem].time_used=0;
				proctab[qelem].qnum=1;
				enqueue(qelem,array_userlist[0]);
			}
		}

		//update sleepqueue too
		if(nonempty(sleepq)) //for 1st queue change the entries and for the remaining, dequeue and enqueue
		{
			qelem=firstid(sleepq);
			while(qelem!=queuetail(sleepq))
			{
				if(proctab[qelem].usr_proc==1)
				{
					proctab[qelem].time_used=0;
					proctab[qelem].qnum=1;
					qelem=queuetab[qelem].qnext;		
				}
			}
		}
	
}

void	clkhandler()
{
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/

	/* Decrement the ms counter, and see if a second has passed */
	struct procent *ptr;
	ptr = &proctab[currpid];
	/* Decrement the ms counter, and see if a second has passed */
	//ptr->runtime=ptr->runtime+(ctr1000-ptr->runtimelast);
	
	
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
	ptr->turnaroundtime=ctr1000-ptr->prctr1000;
	if(ptr->prstate==PR_CURR)
	{
		++ptr->runtime;
	}
	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}
	if(ptr->usr_proc==1)
	{
		
		++ptr->time_used;
		
	}
	++boost;
	if(boost>=PRIORITY_BOOST_PERIOD) //boost time exceeded, so reset it to 0 and reset queue to prio 1 queue and time_used to 0
	{
		priority_boost();
		//resched();
	}
		

	if(ptr->burst_duration>0 && ptr->completed_burst==0 && ptr->prstate==PR_CURR && ptr->usr_proc==1 && ptr->numOfBurst>0)
	{
		//kprintf("checking\n");
		//++boost;
		--ptr->burst_duration;
		//++ptr->time_used;
		if(ptr->burst_duration==0)
		{
		 ptr->completed_burst=1;
		}
	}
	//kprintf("in clkhandler \n");
	
	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */
	if((--preempt) <= 0) {
		//preempt = QUANTUM;
		resched();
	}
}
