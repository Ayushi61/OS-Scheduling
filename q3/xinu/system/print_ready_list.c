#include <xinu.h>


void print_ready_list()
{
qid16 curr,last,prev;
curr = queuehead(readylist);
last=lastid(readylist);
//prev=queuetab[curr].qprev;
struct procent *ptr;
//kprintf("queue head of queue is %d\n",queuehead(readylist));
//kprintf("queue head of queue is %d\n",queuehead(readylist));
//kprintf("queue head of queue is %d\n",curr);
//kprintf("queue head of queue is %d\n",curr);
//kprintf("pid of queue is %d\n",queuetab[queuehead(readylist)].qnext);
//kprintf("head is of 1st id is %d\n",readylist);
//kprintf("is empty %d\n",isempty(readylist));
//kprintf("number of process that are active are : %d\n",prcount);
//kprintf("pid of preocess in ready list are : \n");
//kprintf("%d\n",queuetab[curr].qnext);
//kprintf("prev of 1st id is %d\n",prev);
while(curr!=last)
{
	//kprintf("this is print_ready_list");
	//kprintf("%d\n",queuetab[curr].qnext);
	curr=queuetab[curr].qnext;
	ptr=&proctab[curr];
	kprintf("processes in ready list are %s id=%d\n",ptr->prname,curr);
	
	
}

}
