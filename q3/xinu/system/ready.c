/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
qid16 user_priority_list1;
qid16 user_priority_list2;
qid16 user_priority_list3;

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
	
	prptr->prstate = PR_READY;
	if(prptr->usr_proc==0)
	{
		
		insert(pid, readylist, prptr->prprio);
	}
	else
	{
		if(prptr->qnum==1)
			enqueue(pid,user_priority_list1);
		else if (prptr->qnum==2)
			enqueue(pid,user_priority_list2);
		else
			enqueue(pid,user_priority_list3);
	}
	resched();

	return OK;
}
