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

      uCOSCoord.c

This program has one master task which randomly assigns starting
coordinates and ending coordinates for 10 worker tasks.  Each worker
task gets its two sets of coordinates, moves to the new location,
and sends a message back to the master task telling how long it took
to get from the starting coordinates to the ending coordinates.

The output consists of three parts:

a) Current location: On this grid, tasks display their current location
   as they are moving from their start to end location.  Since tasks
   can occupy the same coordinate at the same time, it is possible for
   one or more task to write its name over another task.

b) End points: Before it starts moving, each task displays its current
   location and its final destination.  As soon as it is done moving, it
   places an "x" in the done column.

c) Time statistics: Before it starts moving, each task calculates how
   long the entire move should take.  This estimate is placed in the
   "Allowed time" row.  When a task is finished moving, it writes its
   id and total execution time to move into a uC/OS-II controlled memory
   partition, and sends the address of this memory location to the master
   task via a mailbox.  The task then determines if it met its time deadline.
   If the deadline was met, a "Y" is placed into the "Deadline met" row,
   and the amount of extra time is placed into the "Difference" row.  If
   the deadline was not met, an "N" is placed into the "Deadline met" row,
   and the amount of time over the deadline is placed into the "Difference"
   row.  The master task then displays the total execution time for the task
   and places a new set of coordinates into the coordinate queue.


See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.

******************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

// Redefine uC/OS-II configuration constants as necessary
#define OS_MAX_EVENTS                     2  // Maximum number of events (semaphores, queues, mailboxes)
#define OS_MAX_TASKS                     11  // Maximum number of tasks system can create
#define OS_MAX_MEM_PART			           10  // Maximum number of memory partions in system

#define OS_TASK_CREATE_EN		            1	// Enable normal task creation
#define OS_MEM_EN					            1  // Enable memory manager
#define OS_Q_EN									1  // Enable queues
#define OS_Q_POST_EN								1  // Enable pre 2.51 queue post method
#define OS_MBOX_EN								1  // Enable mailboxes
#define OS_MBOX_POST_EN							1  // Enable pre 2.51 mail box post method
#define OS_TICKS_PER_SEC                 32  // Set the number of ticks in one second

#use "ucos2.lib"

// Function prototypes for tasks
void Master(void* pdata);
void Worker(void* pdata);

// Function prototypes for helper functions
void InitDisplay();
void DispStr(int x, int y, char *s);

// Global variables
OS_EVENT *CoordinateQ;
OS_EVENT *DoneMbox;
OS_MEM   *MsgMem;
INT8U     MsgBuf[10][15];
void     *QStorage[10];
const char     *const tasknames[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};

// initial data consists of task id, x and y coordinates
int	   InitialData[10][3];

void main()
{
	auto INT8U Error;
	auto INT16U Ticks;

	// Initialize uC/OS-II internal data structures
	OSInit();

	InitDisplay();

   // Create Master task
   Error = OSTaskCreate(Master, NULL, 512, 0);

   // Create queue for storing coordinates
	CoordinateQ = OSQCreate(&QStorage[0], 10);

	// Create and initialize uC/OS-II controlled memory partition.
	MsgMem = OSMemCreate(&MsgBuf[0][0], 10, 15, &Error);

   // Create mailbox tasks use to let Master know they are done moving
   DoneMbox = OSMboxCreate((void *)0);

   // Begin muli-tasking by entering the first ready task
	OSStart();
}

void Master(void* pdata)
{
	/* Local vars can be "static" since only one instance of Master() ever started. */
	static int i, x, y, id;
	static INT8U err, *msg;

   static int Coordinates[10][2];

	for(i = 0; i < 10; i++)
	{
		InitialData[i][0] = i;
      InitialData[i][1] = (int)(rand() * 10);
      InitialData[i][2] = (int)(rand() * 10);
      OSTaskCreate(Worker, (void *)&InitialData[i][0], 512, i + 1);

   }
	// Get and post new coordinates for the task
	for(i = 0; i < 10; i++)
	{
   	Coordinates[i][0] = (int)(rand() * 10);
   	Coordinates[i][1] = (int)(rand() * 10);
  		OSQPost(CoordinateQ, (void *)&Coordinates[i][0]);
   }

	while(1)
	{
		// wait for a task to finish moving, display total time task took to move
		msg = (char *)OSMboxPend(DoneMbox, 0, &err);
      id = msg[0] - 48;
///		DispStr((id * 6) + 19, 32, "     ");
		DispStr((id * 6) + 19, 32, &msg[1]);

		// Get and post new coordinates for the task
  	   Coordinates[id][0] = (int)(rand() * 10);
     	Coordinates[id][1] = (int)(rand() * 10);
  		OSQPost(CoordinateQ, (void *)&Coordinates[id][0]);
	}
}

