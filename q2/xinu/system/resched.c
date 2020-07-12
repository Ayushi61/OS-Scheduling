/* resched.c - resched, resched_cntl */

#include <xinu.h>
#include <stdlib.h>
//#define DEBUG_CTXSW
struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
int16 lottery_winner(qid16 q){
	uint16 winner,total_tickets,comp_id;
	total_tickets=0;
	comp_id=firstid(q);
	//kprintf("in lottery_winner\n");
	while(comp_id!=queuetail(q)){
		total_tickets=total_tickets+proctab[comp_id].tickets;
		//kprintf("%d is the 1 proc tickets\n",proctab[comp_id].tickets);
		//kprintf("%d is the total tickets\n",total_tickets);
		comp_id=queuetab[comp_id].qnext;
	}
	if(total_tickets>0)
	{
	winner=rand()%total_tickets;
	}
	else
		winner=-1;
	//kprintf("going out of winner with winner as %d\n",winner);
	return winner;
	

}

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	pid32 oldpid;
	/* If rescheduling is deferred, record attempt and return */
	
	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}
	
	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	/***if user process calls the resched ***/
	
	
	if(ptold->usr_proc==1) //if user process
	{
		
		//kprintf("%d\n",firstid(readylist));
		if( firstid(readylist)!=0){ // if readlylist has processes other than null
			//kprintf("switching to system process############");
			if (ptold->prstate == PR_CURR){		//if proc is currently running, change state to ready and then add to user queue
				
				ptold->prstate = PR_READY;
				//kprintf("!!!!!!!!!insterting process %d in userlist\n",currpid);
				insert(currpid, userproclist, ptold->tickets);
				
			}
			oldpid=currpid;
			currpid = dequeue(readylist);
		}
		else
		{		//if null is the only proccess in ready list run user process
			if(isempty(userproclist))
			{	
				//if userlist is empty and proccess is currently running
				if (ptold->prstate == PR_CURR){
					//kprintf("returningggggggggggggggg\n");
					//kprintf("%d is the 1st id in readyist\n",firstid(readylist));
					//*****//kprintf("%d is the state of current process\n")
					return;
				}
				else{	// run null proccess
					//kprintf("no process in running state shidt to systemd\n");
					oldpid=currpid;
					currpid = dequeue(readylist);
					//kprintf("will run process %d\n",currpid);
				}
				
			}
			//**********//kprintf("picking lottery\n");
			//if there are other processes in userlist
			else{ 
				if (ptold->prstate == PR_CURR){
					ptold->prstate = PR_READY;
					//kprintf("insterting process %d in userlist\n",currpid);
					insert(currpid, userproclist, ptold->tickets);
				}
				oldpid=currpid;
				int16 winner=lottery_winner(userproclist);
				if(winner>=0)
				{
					currpid = dequeue_userproclist(userproclist,winner);
				}
				else{
					oldpid=currpid;
					currpid = dequeue(readylist);
				}
			}
				
						
		}
		
	}
	else{
		//if calling proccess is system process
		if(currpid!=0){
			if (ptold->prstate == PR_CURR){
			
				if (ptold->prprio > firstkey(readylist)) {
					//kprintf("CONTINUING system 1st\n");
					return;
				}
				ptold->prstate = PR_READY;
				insert(currpid, readylist, ptold->prprio);
				oldpid=currpid;
				currpid = dequeue(readylist);
			}
			else{
				if( firstid(readylist)==0) 
				{
					if(isempty(userproclist))
					{
						//kprintf("CONTINUING system ---not non empty \n");
						oldpid=currpid;
						currpid = dequeue(readylist);
					}
					else{
						//kprintf("switching lottery\n");	
						oldpid=currpid;
						int16 winner=lottery_winner(userproclist);
						//kprintf("%d is winner \n",winner);
						if(winner>=0)
						{
						currpid = dequeue_userproclist(userproclist,winner);
						}
						else{
							oldpid=currpid;
							currpid = dequeue(readylist);
						}
					}
				}
				else if(nonempty(readylist)){
					//kprintf("CONTINUING system last else\n");
					oldpid=currpid;
					currpid = dequeue(readylist);
				}
				else{
					//kprintf("returning\n");
					return;
				}	
			}
		}
		else{ // if current pid is null
			if(isempty(readylist))
			{
				
				if(nonempty(userproclist))
				{
					if (ptold->prstate == PR_CURR){
						ptold->prstate = PR_READY;
						insert(currpid, readylist, ptold->prprio);
					}
					//kprintf("switching lottery\n");	
					oldpid=currpid;
					int16 winner=lottery_winner(userproclist);
					//kprintf("%d is winner \n",winner);
					if(winner>=0)
					{
						currpid = dequeue_userproclist(userproclist,winner);
					}
					else{
						oldpid=currpid;
						currpid = dequeue(readylist);
					}
				}
				else{
					//kprintf("continuing null process");
					return;
				}
			}
			else{
				ptold->prstate = PR_READY;
				insert(currpid, readylist, ptold->prprio);
				//kprintf("CONTINUING system ---not non empty \n");
				oldpid=currpid;
				currpid = dequeue(readylist);
			}
		}		
	}
	
	//kprintf("current pid after dequeue is %d\n",currpid);
	//kprintf("out of ready %d\n",currpid);
	ptnew = &proctab[currpid];
	if(oldpid != currpid)
		ptnew->num_ctxsw=ptnew->num_ctxsw+1;
	if(flag_print_debug_logs==TRUE){
		//sync_printf("old process id %d number of times context switch is : %d\n",currpid,ptnew->num_ctxsw);
		//sync_printf("total runtime id %d is %d \n",currpid,ptnew->runtime);
	}
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	#ifdef DEBUG_CTXSW
		if(oldpid != currpid)
			kprintf("ctxsw::%d-%d\n",oldpid,currpid);
	#endif
	ptnew->runtimelast=ctr1000;
	//sync_printf("clock time before %d, runtime last == %d \n",ptnew->runtime,ptnew->runtimelast);
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	/* xinu code-----------------------------------
	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
//		if (ptold->prprio > firstkey(readylist)) {
//			return;
//		}

		/* Old process will no longer remain current */
		
//		ptold->prstate = PR_READY;
//		insert(currpid, readylist, ptold->prprio);
//	}

	/* Force context switch to highest priority ready process */
/*	oldpid=currpid;
	currpid = dequeue(readylist);
	//kprintf("out of ready %d\n",currpid);
	ptnew = &proctab[currpid];
	ptnew->num_ctxsw=ptnew->num_ctxsw+1;
	if(flag_print_debug_logs==TRUE){
		sync_printf("old process id %d number of times context switch is : %d\n",currpid,ptnew->num_ctxsw);
		sync_printf("total runtime id %d is %d \n",currpid,ptnew->runtime);
	}
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
/*	#ifdef DEBUG_CTXSW
		if(oldpid != currpid)
		sync_printf("ctxsw::%d-%d\n",oldpid,currpid);
	#endif
	ptnew->runtimelast=ctr1000;
	sync_printf("clock time before %d, runtime last == %d \n",ptnew->runtime,ptnew->runtimelast);
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
