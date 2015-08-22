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
/*******************************************************************************

      uCOSJRab1.c

      Demonstration of uC/OS-II.  This application has ten tasks, and can
      make use of the uC/OS-II statistics task if OSTaskStatHook is uncommented
      at the end of this file.  This application requires the JackRabbit
      development board as it uses LEDs DS1-DS4 and buttons S1-S4.

		Task0 & Task 1 control DS1 & DS2.  Delay times for toggling the LEDs
		are determined by Task0 and then sent to Task1 via a mailbox.  Task5
		reports information about these tasks by monitoring S1.  Delay times
		start small and get increasingly bigger.

		Task0										Task1
		Determine DelayTime			 +->	Receive DelayTime
		  used by task0 & task1		 |
		toggle DS1						 |		Toggle DS2
		Send DelayTime	--> DelayMBox+		Resume Task0
		Suspend self

		Task5
		if button S1 is down
		 Get DelayMBox info
		 Display MBox info

		////////////////////////////////////////////////////////////////////
		Task3 & Task4 control DS3 & DS4.  Delay times for toggling the LEDs
		are determined by Task2 and then posted to a message queue so that
		Task3 and Task4 know how long to delay when toggling their LEDs.
		Delay times start big and get increasingly smaller. Task6 reports
		information about the semaphore used between Task3 and Task4 by
		monitoring S2.  Task7 reports information about the message queue
		by monitoring S3.

													Task3
											+-->	Get Delay from Q
		Task2								|		Pend on Sem34
		Determine DelayTime	 ____	|		Toggle DS3
		Post Delay to Q --->	|____|+		Post to Sem34
		Post Delay to Q --->	|____|+
		Pend on Sem24			  Q	|		Task4
											+-->	Get Delay from Q
  													Pend on Sem34
 													Toggle DS4
 													Post to Sem34
 													Post to Sem24

		Task6							Task7							Task8
		if button S2 is down		if button S3 is down		if button S4 is down
		 Get Sem34 info			 Get Q info					 Get statistics info
		 Display Sem34 info		 Display Q info			 Display statistics info

		////////////////////////////////////////////////////////////////////////
		Task8 reports system information by monitoring S4.  This button must
		be held down for longer than the other three buttons since this task
		only runs once every second.

See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.
*******************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

// If DISPLAYSTATS is defined, the statistics task hook will be called everytime
// the statistics task runs (about once a second).  This function will display
// the number of tasks running, the current percentage of CPU usage, and the
// number of context switches per second.

//  If DISPLAYSTATS is defined, the compiler will generate a "User symbol 'X'
//  overrides library symbol 'Y'" warning for the hook function redefined in
//  this file.  This warning can be ignored.

#define DISPLAYSTATS

// Redefine uC/OS-II configuration constants as necessary
#define OS_MAX_TASKS					 10 // Maximum number of tasks
#define OS_MAX_EVENTS             4  // Maximum number of events (semaphores, queues, mailboxes)
#define OS_TASK_CREATE_EN		    0	 // Disable normal task creation
#define OS_TASK_CREATE_EXT_EN     1  // Enable extended task creation
#define OS_TASK_DEL_EN			    1  // Enable task deletion
#define OS_TASK_STAT_EN			    1  // Enable statistics task creation
#define OS_SEM_EN						 1	 // Enable semaphores
#define OS_SEM_QUERY_EN				 1	 // Enable semaphore querying
#define OS_MBOX_EN					 1  // Enable mailboxes
#define OS_MBOX_POST_EN				 1  // Enable posting messages to mailbox
#define OS_MBOX_QUERY_EN			 1  // Enable mailbox querying
#define OS_Q_EN						 1  // Enable queues
#define OS_Q_ACCEPT_EN				 1  // Enable accepting messages from queue
#define OS_Q_POST_EN					 1  // Enable posting messages to queue
#define OS_Q_POST_FRONT_EN			 1  // Enable posting messages to front of queue
#define OS_Q_QUERY_EN				 1  // Enable queue querying
#define OS_TASK_CHANGE_PRIO_EN    1  // Enable task priority change
#define OS_TASK_SUSPEND_EN			 1  // Enable task suspend and resume
#define OS_TIME_DLY_HMSM_EN		 1  // Enable OSTimeDlyHMSM
#define STACK_CNT_512	          8	 // number of 512 byte stacks (task stacks + initial program stack)
#define STACK_CNT_1K		          4	 // number of 1K stacks

#ifdef  DISPLAYSTATS
#define OS_CPU_HOOKS					 0  // Hook functions are defined in this file
#endif


#use "ucos2.lib"

void starter(void* pdata);
void task0(void* pdata);
void task1(void* pdata);
void task2(void* pdata);
void task3(void* pdata);
void task4(void* pdata);
void task5(void* pdata);
void task6(void* pdata);
void task7(void* pdata);
void task8(void* pdata);
void InitDisplay();
void DispStr(int x, int y, char *s);

OS_EVENT *DelayMBox;
OS_EVENT *DelayQ;
OS_EVENT *Sem34;
OS_EVENT *Sem24;
void* DelayQStorage[5];

#define STARTER 		OS_LOWEST_PRIO - 2
#define TASK0			0
#define TASK1			1
#define TASK2			2
#define TASK3			3
#define TASK4			4
#define TASK5			5
#define TASK6			6
#define TASK7			7
#define TASK8			8

void main()
{
	auto INT8U Error;
	auto INT16U Ticks;

	// Initialize uC/OS-II internal data structures
	OSInit();

   // Create task which will initialize the statistics task, display system statistics,
   // and create other system tasks.
   Error = OSTaskCreateExt(starter, NULL, STARTER, 0, 256, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	// Create mailbox used by task0 and task1 so that the delay time for the second task
	// is the same as that for the first task.  Initialize the mailbox as empty with
	// a NULL pointer.
	DelayMBox = OSMboxCreate((void *)0);

	// Create a queue where task2 can deposit delay times for task3 and task4
	DelayQ = OSQCreate(&DelayQStorage[0], 5);

   // Create a semaphore used for synchronization between task3 and task4
   Sem34 = OSSemCreate(1);

   // Create a semaphore used for synchronization between task2 and task4
   Sem24 = OSSemCreate(0);

   // enable LEDs
	WrPortI(SPCR, &SPCRShadow, 0x84);
	// now write all ones to port A which sets outputs high and LEDs off
 	WrPortI(PADR, &PADRShadow, 0xff);

   // Begin muli-tasking by entering the first ready task
	OSStart();
}

void starter(void* pdata)
{
   auto INT8U Error;

   // Initialize the statistics task
	OSStatInit();

   // Initialize display in STDIO window
   InitDisplay();

   // Disable scheduling while tasks are being created
   OSSchedLock();

   // Create other system tasks.
   Error = OSTaskCreateExt(task0, NULL, TASK0, 0,  512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
   Error = OSTaskCreateExt(task1, NULL, TASK1, 0,  512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
   Error = OSTaskCreateExt(task2, NULL, TASK2, 0,  512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
   Error = OSTaskCreateExt(task3, NULL, TASK3, 0,  512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	Error = OSTaskCreateExt(task4, NULL, TASK4, 0,  512, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	Error = OSTaskCreateExt(task5, NULL, TASK5, 0, 1024, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	Error = OSTaskCreateExt(task6, NULL, TASK6, 0, 1024, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	Error = OSTaskCreateExt(task7, NULL, TASK7, 0, 1024, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	Error = OSTaskCreateExt(task8, NULL, TASK8, 0, 1024, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	// Re-enable scheduling
	OSSchedUnlock();

	// delete this task from the system
	OSTaskDel(OS_PRIO_SELF);
}

void task0(void * pdata)
{
   static INT16U DelayTime;

	while(1)
	{
		for(DelayTime = 10; DelayTime < 500; DelayTime += 10)
		{
			BitWrPortI(PADR, &PADRShadow, 0, 0);  // turn LED DS1 on
			OSTimeDlyHMSM(0, 0, 0, DelayTime);	  // delay
			BitWrPortI(PADR, &PADRShadow, 1, 0);  // turn LED DS1 off
			OSTimeDlyHMSM(0, 0, 0, DelayTime);	  // delay
	      // This task has finished turning its LED on and off, send
   	   // message to task1 telling how long to delay.
			OSMboxPost(DelayMBox, (void *)&DelayTime);
	      // Wait for task1 to finish and resume this task.
			OSTaskSuspend(0);
		}
	}
}

void task1(void * pdata)
{
	auto INT8U Error;
 	auto INT16U *dly;

	while(1)
	{
		// Wait for task0 to finish turning its LED on and off
		dly = (INT16U*)OSMboxPend(DelayMBox, 0, &Error);
		BitWrPortI(PADR, &PADRShadow, 0, 1);  // turn LED DS2 on
		OSTimeDlyHMSM(0, 0, 0, *dly);			  // delay for same amount as task0
		BitWrPortI(PADR, &PADRShadow, 1, 1);  // turn LED DS2 off
		OSTimeDlyHMSM(0, 0, 0, *dly);			  // delay for same amount as task0
      // This task has finished turning its LED on and off, allow
      // task0 to run.
		OSTaskResume(0);
	}
}

void task2(void * pdata)
{
	auto INT16U    i, j;
	auto INT8U     Error;
	static INT16U  DelayTimes[4];

   // this task determines delay times for tasks 3 and 4
	while(1)
	{
      // start with a delay time of 100 milliseconds, decrementing by 5
		for(i = 100;i > 5;i -= 5)
		{
         // post the current delay time to the delay time message queue.
      	for(j = 0; j < 2; j++)
      	{
   			DelayTimes[j] = i;
	   		OSQPost(DelayQ, (void *)&DelayTimes[j]);
			}
         // wait until task4 is complete before posting new delay times.
			OSSemPend(Sem24, 0, &Error);
		}
	}
}

void task3(void * pdata)
{
	auto void* msg;
	auto INT8U Error;

	while(1)
	{
      // Make sure that task4 is not current running.
		OSSemPend(Sem34, 0, &Error);
      // Get current delay value from message queue.
		msg = OSQPend(DelayQ, 0, &Error);
      // turn LED DS3 on
		BitWrPortI(PADR, &PADRShadow, 0, 2);
		OSTimeDlyHMSM(0, 0, 0, *(INT16U*)msg);
      // turn LED DS3 off
		BitWrPortI(PADR, &PADRShadow, 1, 2);
		OSTimeDlyHMSM(0, 0, 0, *(INT16U*)msg);
      // Signal task4 to begin running.
		OSSemPost(Sem34);
	}
}

void task4(void * pdata)
{
	auto void* msg;
	auto INT8U Error;

	while(1)
	{
      // Make sure that task3 is not running
		OSSemPend(Sem34, 0, &Error);
      // Get current delay value from message queue.
		msg = OSQPend(DelayQ, 0, &Error);
      // turn LED DS4 on
		BitWrPortI(PADR, &PADRShadow, 0, 3);
		OSTimeDlyHMSM(0, 0, 0, *(INT16U*)msg);
      // turn LED DS4 off
		BitWrPortI(PADR, &PADRShadow, 1, 3);
		OSTimeDlyHMSM(0, 0, 0, *(INT16U*)msg);
      // signal task3 to begin running
		OSSemPost(Sem34);
      // signal task 2 to post new delay times to message queue.
		OSSemPost(Sem24);
	}
}

void task5(void * pdata)
{
	auto OS_MBOX_DATA MBoxInfo;
	auto INT8U HighestPrio, x, y;
	auto INT8U Error;
   auto char  buf[4];

	while(1)
	{
		// if button S1 is down, display information about mailbox
      if (!BitRdPortI(PBDR, 2))
      {
         // Get information about DelayMBox
         if (OSMboxQuery(DelayMBox, &MBoxInfo) == OS_NO_ERR)
         {
            // Allow button time to be released
				OSTimeDlyHMSM(0, 0, 0, 200);
            // If there is a task waiting at this mailbox, display its priority
				if(MBoxInfo.OSEventGrp)
				{
					y = OSUnMapTbl[MBoxInfo.OSEventGrp];
      			x = OSUnMapTbl[MBoxInfo.OSEventTbl[y]];
      			HighestPrio = (y << 3) + x;
	            sprintf(buf, "%d", HighestPrio);
               DispStr(49, 6, "            ");
	            DispStr(49, 6, buf);
				}
				else
					DispStr(49, 6, "None waiting");
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 10);
	}
}

void task6(void * pdata)
{
	auto INT8U HighestPrio, x, y;
	auto OS_SEM_DATA SemInfo;
	auto INT8U Error;
	auto char  buf[4];

	while(1)
	{
		// if button S2 is down, display information about semaphore
      if (!BitRdPortI(PBDR, 3))
      {
         // Get information about semaphore used between tasks 3 and 4
			if(OSSemQuery(Sem34, &SemInfo) == OS_NO_ERR)
			{
            // Allow button time to be released
      		OSTimeDlyHMSM(0, 0, 0, 200);
            // If there is a task waiting at this mailbox, display its priority
      		if(SemInfo.OSEventGrp != 0)
      		{
      			y = OSUnMapTbl[SemInfo.OSEventGrp];
      			x = OSUnMapTbl[SemInfo.OSEventTbl[y]];
      			HighestPrio = (y << 3) + x;
               sprintf(buf, "%d", HighestPrio);
               DispStr(49, 10, "            ");
               DispStr(49, 10, buf);
				}
				else
				   DispStr(49, 10, "None waiting");
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 10);
	}
}

void task7(void * pdata)
{
	auto OS_Q_DATA QInfo;
	auto INT8U Error;
   auto char  buf[35];
   static void *msg;

	while(1)
	{
		// if button S3 is down, display information about Q
      if (!BitRdPortI(PBDR, 4))
      {
         // Get information about the delay queue used by tasks 2, 3 and 4
			if(OSQQuery(DelayQ, &QInfo) == OS_NO_ERR)
			{
            // Allow button time to be released
      		OSTimeDlyHMSM(0, 0, 0, 200);
            sprintf(buf, "%d", QInfo.OSNMsgs);
            DispStr(30, 14, "    ");
            DispStr(30, 14, buf);
				// Since this is a low priority task, even though OSQQuery may report that there
				// is a message in the queue, by the time the call to OSQAccept is made, that message
				// may have already been taken by a higher priority task.  Likewise, OSQQuery may
				// report that there are no messages in the queue, but by the time OSQAccept is called
				// a message may have arrived.  This should explain discrepancies that may be seen in
				// the output when S3 is pushed.
				if((msg = OSQAccept(DelayQ)) != (void *)0)
				{
					//display content of message in queue
               sprintf(buf, "Message in queue is    : %d\n", *(INT16U*)msg);
               DispStr(5, 15, buf);
					//put message back at the front of the queue
					OSQPostFront(DelayQ, msg);
				}
				else
				{
			      DispStr(5, 15,"                                ");
			   }
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 10);
	}
}

void task8(void * pdata)
{
	auto char buf[5];
	auto INT8U i;
	auto INT8U Error;
	auto OS_STK_DATA StackData;

	while(1)
	{
		// if button S4 is down, display information about stack usage by each of the tasks
      if (!BitRdPortI(PBDR, 5))
      {
         // This is done only to illustrate how to change the priority of a task.  This task's
         // priority is already the lowest (except for the statistics task and the idle task).
         // This call changes the priority to be one level higher than the statistics task.
			OSTaskChangePrio(TASK8, STARTER);

         // Get and display information about stack usage for each of the tasks.
			for(i = 0; i <= TASK7; i++)
			{
				OSTaskStkChk(i, &StackData);
            sprintf(buf, "%4ld", StackData.OSFree);
            DispStr(26, i + 21, buf);
            sprintf(buf, "%4ld", StackData.OSUsed);
            DispStr(49, i + 21, buf);
			}

         // Get and display information about stack usage for this task.
			OSTaskStkChk(OS_PRIO_SELF, &StackData);
         sprintf(buf, "%4ld", StackData.OSFree);
         DispStr(26, i + 21, buf);
         sprintf(buf, "%4ld", StackData.OSUsed);
         DispStr(49, i + 21, buf);

         // Change the priority of this task back to its original priority level.
			OSTaskChangePrio(STARTER, TASK8);
      }
     	OSTimeDlyHMSM(0, 0, 0, 10);
   }
}


// If DISPLAYSTATS is defined, this function will be called everytime the statistics task runs
// (about once a second).  This function will display the number of tasks running, the
// current percentage of CPU usage, and the number of context switches per second.

#ifdef DISPLAYSTATS
nodebug xmem
void OSTaskStatHook()
{
	static char outbuf[128];

  	sprintf(outbuf, "     Tasks running      : %5d\n" \
     	             "     CPU usage          : %5d\n" \
        	          "     Ctx switches/second: %5ld", OSTaskCtr, OSCPUUsage, OSCtxSwCtr);
   DispStr(0, 33, outbuf);
	OSCtxSwCtr = 0;
}
#endif

nodebug xmem
void InitDisplay()
{
	auto int i;
	static char buf[65];

   DispStr(0,  1, "                uC/OS-II JackRabbit Development Board Demo");
   DispStr(0,  2, "                -------- ---------- ----------- ----- ----");
   DispStr(0,  4, "[press button s1]");
	DispStr(0,  5, "-----------------");
	DispStr(0,  6, "     Highest priority task waiting at DelayMBox: xxxx");

	DispStr(0,  8, "[press button s2]");
	DispStr(0,  9, "-----------------");
	DispStr(0, 10, "     Highest priority task waiting on Sem34    : xxxx");

   DispStr(0, 12, "[press button s3]");
 	DispStr(0, 13, "-----------------");
   DispStr(0, 14, "     # of messages in DelayQ: xxxx");

	DispStr(0, 17, "[press button s4]");
 	DispStr(0, 18, "-----------------");
	DispStr(0, 19, "    task name       free stack space      used stack space");
   DispStr(0, 20, "    ---------       ----------------      ----------------");

	for(i = 0; i < 9; i++)
	{
      sprintf(buf, "     task%d                xxxx                   xxxx", i);
		DispStr(0, i + 21, buf);
	}

#ifdef DISPLAYSTATS
   DispStr(0, 31, "System statistics");
 	DispStr(0, 32, "-----------------");
#endif
}

nodebug xmem
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

/*
 *  Putting #xmem here places libraries in XMEM but leaves the applicaiton
 *  to wherever the compiler wants to place them.
 *  THIS REALLY ISN'T RECOMMENDED -- should place it at top of program.
 */
#memmap xmem