xmem void Worker(void *pdata)
{
	auto int id, x, y, *msg;
	auto INT8U err;
	auto char buf[10];
	auto char *MemChunk;
	auto unsigned long exectime, allowed;

	// Get memory chunk for storing execution time
	MemChunk = (char *)OSMemGet(MsgMem, &err);

   // Get task id, and initial x and y coordinates
	msg = (int *)pdata;
	id  = msg[0];
	x   = msg[1];
	y   = msg[2];

	while(1)
	{
	   // Display initial coordinates
	   sprintf(buf, " (%d,%d)", x, y);
		DispStr(58, (id * 2) + 6, buf);

      // Display marker on grid
	   sprintf(buf, "t%d", id);
		DispStr((x * 5) + 5, (y * 2) + 6, buf);

		// unmark  done
		DispStr(76, (id * 2) + 6, " ");

		// Get coordinates from queue
      msg = (int *)OSQPend(CoordinateQ,  0, &err);

      // Display new coordinates
	   sprintf(buf, " (%d,%d)", msg[0], msg[1]);
		DispStr(66, (id * 2) + 6, buf);

		// Determine allowed time to move
		// allowed time = (# of moves * 1000)
		allowed = ((((long)abs(msg[0] - x) + 1) + ((long)abs(msg[1] - y))) * 1000);
		sprintf(buf, "%5ld", allowed);

    	// Display allowed time to move
///		DispStr((id * 6) + 19, 30, "     ");
		DispStr((id * 6) + 19, 30, buf);

		// get start time
      exectime = MS_TIMER;

		// move in x direction
	   if(msg[0] > x)
	   {
	      while (x <= msg[0])
	      {
            // write task name into buf and display in coordinate grid.
	      	sprintf(buf, "t%d", id);
	      	DispStr((x * 5) + 5, (y * 2) + 6, buf);
				OSTimeDly(5);
            // erase task name from current location in coordinate grid.
				DispStr((x * 5) + 5, (y * 2) + 6, "  ");
	      	x++;
	      }
	      x--;
	   }
	   else
	   {
	   	while (x >= msg[0])
	   	{
            // write task name into buf and display in coordinate grid.
	   		sprintf(buf, "t%d", id);
	   		DispStr((x * 5) + 5, (y * 2) + 6, buf);
				OSTimeDly(5);
            // erase task name from current location in coordinate grid.
				DispStr((x * 5) + 5, (y * 2) + 6, "  ");
	   		x--;
	   	}
	   	x++;
	   }
		// move in y direction
	   if(msg[1] > y)
	   {
	      while (y < msg[1])
	      {
	      	y++;
            // write task name into buf and display in coordinate grid.
	      	sprintf(buf, "t%d", id);
	      	DispStr((x * 5) + 5, (y * 2) + 6, buf);
				OSTimeDly(5);
            // erase task name from current location in coordinate grid.
				DispStr((x * 5) + 5, (y * 2) + 6, "  ");
	      }
			// write task name into buf and display in coordinate grid.
      	sprintf(buf, "t%d", id);
      	DispStr((x * 5) + 5, (y * 2) + 6, buf);
	   }
	   else
	   {
	   	while (y > msg[1])
	   	{
	   		y--;
            // write task name into buf and display in coordinate grid.
	   		sprintf(buf, "t%d", id);
	   		DispStr((x * 5) + 5, (y * 2) + 6, buf);
				OSTimeDly(5);
            // erase task name from current location in coordinate grid.
				DispStr((x * 5) + 5, (y * 2) + 6, "  ");
	   	}
			// write task name into buf and display in coordinate grid.
   		sprintf(buf, "t%d", id);
   		DispStr((x * 5) + 5, (y * 2) + 6, buf);
	   }

		// determine total execution time
      exectime = MS_TIMER - exectime;

		// mark as done
		DispStr(76, (id * 2) + 6, "x");

      // remember new location
		x = msg[0];
		y = msg[1];

      // write id and execution time into memory chunk
      sprintf(MemChunk, "%d%5ld", id, exectime);

      // mark whether or not deadline met
      if (exectime > allowed)
      {
      	DispStr(((id * 6) + 2) + 19, 34, "N");
			sprintf(buf, "%5ld", (long)(exectime - allowed));
///      	DispStr((id * 6) + 19, 36, "     ");
      	DispStr((id * 6) + 19, 36, buf);
      	DispStr((id * 6) + 19, 38, "     ");
      }
		else
		{
      	DispStr(((id * 6) + 2) + 19, 34, "Y");
			sprintf(buf, "%5ld", (long)(allowed - exectime));
///      	DispStr((id * 6) + 19, 38, "     ");
      	DispStr((id * 6) + 19, 38, buf);
      	DispStr((id * 6) + 19, 36, "     ");
      }

      // post address of MemChunk to done mailbox
      OSMboxPost(DoneMbox, MemChunk);
	}
}

