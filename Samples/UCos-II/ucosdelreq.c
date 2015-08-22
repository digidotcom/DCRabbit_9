/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/******************************************************************

      uCOSDelReq.c

  Create maximum amount of system tasks, and then destroy all except
  for idle task and statistics task. Demonstrates the use of
  OSTaskDelReq().

  Each of the created tasks run the same piece of code.  Each task
  writes an 'r' to the stdio window under its name.  When the task
  is deleted, the destructor task writes a 'd' under the task's name.
  Each task deletes itself if so requested, otherwise, it delays itself
  for 1 second.  The destructor task deletes itself after all other
  tasks are deleted.

  The compiler will generate a "User symbol 'X' overrides library
  symbol 'Y'" warning for the hook function redefined in this file.
  This warning can be ignored.

  Note:  This sample pushes the limits of memory when compiled to RAM
         with separate I&D space enabled on a target equipped with only
         128 KB of SRAM.  If a run-time xalloc() error is reported, it
         is most likely due to not enough memory remaining to allocate
         the required stack space.  In this case, reduce the value of
         the OS_MAX_TASKS macro definition, below, to as little as 5.

See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.

******************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

#define OS_MAX_EVENTS                     2  // Maximum number of events (semaphores, queues, mailboxes)
#define OS_MAX_TASKS                     61  // Maximum number of tasks system can create
#define OS_MAX_MEM_PART			            2  // Maximum number of memory partions in system

#define OS_TASK_CREATE_EXT_EN	            1  // Enable extended task creation
#define OS_TASK_DEL_EN			            1  // Enable task deletion
#define OS_TASK_STAT_EN			            1  // Enable statistics task creation
#define OS_TASK_QUERY_EN						1  // Enable task querying
#define OS_CPU_HOOK_EN							0  // Hook functions appear in this file

#define STACK_CNT_512	   OS_MAX_TASKS + 2	// number of 512 byte stacks (initial program + task + stat stacks)

#use "ucos2.lib"

void DestructorTask(void* pdata);
void GenericTask(void* pdata);

void InitDisplay();
void DispStr();

void main()
{
	auto INT8U Error;
	auto INT16U Ticks;
	auto unsigned int i;

	// Initialize uC/OS-II internal data structures
	OSInit();

	InitDisplay();

   Error = OSTaskCreateExt(DestructorTask, NULL, OS_LOWEST_PRIO - 2, 0, 512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

   // Begin multi-tasking by entering the first ready task
	OSStart();
}

void GenericTask(void * pdata)
{
	auto OS_TCB data;

	OSTaskQuery(OS_PRIO_SELF, &data);

   if (data.OSTCBPrio < 20)
		DispStr((data.OSTCBPrio * 4) + 2, 7, "r");
	if (data.OSTCBPrio >= 20 && data.OSTCBPrio < 40)
		DispStr(((data.OSTCBPrio % 20) * 4)  + 2, 10, "r");
	if (data.OSTCBPrio >= 40 && data.OSTCBPrio < 60)
		DispStr(((data.OSTCBPrio % 20) * 4) + 2, 13, "r");

	while(1)
	{
      // check to see if deletion request registered, if so delete self
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
			OSTaskDel(OS_PRIO_SELF);
		OSTimeDly(2);
	}
}

void DestructorTask(void* pdata)
{
   auto INT8U Error;
   auto unsigned int i;

   // Initialize the statistics task
	OSStatInit();

   OSSchedLock();
   for(i = 0; i < OS_MAX_TASKS - 1; i++)
      Error = OSTaskCreateExt(GenericTask, NULL, i, 0, 512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSSchedUnlock();

   // Set counter to 0 so that we can start deleting tasks from the highest to
   // lowest priority.

   // if i rolls over and becomes 0 again, this task will be safe since OSTaskDelReq
   // will return OS_TASK_NOT_EXIST
	for(i = 0; ; i++)
	{
      // if there is still a task other than this task, the statistics task, or the idle task,
      // delete it.
		if(i < OS_MAX_TASKS)
		{
			Error = OSTaskDelReq(i);
			if(Error == OS_NO_ERR)
			{
				while(Error != OS_TASK_NOT_EXIST)
				{
				   Error = OSTaskDelReq(i);
         		OSTimeDly(OS_TICKS_PER_SEC / 5);				  // delay for a bit
				}
				   if (i < 20)
						DispStr((i * 4) + 2, 7, "d");
					if (i >= 20 && i < 40)
						DispStr(((i % 20) * 4)  + 2, 10, "d");
					if (i >= 40 && i < 60)
						DispStr(((i % 20) * 4) + 2, 13, "d");
			}
	   }
		// if all tasks have been deleted, delete this task leaving only idle and stat tasks
		if(i == OS_MAX_TASKS)
			OSTaskDel(OS_PRIO_SELF);
   }
}

nodebug void InitDisplay()
{

	DispStr(0, 1, "                             uC/OS-II Task Deletion                               ");
	DispStr(0, 2, "                            ------------------------                              ");

	DispStr(0,  4, " Task Status:                                                                    ");
	DispStr(0,  5, " ------------                                                                    ");
   DispStr(0,  6, " t00 t01 t02 t03 t04 t05 t06 t07 t08 t09 t10 t11 t12 t13 t14 t15 t16 t17 t18 t19 ");
   DispStr(0,  7, "                                                                                 ");

   DispStr(0,  9, " t20 t21 t22 t23 t24 t25 t26 t27 t28 t29 t30 t31 t32 t33 t34 t35 t36 t37 t38 t39 ");
   DispStr(0, 10, "                                                                                 ");

   DispStr(0, 12, " t40 t41 t42 t43 t44 t45 t46 t47 t48 t49 t50 t51 t52 t53 t54 t55 t56 t57 t58 t59 ");
   DispStr(0, 13, "                                                                                 ");

	DispStr(0, 15, "  r = running/ready                                                              ");
	DispStr(0, 16, "  d = deleted                                                                    ");
   DispStr(0, 18, " System Statistics:                                                              ");
   DispStr(0, 19, " ------------------                                                              ");
}

nodebug void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

nodebug void OSTaskStatHook()
{
	static char outbuf[128];

  	sprintf(outbuf, "     Tasks running      : %5d\n" \
     	             "     CPU usage          : %5d\n" \
        	          "     Ctx switches/second: %5ld", OSTaskCtr, OSCPUUsage, OSCtxSwCtr);
   DispStr(0, 20, outbuf);
	OSCtxSwCtr = 0;
}