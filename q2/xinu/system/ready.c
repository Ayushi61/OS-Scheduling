/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
qid16	userproclist;

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];

	
	////kprintf("process in ready state %d\n",pid);
	if(prptr->usr_proc==1)
	{
		//if(prptr->tickets>0){
			prptr->prstate = PR_READY;
			insert(pid,userproclist,prptr->tickets);
		//kprintf("##############adding to ready list, sleep complete %d\n",pid);
			resched();
		//}
		//else{
			//prptr->prstate=PR_SLEEP;
		//	resched();
		//}
		
	}
	else{
		insert(pid, readylist, prptr->prprio);
		resched();
	}
	
	
	return OK;
}