nodebug void
InitDisplay()
{
	DispStr(0,  1, "                                  uC/OS-II Demo                                 ");
	DispStr(0,  2, "                                 ---------------                                ");

	DispStr(0,  4, "    0    1    2    3    4    5    6    7    8    9         start    end   done  ");
	DispStr(0,  5, "   ---------------------------------------------------   -----------------------");
	DispStr(0,  6, "0  |    |    |    |    |    |    |    |    |    |    | t0|       |       |     |");
	DispStr(0,  7, "   ---------------------------------------------------   -----------------------");
	DispStr(0,  8, "1  |    |    |    |    |    |    |    |    |    |    | t1|       |       |     |");
	DispStr(0,  9, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 10, "2  |    |    |    |    |    |    |    |    |    |    | t2|       |       |     |");
	DispStr(0, 11, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 12, "3  |    |    |    |    |    |    |    |    |    |    | t3|       |       |     |");
	DispStr(0, 13, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 14, "4  |    |    |    |    |    |    |    |    |    |    | t4|       |       |     |");
	DispStr(0, 15, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 16, "5  |    |    |    |    |    |    |    |    |    |    | t5|       |       |     |");
	DispStr(0, 17, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 18, "6  |    |    |    |    |    |    |    |    |    |    | t6|       |       |     |");
	DispStr(0, 19, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 20, "7  |    |    |    |    |    |    |    |    |    |    | t7|       |       |     |");
	DispStr(0, 21, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 22, "8  |    |    |    |    |    |    |    |    |    |    | t8|       |       |     |");
	DispStr(0, 23, "   ---------------------------------------------------   -----------------------");
	DispStr(0, 24, "9  |    |    |    |    |    |    |    |    |    |    | t9|       |       |     |");
	DispStr(0, 25, "   ---------------------------------------------------   -----------------------");
   DispStr(0, 26, "     (a) Current Location                                  (b) End points       ");

	DispStr(0, 28, "                    t0    t1    t2    t3    t4    t5    t6    t7    t8    t9     ");
	DispStr(0, 29, "                  -------------------------------------------------------------  ");
	DispStr(0, 30, "Allowed time (ms) |     |     |     |     |     |     |     |     |     |     |  ");
	DispStr(0, 31, "                  -------------------------------------------------------------  ");
	DispStr(0, 32, "Elapsed time (ms) |     |     |     |     |     |     |     |     |     |     |  ");
	DispStr(0, 33, "                  -------------------------------------------------------------  ");
	DispStr(0, 34, "Deadline met      |     |     |     |     |     |     |     |     |     |     |  ");
	DispStr(0, 35, "                  -------------------------------------------------------------  ");
	DispStr(0, 36, "Difference  (+ms) |     |     |     |     |     |     |     |     |     |     |  ");
	DispStr(0, 37, "                  -------------------------------------------------------------  ");
   DispStr(0, 38, "            (-ms) |     |     |     |     |     |     |     |     |     |     |  ");
	DispStr(0, 39, "                  -------------------------------------------------------------  ");
	DispStr(0, 40, "                    (c) Time Statistics                                          ");
}

nodebug void
DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

#memmap xmem