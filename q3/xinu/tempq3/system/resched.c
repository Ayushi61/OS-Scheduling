/* resched.c - resched, resched_cntl */

#include <xinu.h>

//#define DEBUG_CTXSW
struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	pid32 oldpid,qelem;
	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	qid16 array_userlist[3]={user_priority_list1,user_priority_list2,user_priority_list3};
	int lengthOfArray=(int) (sizeof(array_userlist)/sizeof(array_userlist[0]));
	/**boost handling done**/
	/**start with scheduling MLFQ**/
	if(ptold->usr_proc==0) // if it is a system process that called for schedule
	{
		if(currpid!=0)	//if current process is not null
		{
			if(ptold->prstate==PR_CURR)  //system process that is not null and is running state
			{	
				preempt=QUANTUM;
				if (ptold->prprio > firstkey(readylist)) 
				{
					return;
				}
				ptold->prstate = PR_READY;
				insert(currpid, readylist, ptold->prprio);
				oldpid=currpid;
				currpid = dequeue(readylist);
				
			}
			else
			{
				if(firstid(readylist)!=0) //ie some system process found
				{
					preempt=QUANTUM;
					oldpid=currpid;
					currpid = dequeue(readylist);
				}
				else   //since no other system proccess except for null process is present, check for availablity of user proccess
				{
					//kprintf("strange i am here \n");
					oldpid=currpid;
					int i,flag=0,k,power=1;
					for(i=0;i<lengthOfArray;i++)
					{
						if(nonempty(array_userlist[i]))
						{	
							/*compute power to which quantum is to be raised */
							for(k=0;k<i;k++)
							{
								power*=2;
							}
							if(TIME_ALLOTMENT>=(QUANTUM*power))
								preempt=QUANTUM*power;
							else
								preempt=QUANTUM*(TIME_ALLOTMENT/QUANTUM);
							currpid=dequeue(array_userlist[i]);
							flag=1;
							break;
						}
					}
					
					if(flag==0)
					{
						preempt=QUANTUM;
						oldpid=currpid;
						currpid = dequeue(readylist);
					}
				}
			}
		}
		else   //process is null
		{
			if(ptold->prstate==PR_CURR)  //check for any user proccesses
			{
				if(isempty(readylist))
				{
					oldpid=currpid;
					int i,flag=0,k,power=1;
					for(i=0;i<lengthOfArray;i++)
					{
						if(nonempty(array_userlist[i]))
						{	
							/*compute power to which quantum is to be raised */
							for(k=0;k<i;k++)
							{
								power*=2;
							}
							ptold->prstate = PR_READY;
							insert(currpid, readylist, ptold->prprio);
							if(TIME_ALLOTMENT>=(QUANTUM*power))
								preempt=QUANTUM*power;
							else
								preempt=QUANTUM*(TIME_ALLOTMENT/QUANTUM);
							currpid=dequeue(array_userlist[i]);
							flag=1;
							break;
						}
					}
					
					if(flag==0)
					{
						preempt=QUANTUM;
						return;
					}
	
				}
				else
				{
					oldpid=currpid;
					ptold->prstate = PR_READY;
					insert(currpid, readylist, ptold->prprio);
					preempt=QUANTUM;
					currpid=dequeue(readylist);	
				}

			}
			else //if not current state
			{
				if(isempty(readylist))
				{
					oldpid=currpid;
					int i,flag=0,k,power=1;
					for(i=0;i<lengthOfArray;i++)
					{
						if(nonempty(array_userlist[i]))
						{	
							/*compute power to which quantum is to be raised */
							for(k=0;k<i;k++)
							{
								power*=2;
							}
							if(TIME_ALLOTMENT>=(QUANTUM*power))
								preempt=QUANTUM*power;
							else
								preempt=QUANTUM*(TIME_ALLOTMENT/QUANTUM);
							currpid=dequeue(array_userlist[i]);
							flag=1;
							break;
						}
					}
					
					if(flag==0)
					{
						preempt=QUANTUM;
						return;
					}
				}
				else
				{
					oldpid=currpid;
					preempt=QUANTUM;
					currpid=dequeue(readylist);	
				}	
			}
		}
		
	}
	else //resched was called by user process
	{
		if(ptold->prstate==PR_CURR) //if user proccess was running
		{
			
			//check if any system process are present
			//if alloted time is up and resched was called , then set alloted time to 0, and bring down the priority
			if(ptold->time_used>=TIME_ALLOTMENT)
			{
				ptold->time_used=0;
				ptold->prstate=PR_READY;
				//bringing down priorities
				/*int i,flag=0,k,power=1;
				for(i=0;i<lengthOfArray;i++)
				{
					if(ptold->qnum==(i+1))
					{
						if(lengthOfArray!=(i+1))
						{
							ptold->qnum=i+2;
							enqueue(currpid,array_userlist[i+1]);
						}
						else
						{
							ptold->qnum=i+1;
							enqueue(currpid,array_userlist[i]);
						}
					}
					
				}*/
				if(ptold->qnum==1)
				{
					ptold->qnum=2;
					enqueue(currpid,user_priority_list2);
				}
				else
				{
					ptold->qnum=3;
					enqueue(currpid,user_priority_list3);
				}
				if(firstid(readylist)!=0)
				{
					//system process exist, schedule them
					preempt=QUANTUM;
					oldpid=currpid;
					currpid=dequeue(readylist);
				}
				else
				{
					//check for user proccess
					oldpid=currpid;
					int i,flag=0,k,power=1;
					for(i=0;i<lengthOfArray;i++)
					{
						if(nonempty(array_userlist[i]))
						{	
							/*compute power to which quantum is to be raised */
							for(k=0;k<i;k++)
							{
								power*=2;
							}
							if(TIME_ALLOTMENT>=(QUANTUM*power))
								preempt=QUANTUM*power;
							else
								preempt=QUANTUM*(TIME_ALLOTMENT/QUANTUM);
							currpid=dequeue(array_userlist[i]);
							flag=1;
							break;
						}
					}
					
					if(flag==0)
					{
						kprintf("in impossible");
						preempt=QUANTUM;
						oldpid=currpid;
						currpid = dequeue(readylist);

						//return;
					}	
				}
			}
			else  //not due to time elapsed
			{
				ptold->prstate=PR_READY;
				int i,flag=0,k,power=1;
				for(i=0;i<lengthOfArray;i++)
				{
					if(ptold->qnum==i+1)
					{
					
						enqueue(currpid,array_userlist[i]);
						flag=1;
						break;
					}
				}
				if(firstid(readylist)!=0)
				{
					//system process exist, schedule them
					preempt=QUANTUM;
					oldpid=currpid;
					currpid=dequeue(readylist);
				}
				else
				{
					oldpid=currpid;
					int i,flag=0,k,power=1;
					for(i=0;i<lengthOfArray;i++)
					{
						if(nonempty(array_userlist[i]))
						{	
							/*compute power to which quantum is to be raised */
							for(k=0;k<i;k++)
							{
								power*=2;
							}
							if(TIME_ALLOTMENT>=(QUANTUM*power))
								preempt=QUANTUM*power;
							else
								preempt=QUANTUM*(TIME_ALLOTMENT/QUANTUM);
							currpid=dequeue(array_userlist[i]);
							flag=1;
							break;
						}
					}
					
					if(flag==0)
					{
						kprintf("impossible \n");
						preempt=QUANTUM;
						oldpid=currpid;
						currpid = dequeue(readylist);
						/*if(ptold->qnum==1)
							preempt=QUANTUM;
						else if(ptold->qnum==2)
							preempt=QUANTUM*2;
						else
							preempt=QUANTUM*4;
						return;*/
					}
				}
			}
			
			
		}
		else  // called by user process that is not currently running
		{
			//check for system priority
			if(firstid(readylist)!=0)
			{
				oldpid=currpid;
				preempt=QUANTUM;
				dequeue(readylist);

			}
			else{
				//since no processes in ready list, try user proccesses
				oldpid=currpid;
				int i,flag=0,k,power=1;
				for(i=0;i<lengthOfArray;i++)
				{
					if(nonempty(array_userlist[i]))
					{	
						/*compute power to which quantum is to be raised */
						for(k=0;k<i;k++)
						{
							power*=2;
						}
						if(TIME_ALLOTMENT>=(QUANTUM*power))
								preempt=QUANTUM*power;
							else
								preempt=QUANTUM*(TIME_ALLOTMENT/QUANTUM);
						currpid=dequeue(array_userlist[i]);
						flag=1;
						break;
					}
				}
				
				if(flag==0)
				{
					preempt=QUANTUM;
					oldpid=currpid;
					currpid = dequeue(readylist);
					
				}
				
			}
			
		}

	}

	

	
	/*if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
	/*	if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

	/*	ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */
	
/*	oldpid=currpid;
	currpid = dequeue(readylist);*/
	ptnew = &proctab[currpid];
	if(oldpid != currpid)
		ptnew->num_ctxsw=ptnew->num_ctxsw+1;
	//if(flag_print_debug_logs==TRUE){
		//sync_printf("old process id %d number of times context switch is : %d\n",currpid,ptnew->num_ctxsw);
		//sync_printf("total runtime id %d is %d \n",currpid,ptnew->runtime);
	//}
	ptnew->prstate = PR_CURR;
	//preempt = QUANTUM;		/* Reset time slice for process	*/
	#ifdef DEBUG_CTXSW
		if(oldpid != currpid)
			kprintf("ctxsw::%d-%d\n",oldpid,currpid);
	#endif
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
