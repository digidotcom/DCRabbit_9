/* START PROGRAM DESCRIPTION **********************************************


DESCRIPTION:	Exercise and display RabbitSys functionality, including
timer events, virtual watchdogs, error logging, Console interface functions,

END DESCRIPTION **********************************************************/
#define STDIO_DISABLE_FLOATS

#use "DCRTCP.LIB"

#define CRLFstr "\r\n"
#define MYBAUD 115200
#define FINBUFSIZE 255
#define FOUTBUFSIZE 63

#define RES_X 0
#define RES_Y 14
#define INP_X 19
#define INP_Y 12

#define ESCAPE 27

#define SMTP_IP "209.233.102.3"

#define TCPSOCKETS 3
#define UDPSOCKETS 2
#undef TCP_BUF_SIZE
#define TCP_BUF_SIZE 0x1000

typedef struct menuEntry_t
{
	char key;				//input character to select this item
	int item;				//item number.  Must be unique unless a submenu
	char *menuText;		//menu text for this item
	int (*menuFunc)();	//function to execute for this item
	int menuLink;			//if not zero what menu to display after
								//  menuFunc() executed.
} menuEntry ;

void quit(void);

const menuEntry mainMenu[] = {			//this is by default Menu 1.
{ '1', 0, "Console Functions ->", 	NULL, 2 },
{ '2', 0, "Events            ->",  	NULL, 4 },
{ '3', 0, "System Functions  ->",	NULL, 5 },
{ '4', 0, "Monitor Functions ->",	NULL, 7 },
{ '5', 0, "Network Functions ->",   NULL, 6 },
{ '6', 9, "Set Virt. Watchdg...", 	NULL, 0 },
{ 'm', 0, "more              ->",	NULL, 3 },
{ 'q',-1, "quit",							quit, 0 },
{  0 , 0, NULL, NULL, 0 }
};

const menuEntry ConsoleMenu[] = {		//this is Menu 2.
{ '1', 1, "Change Con Ser Port ", 	NULL, 0 },
{ '2', 2, "Disable Con Ser Port",	_sys_con_disable_serial, 0 },
{ '3',11, "Set tick interval",		NULL, 0 },
{ '4',12, "Set RTE behavior",			NULL, 0 },
{ ' ', 0,  "", 							NULL, 0 },
{ 'b', 0, "Back", 						NULL, 1 },
{  0 , 0, NULL, NULL, 0 }
};

void toggleEventDisplay(void);
void clearTheScreen(void);
void toggleClockDisplay(void);
void resetStats(void);
void showStats(void);

const menuEntry more1Menu[] = {       	//this is Menu 3.
{ 'c', 0,  "Clear Screen", 			clearTheScreen, 0 },
{ 'e', 0,  "Toggle Event Display", 	toggleEventDisplay, 0 },
{ 'k', 0,  "Toggle Clock Display", 	toggleClockDisplay, 0 },
{ 'r', 0,  "Reset Event Stats   ",	resetStats, 0 },
{ 's', 0,  "Toggle Stats Display",	showStats,  0 },
{ ' ', 0,  "", 							NULL, 0 },
{ 'b', 0,  "Back", 						NULL, 1 },
{  0 , 0, NULL, NULL, 0 }
};

const menuEntry EventMenu[] = {			//this is Menu 4.
{ 'a', 6,  "Add Event", 	NULL, 0 },
{ 'd', 4,  "Delete Event", NULL, 0 },
{ 'p',13,  "Add Polled Event",NULL, 0 },
{ 'q',10,  "Query an Event",  NULL, 0 },
{ 't',16,  "Trigger an Event",NULL, 0 },
{ ' ', 0,  "",					NULL, 0 },
{ '1',17,  "Assign ISR",	NULL, 0 },
{ '2',18,  "Trigger ISR",	NULL, 0 },
{ 'b', 0,  "Back", 			NULL, 1 },
{  0 , 0,  NULL, 				NULL, 0 }
};

void setClock(void);
void saveNetParms(void);
void clockDoublerTest (void);
void openAllDevices(void);
void assignAllIntVectors(void);

const menuEntry SystemFunctionsMenu[] = {			//This is Menu 5.
{ 'a', 0,  "Assign int vectors",	assignAllIntVectors, 0 },
{ 'c', 0,  "Set Clock...",	 		setClock, 0 },
{ 'd', 0,  "Clock Doubler Test",	clockDoublerTest, 0 },
{ 's', 0,  "Save System Parms",	saveNetParms, 0 },
{ 'i',15,  "Set Interrupt Level",NULL, 0 },
{ 'w',20,  "Set Sec. WD time",   NULL, 0 },
{ ' ', 0,  "",							NULL, 0 },
{ 'b', 0,  "Back", 					NULL, 1 },
{  0 , 0,  NULL, 						NULL, 0 }
};

const menuEntry NetFuncsMenu[] = {			//This is Menu 6.
{ '1', 7, "Set Email Data...", 	NULL, 0 },
{ '3', 3, "Read a web page...", 	NULL, 0 },
{ ' ', 0, "",							NULL, 0 },
{ 'b', 0,  "Back", 					NULL, 1 },
{  0 , 0,  NULL, 						NULL, 0 }
};

void getLogDefinitions(void);
void getWatchContents(void);

const menuEntry MonitorMenu[] = {			//This is Menu 7.
{ '1', 19,"Get Log Contents... ", 	NULL, 0 },
{ '2', 0, "Get Watch Contents...", 	getWatchContents, 0 },
{ '3', 0, "Get Log Definitions...",	getLogDefinitions, 0 },
{ '4', 5, "Log an Error...     ",	NULL, 0 },
{ ' ', 0, "",								NULL, 0 },
{ 'b', 0,  "Back", 						NULL, 1 },
{  0 , 0,  NULL, 							NULL, 0 }
};

menuEntry * const menuSystem[] =
{
	NULL										// 0
	,mainMenu                        // 1
	,ConsoleMenu                     // 2
	,more1Menu                       // 3
	,EventMenu                       // 4
	,SystemFunctionsMenu             // 5
	,NetFuncsMenu                    // 6
   ,MonitorMenu							// 7
};


/*--------------------------------------------------
 When events are defined they may be given pointers
 to static data to store with the event callback
 procedure address and other event data.  This data
 address is stored in the data structure that is
 passed to the callback procedure, and may be accessed
 in any way you think is appropriate.  An array
 element "evntStrs[n]" contains the data that is
 specific to a particular event defined in this
 program.
 --------------------------------------------------*/
typedef struct
{
	_sys_event_handle seh;
   char eventString[24];
} eventStruct;

eventStruct evntStrs[20];
int  ecnt;

/*--------------------------------------------------
 Statistics variables. Difference refers to the difference
 between the actual time the callback function executes and
 the scheduled time.
 --------------------------------------------------*/
long diff;           // sum of all differences
long maxdiff;        // maximum difference
long diffcnt;        // how many events have occurred

// if this is non-zero the clock is displayed.
int clockDisplay;

// if non-zero, events will be displayed.
int edisplay;

// if non-zero, event statistics will be displayed.
int showstats;

char bigbuf[256];

// UI stuff.
int linesToErase;
menuEntry *mp, *cm;

/*--------------------------------------------------
 The event stack allows my events, which are
 psuedo-interrupt routines, to flag the main loop
 that there is data to display on the screen, and
 where to display it.
 --------------------------------------------------*/
typedef struct
{
	int	x;
	int	y;
	char *s;
} eventStack_data_t;

#define EVNT_MAX 10
struct
{
	eventStack_data_t eventdata[EVNT_MAX];
	int tos;
} eventStack;


/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
void gotoxy (unsigned int x, unsigned int y)
{
   if ( (y>29) || (x>79) )
   {
   	printf("! x = %d, y=%d ",x,y);
   	while (kbhit()==0) ;
   }
	printf("\x1B=%c%c",(char)x+' ',(char)y+' ');
} //gotoxy()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
void cls ()
{
   printf ( " \x1Bt" );            	// Space Opens Window
} //cls()

nodebug
void quit ()
{
	exit(0);
}

/* START FUNCTION DESCRIPTION ********************************************
ahtoi                                                  <.LIB>

SYNTAX:			int ahtoi ( char * str );

DESCRIPTION:	convert the string base-16 UNSIGNED number into an integer.

PARAMETER:		string representation of the number in hex.

RETURN VALUE:	integer value of "str".

END DESCRIPTION **********************************************************/
nodebug
int ahtoi (char *s)
{
	int result;
	char c;

	if (!s)
		return 0;
	if ( (strlen(s) > 4) || (strlen(s) == 0) )
		return 0;
	result = 0;
	while (*s)
	{
		c = toupper(*s++);
		if ( (c > 'F') || (c < '0') )
			return 0;
		result <<= 4;
		result += (c>'9') ? c-'A'+10 : c-'0';
	}
	return result;
} //ahtoi()

/*--------------------------------------------------
 ClearResultArea()
 --------------------------------------------------*/
nodebug
int clearResultArea (int numLines)
{
	char line[80];

	memset(line, ' ', 79);
	line[79]=0;
	gotoxy(RES_X, RES_Y);
	do
	{
		printf("%s\n",line);
	} while (numLines--);
	return 0;
} //clearResultArea()

/*--------------------------------------------------
 beASerialConsole()

 get all input from STDIO and send it to the RabbitSys
 Console, and send all RabbitSys Console output to
 STDOUT.

 Setup: connect serial ports E and F together
 	TXE -> RXF
 	RXE -> TXF
 	GND -> GND

 This will not do anything if you are debugging using
 TCP/IP, since Dynamic C is actually logged in to
 RabbitSys.
 --------------------------------------------------*/
nodebug
firsttime int beASerialConsole (CoData *data)
{
	char c;

   if (data->firsttimeflag)
   {
		if (serFopen(MYBAUD)==0)
		{
			printf("cannot open 'F' at 115200");
		}

		printf("Press ESCape to exit serial terminal mode\n\n");
		data->firsttimeflag = 0;
	}
   while (1)
   {
   	if (kbhit())
   	{
	      c = getchar();
	      if (c == ESCAPE)             //user pressed escape to exit
	         break;
	      if (serFputc(c) != 1)
	      {
	         printf("serFputc() failed\n");
	         break;
	      }
	      if (c == '\r')
	      	serFputc('\n');
	   }
      _sys_tick(1);
      while ( serFpeek() != -1 )
      {
      	c = serFgetc();
         putchar(c);
      }
      return 0;
   };
	cls();
	serFclose();
	return 1;
} //beASerialConsole()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug void eventStackPush (int x, int y, char *s)
{
   eventStack_data_t *ed;

	if ( eventStack.tos+1 != EVNT_MAX )
	{
		ed = &eventStack.eventdata[++eventStack.tos];
		ed->x = x;
		ed->y = y;
		ed->s = s;
	}
} //eventStackPush()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
eventStack_data_t *eventStackPop (void)
{
	if (eventStack.tos != -1)
	{
		return &eventStack.eventdata[eventStack.tos--];
	}
	return NULL;
}//eventStackPop()

void getLogDefinitions (void)
{
	char buffer[128];

   _sys_mon_get_log_def(buffer);
   gotoxy(RES_X,RES_Y);
   printf(buffer);
   linesToErase=2;
}

void getWatchContents (void)
{
	int rslt;
   char *p;

   gotoxy(RES_X,RES_Y);
   for (rslt = 0; !_sys_mon_get_watch(bigbuf); ++rslt)
   {
   	p = strtok(bigbuf,"\n");
   	while ( p )
      {
      	gotoxy(RES_X,RES_Y+(rslt++));
      	printf(p);
         p = strtok(NULL,"\n");
      }
   }
   if (strlen(bigbuf))
   {
   	p = strtok(bigbuf,"\n");
   	while (p)
      {
      	gotoxy(RES_X,RES_Y+(rslt++));
      	printf(p);
         p = strtok(NULL,"\n");
      }
      ++rslt;
   }
   printf("\nWatch Contents Done.");
   linesToErase = rslt;
}

/* START FUNCTION DESCRIPTION ********************************************
calc_diff                                                  <.LIB>

SYNTAX:  		void calc_diff (_sys_event_handle *seh)

DESCRIPTION:   calculates latency and number of times it has
executed for average latency. Tracks maximum latency time.

PARAMETER:		system event handle pointer

RETURN VALUE:	none.

END DESCRIPTION **********************************************************/
nodebug
void calc_diff (_sys_event_data_t *sd)
{
	long t;

   t = MS_TIMER - sd->timer.timeout;
	diff += t;
	if (t>maxdiff)
		maxdiff = t;
	++diffcnt;
}

/**START FUNCTION DESCRIPTION ********************************************
SYNTAX:			void timerEventProc (_sys_event_handle *seh, _sys_event_data_t *data)

DESCRIPTION:	timer event callback procedure. Pushes data on the
event stack and exits.  Timer event procedures need to be kept
short if the RabbitSys tick timer (see Console Setup command) is
non-zero because they are running in an interrupt context.

PARAMETER:		system event handle pointer
PARAMETER2:		address of data for this function

RETURN VALUE:	none.

END DESCRIPTION **********************************************************/
nodebug
void timerEventProc (_sys_event_handle *seh, _sys_event_data_t *data)
{
	char s[16];

	calc_diff(data);
   if (edisplay)
   {
   	// the casting of "data" is hideous, but if you only have to
   	// do it once...
   	//
   	// NOTE: the event data type ("...->timer...") is critical
   	//       to correctly accessing your data.
   	eventStackPush(0,24,((eventStruct *)(data->timer.data))->eventString);
   }
} //timerEventProc()



/**START FUNCTION DESCRIPTION ********************************************
SYNTAX:			void alertEventProc (_sys_event_handle *seh, _sys_event_data_t *data)

DESCRIPTION:	alert event callback procedure. An alert event may or may
not be called in an interrupt context. If an alert requires you to take
some critical action you probably should do it here and now, rather than
risk not getting to it in your main control loop. In this program alerts
mean nothing and so I just push the string addres on my event stack.

PARAMETER:		system event handle pointer
PARAMETER2:		address of data for this function

RETURN VALUE:	none.

END DESCRIPTION **********************************************************/
debug
void alertEventProc (_sys_event_handle *seh, _sys_event_data_t *data)
{
   // NOTE: the event data type ("...->startup...") is critical
   //       to correctly accessing your data.
	eventStackPush(0,26,
	 	((eventStruct *)(data->startup.data))->eventString);
} //alertEventProc()


/**START FUNCTION DESCRIPTION ********************************************
SYNTAX:			void eClockProc (_sys_event_handle *seh, _sys_event_data_t *data)

DESCRIPTION:	event callback procedure to display the current time.  This
procedure is probably called in an interrupt context so I push the event
data (a string address) on my event stack and handle it later.

PARAMETER1:		system event handle address
PARAMETER2:		pointer to data for this function

RETURN VALUE:	none.

END DESCRIPTION **********************************************************/
nodebug
void eClockProc (_sys_event_handle *seh, _sys_event_data_t *data)
{
	struct tm		rtc;					// time struct
	char hrs[4], min[4], sec[4];

	calc_diff(data);
   if (!clockDisplay)
   	return;

	mktm(&rtc, SEC_TIMER);
	itoa(rtc.tm_hour, hrs);
	itoa(rtc.tm_min, min);
	itoa(rtc.tm_sec, sec);
	if (!sec[1])
	{
		sec[1] = sec[0];
		sec[0] = '0';
		sec[2] = 0;
	}
	if (!min[1])
	{
		min[1] = min[0];
		min[0] = '0';
		min[2] = 0;
	}
   // NOTE: the event data type ("...->timer...") is critical
   //       to correctly accessing your data.
	sprintf(((eventStruct *)(data->timer.data))->eventString,
   	"%s:%s:%s",hrs,min,sec);
	eventStackPush(64,0,((eventStruct *)(data->timer.data))->eventString);
}//eClockProc()

/**START FUNCTION DESCRIPTION ********************************************
SYNTAX:			void SetupInitialTimers (void)

DESCRIPTION:	register timer events with RabbitSys. Six timer events are
registered, including the clock display event and a timer with Exponential
Back-off set.  All the registered timer events are recurring events. Alert
and Shutdown events may not be recurring.

PARAMETER:		none.

RETURN VALUE:	none.

END DESCRIPTION **********************************************************/
nodebug
void SetupInitialTimers (void)
{
	_sys_event_data_t edata;
	_sys_event_handle seh;
	int rslt;

	edata.shutdown.sflags = 0;
   strcpy(evntStrs[ecnt].eventString,"alert #1");
   edata.shutdown.stack = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_ALERT,alertEventProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

   edata.timer.tflags = _SYS_EVENT_RECUR;//_SYS_EVENT_SYSTEM | _SYS_EVENT_RECUR;
   edata.timer.interval = 1000;
   strcpy(evntStrs[ecnt].eventString,"1   second timer");
   edata.timer.data = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_TIMER,eClockProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

   edata.timer.tflags = _SYS_EVENT_RECUR;
   edata.timer.interval = 3000;
   strcpy(evntStrs[ecnt].eventString,"3   second timer");
   edata.timer.data = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_TIMER,timerEventProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

   strcpy(evntStrs[ecnt].eventString,"shutdown #1");
   edata.shutdown.stack = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_SHUTDOWN,alertEventProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

   edata.timer.tflags = _SYS_EVENT_RECUR;
   edata.timer.interval = 5000;
   strcpy(evntStrs[ecnt].eventString,"5   second timer");
   edata.timer.data = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_TIMER,timerEventProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

   //edata.timer.tflags = _SYS_EVENT_RECUR;
   edata.timer.interval = 7000;
   strcpy(evntStrs[ecnt].eventString,"7   second timer");
   edata.timer.data = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_TIMER,timerEventProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

   edata.timer.tflags = _SYS_EVENT_RECUR | _SYS_EVENT_EBO;
   edata.timer.interval = 50;
   strcpy(evntStrs[ecnt].eventString,"50  ms EBO timer");
   edata.timer.data = &evntStrs[ecnt];
	rslt = _sys_add_event(_SYS_EVENT_TIMER,timerEventProc,&evntStrs[ecnt].seh,&edata);
	++ecnt;

} //SetupInitialTimers()

/*-----------------1/18/2005 1:44PM-----------------
 SET THE CLOCK
 This is a blocking user-input function.  Events may
 pile up in the stack and be lost.
 --------------------------------------------------*/
nodebug
void setClock (void)
{
	char s[64];
	char *p, *endptr;
	unsigned int month, day, year, hour, minute, second;
	struct tm rtc;					// time struct
	char rslt;

   clockDisplay = 0;
	printf("Enter the correct date/time as:  mm/dd/yy hh:mm:ss\n");
	printf("or just press Enter to leave the date/time unchanged:\n\n");

	gets(s);
	p = s;

   month = (unsigned int)strtod(p, &endptr);
   p = endptr + 1;
   day = (unsigned int)strtod(p, &endptr);
   p = endptr + 1;
   year = (unsigned int)strtod(p, &endptr) + 100;
   p = endptr + 1;
   hour = (unsigned int)strtod(p, &endptr);
   p = endptr + 1;
   minute = (unsigned int)strtod(p, &endptr);
   p = endptr + 1;
   second = (unsigned int)strtod(p, &endptr);
	// change the date/time via tm_wr
	rtc.tm_sec  = second;	   // 0-59
	rtc.tm_min  = minute;		// 0-59
	rtc.tm_hour = hour;			// 0-23
	rtc.tm_mday = day;			// 1-31
	rtc.tm_mon  = month;			// 1-12
	rtc.tm_year = year;			// 80-147, add 1900 to get year
										//		(i.e. 99 -> 1999)

	tm_wr(&rtc);					// set clock
	printf("RTC changed!\n\n");
   clockDisplay = 1;
   linesToErase = 5;
} //setClock()

/* START FUNCTION DESCRIPTION ********************************************
my_gets                                                  <.LIB>

SYNTAX:			int my_gets(char * prompt, char *buf, int x, int y);

DESCRIPTION:	This "firsttime" function will check for input from STDIO,
returning zero (0) until the user presses "return".  Only call this
function from inside a "waitfor()" statement.

The prompt will be displayed on the first call, and the user-entered
characters will be echoed after the prompt.  If the user presses ESCape
the escape character will be placed at buf[0] and the string will
be one character long.

PARAMETER1:		prompt to display. The prompt is limited to one line only.
PARAMETER2:		where to place user input.
PARAMETER3:		what column to print the prompt on.
PARAMETER4:		what row to print the prompt on.

RETURN VALUE:	0		user still entering data
					1		user pressed return or ESCape.

END DESCRIPTION **********************************************************/
nodebug
firsttime int my_gets (CoData *data, char * prompt, char *buf, int x, int y)
{
	int i, offset,rslt;

   if (data->firsttimeflag)			//if first time through, display
   {                                //  the prompt and set the
   	if (prompt)                   //  buffer to zero length.
   	{
   		gotoxy(x,y);
   		printf(prompt);
   	}
   	buf[0] = 0;
		data->firsttimeflag = 0;
	}

	i = strlen(buf);
	offset = strlen(prompt);    			//column to echo user characters
   if ( kbhit() )
   {
	   rslt=getchar();
	   if ((rslt == '\b') && i)			//backspace?
	   {
	   	gotoxy(x+i+offset,y);
	   	printf("\b \b");          //clear the previous character
	   	--i;                          //  being displayed, then delete
	   	buf[i] = 0;                   //  it from the buffer.
	   }
	   else if (rslt == ESCAPE)			//escape
	   {
	   	buf[0] = rslt;
	   	buf[1] = 0;
	   	return 1;
	   }
	   else if (isprint(rslt))          //a printable character has been
	   {                                //entered, append it and display it.
	   	buf[i] = rslt;
	   	buf[i+1] = 0;
	   	gotoxy(x+offset,y);
	   	printf(buf);
	   }
	   if ( rslt != '\r' )
	   {
	   	return 0;
	   }
   	buf[i] = 0;
   	printf("\n");
   }
   else
   	return 0;
   return 1;
}//my_gets()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
void showScreen (void)
{
	char cmdStr[24];
  	struct tm		rtc;					// time struct

	mktm(&rtc, SEC_TIMER);
	cls();
	sprintf(cmdStr, "%04.4x", _sys_version());
	cmdStr[5] = 0;
	cmdStr[4] = cmdStr[3];
	cmdStr[3] = cmdStr[2];
	cmdStr[2] = '.';
	gotoxy(0,0);
	printf("RabbitSys Version:%s",cmdStr);
	gotoxy(53,0);
	printf("%d/%02d/%02d",rtc.tm_year+1900,rtc.tm_mon,rtc.tm_mday);
	gotoxy(0,22);
	printf("Periodic Events\r\n------------------");
	gotoxy(35,0);
   inet_ntoa(cmdStr,gethostid());
   printf("IP:%s",cmdStr);
} //showScreen()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
void clearMenu (void)
{
	int rc;
	char cmdStr[48];

	gotoxy(0,2);
	memset(cmdStr,' ',47);
	cmdStr[47] = 0;
	for ( rc=0; rc<10; ++rc)
	{
		printf("%s\n", cmdStr);
	}
}//clearMenu()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
void showMenu (menuEntry *mp)
{
	char cmdStr[48];

	clearMenu();
	gotoxy(0,2);
	while ( mp->key )
	{
		strcpy(cmdStr,"  > %s\n");
		cmdStr[1] = mp->key;
		if ( mp->key == ' ' )
			cmdStr[2] = mp->key;
		printf(cmdStr, mp->menuText);
		++mp;
	}
} //showMenu

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
menuEntry *findMenuItem (menuEntry *mp, int item)
{
	while ( mp->key )
	{
		if ( mp->key == item )
		{
			break;
		}
		++mp;
	}
	return mp;
} //findMenuItem()

/*--------------------------------------------------
 --------------------------------------------------*/
nodebug
void setupRSNetwork (void)
{
	int rslt;
	char cmdStr[64];

	if ( ifconfig(IF_DEFAULT, IFS_DOWN,
		IFS_DHCP, 0,
		IFS_IPADDR, aton("10.10.6.126"),
		IFS_NETMASK,aton("255.255.255.0"),
		IFS_ROUTER_SET, aton("10.10.6.1"),
		IFS_NAMESERVER_SET, aton("10.10.6.1"),
		IFS_UP, IFS_END) )
	{
		printf("ifconfig() failed\n");
	}

	if ( _sys_mon_set_email("209.233.102.3", "you@yourplace.net") )
	{
		printf("set_email() failed\n");
	}
  	_sys_UPISaveData();
}

int rst10Count, rst38Count;

/* START FUNCTION DESCRIPTION ********************************************
isr_RST10

SYNTAX:			void isr_RST10(void)

DESCRIPTION:	increment a counter just to demonstrate the registerisr()
function.  Note that this is called through the internal interrupt table
but is NOT REALLY an interrupt, as IP is not changed.  The "interrupt"
attribute is not used for RST 0xnn vectors.

PARAMETER:		none.

RETURN VALUE:	none.

END DESCRIPTION **********************************************************/
nodebug
root void isr_RST10 (void)
{
	++rst10Count;
}

nodebug
root void isr_RST38 (void)
{
	++rst38Count;
}

/*--------------------------------------------------
 CLOCK DOUBLER TESTS
 --------------------------------------------------*/
nodebug
void clockDoublerTest (void)
{
	long tc;
	unsigned long rslt;

	gotoxy(RES_X, RES_Y);
	printf("Counting for 1 sec., doubler ON:  ");
	clockDoublerOn();
	tc = MS_TIMER;
	rslt = 0;
	while ( (MS_TIMER - tc) < 1000 )
	   ++rslt;
	printf("%ul\n",rslt);

	printf("Counting for 1 sec., doubler OFF: ");
	clockDoublerOff();
	tc = MS_TIMER;
	rslt = 0;
	while ( (MS_TIMER - tc) < 1000 )
	   ++rslt;
	printf("%ul\n",rslt);
	clockDoublerOn();
	printf("Clock Doubler ON.");
	linesToErase = 3;
}//clockDoublerTest()

/*--------------------------------------------------
 ASSIGN INTERRUPT VECTORS TEST

 Assign internal interrupt vectors 0-15 to isr_RST10.
 --------------------------------------------------*/
//nodebug
void assignAllIntVectors (void)
{
	int i;
   void *v;

   gotoxy(RES_X,RES_Y);
   printf("Internal Vectors:\n0123456789ABCDEF0123456789ABCDEF");
   for (i=0;i<0x20;++i)
   {
   	// skip holes in vector table
   	if ( ((i>0x0F) && (i<0x17)) ||
      	(i>0x1D) )
      {
	      gotoxy(RES_X+i, RES_Y+2);
	      printf(".");
      }
      else
      {
	      SetVectIntern(i, isr_RST10);
	      v = GetVectIntern(i);
	      gotoxy(RES_X+i, RES_Y+2);
	      printf(( v != isr_RST10 ) ? "F" : "P");
      }
   }
   gotoxy(RES_X,RES_Y+4);
   printf("External Vectors:\n01");
   for (i=0; i<2; ++i)
   {
      SetVectExtern3000(i, isr_RST10);
      v = GetVectExtern3000(i);
      gotoxy(RES_X+i, RES_Y+6);
      printf(( v != isr_RST10 ) ? "F" : "P");
   }
   linesToErase = 7;
}//assignAllIntVectors()

/*--------------------------------------------------
 RESET EVENT TIMING STATISTICS
 --------------------------------------------------*/
nodebug
void resetStats (void)
{
   diff = 0L;
   diffcnt = 0L;
   maxdiff = 0L;
   gotoxy(RES_X, RES_Y); printf("Stats Reset\n");
   linesToErase = 1;
}//resetStats()

nodebug
void showStats (void)
{
	showstats = !showstats;
};

/*--------------------------------------------------
 CLEAR THE SCREEN
 --------------------------------------------------*/
nodebug
void clearTheScreen (void)
{
	showScreen();
	showMenu(cm);
}//clearTheScreen()

/*--------------------------------------------------
 TOGGLE CLOCK DISPLAY
 --------------------------------------------------*/
nodebug
void toggleClockDisplay (void)
{
	clockDisplay = !clockDisplay;
}//toggleClockDisplay()

/*--------------------------------------------------
 TOGGLE EVENT DISPLAY
 --------------------------------------------------*/
nodebug
void toggleEventDisplay (void)
{
	edisplay = !edisplay;
}//toggleEventDisplay()

/*--------------------------------------------------
 Save Network Parameters in UPI structure.
 --------------------------------------------------*/
void saveNetParms (void)
{
	_sys_UPISaveData();
	gotoxy(RES_X,RES_Y+1);
	printf("saved okay\n");
	linesToErase = 2;
}

/*--------------------------------------------------
 CHANGE SERIAL CONSOLE PORT
 --------------------------------------------------*/
void changePort (char c)
{
	int rslt;
	char *msg;

   rslt = _sys_con_alt_serial(c);
   if (rslt)
   {
      if (rslt == -EBUSY)
         msg = "Console session in progress; unable to switch " \
               "ports now.\n";
      else if (rslt == -EACCES)
         msg = "Console port changed already or disabled. " \
               "Command failed.\n";
      else if (rslt == -EINVAL)
         msg = "Invalid port specified.\n";
      else
         msg = "Unknown error code returned from " \
               "_sys_con_alt_serial()";
   }
   else
      msg = "Port changed.\n";
   gotoxy(RES_X,RES_Y+1);
   printf("%s",msg);
   linesToErase=2;
}//changePort()

/*--------------------------------------------------
 ACTIVE.C display a web page.

 Show a web page as raw HTML on the display.
 --------------------------------------------------*/
tcp_Socket socket;
void readAWebPage ( char *URL, char *page )
{
	char	buffer[100];
	int 	bytes_read;
	longword destIP;
	long timer;

   if (!page)
   {
   	printf("page?\n");
   	return;
   }
	gotoxy(RES_X, RES_Y+2);
	if( 0L == (destIP = resolve(URL)) ) {
		printf( "ERROR: Cannot resolve \"%s\" into an IP address\n", URL );
		return;
	}
	tcp_open(&socket,0,destIP,80,NULL);

	printf("Waiting for connection");
	timer = MS_TIMER;
	while(!sock_established(&socket) && sock_bytesready(&socket)==-1) {
		tcp_tick(NULL);
		if ((MS_TIMER-timer) > 20000)
		{
			printf("timeout waiting for connection\n");
			sock_abort(&socket);
			return ;
		}
		if ( ((int)MS_TIMER % 1000) == 0)
		{
			printf(".");
		}
	}
	printf("\n");

	printf("Connection established, sending get request...\n");

	/*
	 *  If don't send the HTTP version number, then server believes we are
	 *  a pre-1.0 version client.
	 */
	if ( *page == 0 )
		strcpy(page,"/");
	sprintf(buffer,"GET %s\r\n\r\n", page);
	sock_write(&socket, buffer, strlen(buffer) );

	/*
	 *  When tcp_tick()-ing on a specific socket, we get non-zero return while
	 *  it is active, and zero when it is closed (as used here).
	 */
	do {
		bytes_read=sock_fastread(&socket,buffer,sizeof(buffer)-1);

		if(bytes_read>0) {
			buffer[bytes_read] = '\0';
			/*
			 * By using the "%s" format, if there are "%" in the buffer, printf()
			 *  won't try to interpret them!
			 */
			printf("%s",buffer);
		}
		//check for abort.
		if ( kbhit() != 0)
		{
			if (getchar() == 27)
			{
				sock_abort(&socket);
				return;
			}
		}
	} while(tcp_tick(&socket));

	sock_abort(&socket);
	printf("\nConnection closed...\n");
} //readAWebPage()

/*--------------------------------------------------
  This is ConsoleTest's command interpreter that is
  called from the RabbitSys Console. "cmd" is the
  first space delimited string on the command line,
  and "args" is anything that follows, if there is
  anything.

  This function is called in user-mode. It must finish
  before the watchdogs hit, or service them from here.
 --------------------------------------------------*/
char *myCommandInterpreter (char *cmd, char *args )
{
	int i;

	gotoxy(0,19);
	printf("Command:   %s\n",cmd);

	// args will be NULL if there are no arguments
	if (args)
		printf("Arguments: %s\n",args);
	else
		printf("No arguments provided\n");

	// We can look for anything and do anything.
	if (strcmp(cmd,"dognuts"))
		return NULL;
	return "command dognuts entered\r\n";
}

//nodebug
void main ()
{
	int newport, rslt;
	char *msg, cmdStr[48], cmdStr2[48];
	int tc;
	char *s;
	char ch;
	_sys_event_data_t edata;
	unsigned int *eflags;
	eventStruct *edataPtr;
	_sys_event_handle seh;
	int myWatchdog;
	void *p;
	_sys_tcp_Socket *tcp;
	_sys_udp_Socket *udp;
	int *iptr;
	int disp_isrcount;
	eventStack_data_t ed;
	long timer;
	int wdogs[10];

	if (sock_init())
	{
	   printf("cannot allocate socket memory\r\n");
	}
   //setupRSNetwork();
   while (ifpending(IF_DEFAULT) == IF_COMING_UP)
   {
		tcp_tick(NULL);
	}

	// register our own command interpreter with the RabbitSys Console.
	_sys_con_RegisterCmdI(&myCommandInterpreter);

	memset(evntStrs,0,sizeof(evntStrs));
	tc=disp_isrcount=0;
	ecnt = 0;
	linesToErase = 0;
	diff = 0L;
	maxdiff = 0L;
	diffcnt = 0L;
	edisplay = 1;
	clockDisplay = 1;
   rst10Count = rst38Count = 0;
	cm = menuSystem[1];
	showScreen();
	showMenu(cm);
	eventStack.tos = -1;
	_sys_swd_period(0x64FF);			//100 * 7.8ms
	SetupInitialTimers();
	showstats = 1;
	while (1)
	{
		costate Rabbit always_on
		{
      	if (clockDisplay)
         {
	         gotoxy(50,5);
	         printf("MS_TIMER   :%lu",MS_TIMER);
	         gotoxy(50,6);
	         printf("SEC_TIMER  :%lu",SEC_TIMER);
	         gotoxy(50,7);
	         printf("TICK_TIMER :%lu",TICK_TIMER);
	         waitfor(DelayMs(500));
         }
      }
      costate rhinoceros always_on
      {
      	if ( eventStack.tos != -1 )
      	{
      		memcpy(&ed, eventStackPop(), sizeof(eventStack_data_t) );
      		gotoxy(ed.x, ed.y);
      		printf(ed.s);
      	}
      }
      costate hippopotomus always_on
      {
        	if (disp_isrcount)
        	{
	         gotoxy(0,28);
	         printf("RST10 %x  RST38 %x",rst10Count,rst38Count);
	      }
         waitfor(DelayMs(1000L));
         if ( showstats )
         {
	         gotoxy(50,22);
	         printf("Event Statistics\n");
	         gotoxy(50,23);
	         printf("----------------\n");
	         gotoxy(50,24);
	         if (diffcnt) printf("Avg Overage %-8ld\n",(diff/diffcnt));
	         gotoxy(50,25);
	         printf("Max Overage %ld",maxdiff);
	      }
      }
	   costate Elephant always_on //menu and test functions
	   {
         gotoxy(INP_X, INP_Y);
	   	printf("                     \r");	//clear input area
	   	printf("Select an option: ");

			while (kbhit() == 0)
		   	waitfor(DelayMs(50));
		   gotoxy(INP_X,INP_Y);
		   rslt=getchar();
			cmdStr[0] = rslt; cmdStr[1] = 0;
         printf(cmdStr);

			linesToErase = clearResultArea(linesToErase);
	   	gotoxy(RES_X, RES_Y);

			mp = findMenuItem(cm, rslt);
			if ( mp->menuFunc )
			{
				rslt = (mp->menuFunc)();
			}

		   switch (mp->item)
		   {
				case 0:
					if ( mp->menuLink )
					{
						cm = menuSystem[mp->menuLink];
						showMenu(cm);
					}
					break;
		   	case 1:
					/*--------------------------------------------------
					CHANGE SERIAL CONSOLE PORT
					 --------------------------------------------------*/
	            ++linesToErase;
					waitfor(my_gets("New port (A,B,C,D,E,F):", cmdStr,
						RES_X, RES_Y));
					if (*cmdStr == ESCAPE)
						break;
					*cmdStr = toupper(*cmdStr);
               strcpy(&cmdStr[1]," selected.\n");
               gotoxy(RES_X+25,RES_Y);
	            printf("%s",cmdStr);
	            changePort(*cmdStr);
					break;
				case 2:
					/*--------------------------------------------------
					 DISABLE SERIAL CONSOLE
                The serial disable function is called prior to this
                because of the menu function field.  "rslt" has the
                result for this call.
					 --------------------------------------------------*/
					if (rslt)
					{
						if (rslt == -EBUSY)
	                  msg = "Console session in progress; unable to disable " \
	                        "port now.\n";
	               else if (rslt == -EPERM)
	               	msg = "Serial Console already disabled\n";
					}
					else
						msg = "Serial Console disabled\n";
               gotoxy(RES_X+25,RES_Y);
					printf("%s",msg);
					++linesToErase;
					break;
				case 3:
					/*--------------------------------------------------
					 read a web page.  URL refers to the URL of a site,
					 while page is the content address within the site.
					 Ex., URL = www.zworld.com, page=/company/careers
					 would load www.zworld.com/company/careers. The raw
					 HTML is displayed. If ESCape is pressed while the
					 page is printing the routine aborts.
					 --------------------------------------------------*/
					*cmdStr2 = ESCAPE;
               waitfor(my_gets("URL:", cmdStr, RES_X, RES_Y));
               if (*cmdStr != ESCAPE)
						waitfor(my_gets("page:", cmdStr2, RES_X, RES_Y+1));
					if ( *cmdStr2 != ESCAPE )
               {
						edisplay = 0;
						clockDisplay = 0;
	               disp_isrcount = 0;
	               showstats = 0;
                  readAWebPage (cmdStr, cmdStr2);
                  printf("\n\nPress any key...");
                  while (kbhit() == 0) ;
                  clearTheScreen();
               }

					break;
				case 4:
					/*--------------------------------------------------
					 REMOVE AN EVENT
					 Remove an event that you defined.
					 --------------------------------------------------*/
					clearMenu();
					gotoxy(0,2);
					for (tc=0; tc<10;++tc)
					{
                	if (evntStrs[tc].seh)
                		printf("%x >%s\n",tc,evntStrs[tc].eventString);
					}
               waitfor(my_gets("index?", cmdStr, RES_X, RES_Y));
               showScreen();
               showMenu(cm);
               if (*cmdStr != ESCAPE)
               {
               	tc = atoi(cmdStr);
               	rslt = _sys_remove_event(&evntStrs[tc].seh);
               	gotoxy(RES_X, RES_Y+1);
               	printf("_sys_remove_event() result:%x\n",rslt);
               }
               linesToErase = 3;
					break;
				case 5:
					/*--------------------------------------------------
					 LOG ERRORS - RUNTIME
					 Log a value in the runtime log.
					 --------------------------------------------------*/
               waitfor(my_gets("Value to enter in runtime log:",
               	cmdStr, RES_X, RES_Y));
               if ( *cmdStr != ESCAPE )
           			_sys_mon_rt_error(atoi(cmdStr));
               linesToErase = 3;
					break;
				case 6:
					/*--------------------------------------------------
					 ADD AN EVENT
					 Add timer, alert, and shutdown events.  All events
					 ask for a string to print on the display if triggered.
					 Timer events may be recurring and EBO (exponential
					 back-off).
					 --------------------------------------------------*/
					++linesToErase;
					for (ecnt=0;ecnt<20;++ecnt)
					{
						if (evntStrs[ecnt].seh==NULL)
							break;
					}
					if (ecnt == 20)
					{
						gotoxy(RES_X, RES_Y);
						printf("no more event message space\n");
						break;
					}
					waitfor(my_gets("Type (1:timer, 2:alert, 3:shutdown, n:other):",cmdStr,
						RES_X, RES_Y+1));
					if ( *cmdStr != ESCAPE )
					{
						++linesToErase;
               	rslt = atoi(cmdStr);
               	if ( rslt == _SYS_EVENT_TIMER )
               	{
               		eflags = &edata.timer.tflags;
               		edataPtr = &edata.timer.data;
               	}
               	else
               	{
               		eflags = &edata.startup.aflags;
               		edataPtr = &edata.startup.data;
               	}
               	gotoxy(RES_X,RES_Y+2);
               	printf("data string:\r\n");

               	waitfor(my_gets("", cmdStr, RES_X+36, RES_Y+2));
						if ( *cmdStr == ESCAPE )
						{
							break;
						}
               	strcpy(evntStrs[ecnt].eventString,cmdStr);
						++linesToErase;
              	   waitfor(my_gets("Flags (Sys:8000, Recur:1, EBO:2, User:xxxx): ", cmdStr,
              	   	RES_X, RES_Y+3));
						if ( *cmdStr == ESCAPE )
						{
							break;
						}
              		*eflags = ahtoi(cmdStr);

               	if (rslt == _SYS_EVENT_TIMER)
               	{
							++linesToErase;
               	   waitfor(my_gets("Interval (ms): ", cmdStr, RES_X, RES_Y+4));
							if ( *cmdStr == ESCAPE )
							{
								break;
							}
                     edisplay = 1;
               		edata.timer.interval = atol(cmdStr);
               	}
               	(void *)*edataPtr = &evntStrs[ecnt];
               	if (rslt == _SYS_EVENT_TIMER)
               	{
               		rslt = _sys_add_event(rslt,timerEventProc,
               		 	&evntStrs[ecnt].seh,&edata);
               	}
               	else
               	{
               		rslt = _sys_add_event(rslt,alertEventProc,
               			&evntStrs[ecnt].seh,&edata);
               	}
               	if (rslt)
               	{
               		printf("Error adding event:%x\r\n",rslt);
               	}
               	++ecnt;
					}
               linesToErase = 5;
					break;
				case 7:
					/*--------------------------------------------------
					 SET EMAIL ADDRESS IN RABBITSYS [_sys_mon_set_email()]
					 call the monitor routine to set the address.  You may
					 set it to "".
					 --------------------------------------------------*/
					waitfor(my_gets("email address:", cmdStr, RES_X, RES_Y));
					if ( *cmdStr != ESCAPE )
					{
						waitfor(my_gets("SMTP IP address:", cmdStr2, RES_X, RES_Y+1));
						if ( *cmdStr != ESCAPE )
                  {
	                  if (tc = _sys_mon_set_email(cmdStr2, cmdStr))
	                  {
	                     itoa(tc, cmdStr);
	                     printf("Error setting email address:%s",cmdStr);
	                  }
                  }
					}
					linesToErase = 3;
					break;

				case 8:
					break;

				case 9:
					/*--------------------------------------------------
					VIRTUAL WATCHDOG TEST
					 allocate a watchdog, then hit it when the user
					 presses a key.  If the user presses Escape release
					 the watchdog and exit this test.
					 --------------------------------------------------*/
					waitfor(my_gets("Enter Virtual Watchdog Count:", cmdStr,
						RES_X, RES_Y));
					if (*cmdStr == ESCAPE)
						break;
					waitfor(my_gets("Enter number of watchdogs:", cmdStr2,
						RES_X, RES_Y+1));
					if (*cmdStr2 == ESCAPE || atoi(cmdStr2)==0)
						break;
					gotoxy(RES_X,RES_Y+2);
              	printf("Press Escape to stop watchdog\n" \
              		"Press any other key to hit the watchdog\n");
              	waitfor (DelayMs(1000L));
              	for (tc=0;tc<atoi(cmdStr2);++tc)
              	{
	               wdogs[tc] = VdGetFreeWd(atoi(cmdStr));
              	}
               while (1)
               {
               	yield;
   	           	for (tc=0;tc<atoi(cmdStr2);++tc)
	              	{
	                  gotoxy(RES_X,RES_Y+4+tc);
	                  printf("%d", vd_wdogarray[wdogs[tc]]>>8);
	                  if ( kbhit() )
	                  {
	                     if ( getchar()==ESCAPE )
	                     {
	                     	for (tc=0;tc<atoi(cmdStr2);++tc)
		                        VdReleaseWd(wdogs[tc]);
	                        goto VDogTestDone;
	                     }
	                     else
	                     {
	                     	for (tc=0;tc<atoi(cmdStr2);++tc)
		                        VdHitWd(wdogs[tc]);
	                     }
	                  }
                  }
               }//while
      		VDogTestDone:
               linesToErase = 5+atoi(cmdStr2);
					break;
				case 10: // Query an Event (ETA)
					/*-----------------9/14/2005 3:17PM-----------------
					 check when an event is going to occur. List all our
					 defined events and ask for an index. Call
					 _sys_event_eta() for the eta.
					 --------------------------------------------------*/
					clearMenu();
					gotoxy(0,2);
					for (tc=0; tc<10;++tc)
					{
                	if (evntStrs[tc].seh)
                		printf("%d >%x:%s\n",tc,evntStrs[tc].seh,evntStrs[tc].eventString);
					}
               waitfor(my_gets("index?", cmdStr, RES_X, RES_Y));
               if (*cmdStr != ESCAPE)
               {
               	tc = atoi(cmdStr);
               	timer = _sys_event_eta(evntStrs[tc].seh);
               	gotoxy(RES_X, RES_Y+1);
               	if (timer == 2147483647L)
               		printf("_sys_event_eta() result: MAXLONG (Non-timed event)\n");
               	else
               		printf("_sys_event_eta() result:%Ld\n",timer);
               }
               linesToErase = 3;
               showMenu(cm);
					break;
				case 11: //Set tick interval
					/*-----------------9/14/2005 3:24PM-----------------
					 set the automatic RabbitSys tick servicing interval.
					 If you set it to zero the watchdogs will hit and
					 stop this application because it doesn't call _sys_tick().
					 --------------------------------------------------*/
					waitfor(my_gets("Tick interval in milliseconds:", cmdStr,
						RES_X, RES_Y));
					if (*cmdStr != ESCAPE)
					{
						rslt = _sys_con_settickinterval(atoi(cmdStr));
               	gotoxy(RES_X, RES_Y+1);
						printf("_sys_con_settick() result: %x\n",rslt);
					}
					linesToErase = 3;
					break;
				case 12: //Set RTE behavior
					/*-----------------9/14/2005 3:25PM-----------------
					 set the runtime error behavior.  If set to "stop" a
					 runtime error will cause the application to stop,
					 triggering shutdown events, and marking the application
					 as stopped.
					 --------------------------------------------------*/
					*cmdStr = 0;
					while ( (*cmdStr != 'c') && (*cmdStr != 's') )
					{
               	waitfor(my_gets("RTE behavior ('s'top or 'c'ontinue):", cmdStr,
							RES_X, RES_Y));
	               if (*cmdStr == ESCAPE) break;
	            }
               _sys_con_setrte((int)*cmdStr);
              	gotoxy(RES_X, RES_Y+1);
               printf("_sys_con_setrte() completed.\n",rslt);
					linesToErase = 3;
					break;
				case 13: //Polled timer event
					/*--------------------------------------------------
					 Add a timer event without a callback function and
					 wait for it to expire.
					 --------------------------------------------------*/
               ++linesToErase;
               waitfor(my_gets("Time (ms): ", cmdStr, RES_X, RES_Y));
               if ( *cmdStr == ESCAPE )
               {
                  break;
               }
               edata.timer.interval = atol(cmdStr);
           		edata.timer.tflags = 0;
           		edata.timer.data = NULL;
           		// for this instance we will use a local variable for the
           		// event handle since it will not go out of scope.
           		rslt = _sys_add_event(_SYS_EVENT_TIMER,NULL,&tc,&edata);
           		if (!rslt)
           		{
           			gotoxy(RES_X, RES_Y+1);
           			printf("Should expire at %ld",atol(cmdStr)+MS_TIMER);
           			gotoxy(RES_X, RES_Y+2);
           			printf("Polling...");
           			while (tc) yield;
           			gotoxy(RES_X+11, RES_Y+2);
           			printf("Timer expired at %ld.",MS_TIMER);
           		}
           		else
           		{
           			gotoxy(RES_X, RES_Y+1);
           			printf("failed to add event: %d",rslt);
           		}

					break;
				case 14: //Set number of users
					break;
				case 15: // set interrupt level
					/*--------------------------------------------------
					 set interrupt level.  If you set it greater than
                zero (0) the periodic interrupt will be blocked and
                no watchdog servicing will take place.  The dog will
                bite you...
					 --------------------------------------------------*/
					waitfor(my_gets("Interrupt level (0-3):", cmdStr,
						RES_X, RES_Y));
					if (*cmdStr != ESCAPE)
					{
						rslt = atoi(cmdStr);
						asm ld hl,(sp+@sp+rslt) $ ld h,L $ push hl $ pop ip $ add sp,1
               	gotoxy(RES_X, RES_Y+1);
						printf("Interrupt level set to %s.\n",cmdStr);
					}
					linesToErase = 3;
					break;
				case 16: // Trigger an event
					/*--------------------------------------------------
					 trigger an event by calling sys_exec_event(eventtype).
					 --------------------------------------------------*/
					 waitfor(my_gets("Event type to trigger (a number):",cmdStr,
                	RES_X,RES_Y));
					 if (*cmdStr != ESCAPE)
					 {
					 	if (_sys_exec_event(atoi(cmdStr)) )
					 	{
					 		gotoxy(RES_X,RES_Y+1);
					 		printf("Event type does not exist.");
					 	}
					 }
					 linesToErase = 3 ;
					break;
				case 17:
					/*--------------------------------------------------
					 ASSIGN ISR's
					 Assign isr's to the RST10 or RST38 vectors.
					 --------------------------------------------------*/
               waitfor(my_gets("Set RST10 (1) or RST38 (2)?", cmdStr, RES_X, RES_Y));
               if (*cmdStr=='1')
               {
                  SetVectIntern(2, isr_RST10);
                  if ( GetVectIntern(2) != isr_RST10 )
                  {
                  	gotoxy(RES_X, RES_Y+1);
                  	printf("setting RST 0x10 vector failed.\n");
                  }
               }
               else if (*cmdStr=='2')
               {
						SetVectIntern(7, isr_RST38);
                  if ( GetVectIntern(7) != isr_RST38 )
                  {
                  	gotoxy(RES_X, RES_Y+1);
                  	printf("setting RST 0x38 vector failed.\n");
                  }
               }
               linesToErase = 3;
					break;
				case 18:
					/*--------------------------------------------------
					 TRIGGER ISR's
					 Cause RST10 or RST38 to occur.
					 --------------------------------------------------*/
               waitfor(my_gets("Trigger RST10 (1) or RST38 (2)?", cmdStr, RES_X, RES_Y));
               if (*cmdStr=='1')
               {
               	asm rst 0x10;
               	disp_isrcount =1;
               }
               else if (*cmdStr=='2')
               {
						asm rst 0x38;
               	disp_isrcount =1;
               }
               linesToErase = 3;
					break;

            case 19: // Get Log Contents..
					/*--------------------------------------------------
                Get the contents of a log.
					 --------------------------------------------------*/
            	waitfor(my_gets("Which log (0-4)?",cmdStr,RES_X,RES_Y));
               if (*cmdStr==ESCAPE) break;
            	for (rslt = 0; !_sys_mon_get_log(atoi(cmdStr),bigbuf); ++rslt)
               {
               	gotoxy(RES_X,RES_Y+rslt);
                  printf(bigbuf);
               }
               if (strlen(bigbuf))
               {
	            	gotoxy(RES_X,RES_Y+rslt);
	            	printf(bigbuf);
	            	++rslt;
               }
	            gotoxy(RES_X,RES_Y+rslt);
	            printf("Log Contents Done.");
               linesToErase = rslt;
            	break;

            case 20: // Set Secondary Watchdog timeout
            	waitfor(my_gets("Enter raw counts (1-255):",cmdStr2,RES_X,RES_Y));
               if (*cmdStr2 == ESCAPE)
               	break;
               rslt = atoi(cmdStr2);
               if (rslt > 255 || rslt < 1)
               {
               	gotoxy(RES_X,RES_Y+1);
                  printf("Value out of range");
               }
               else
               {
	               waitfor(my_gets("Enter # of counts(0-255):",cmdStr,RES_X,RES_Y+1));
	               if (*cmdStr != ESCAPE)
	               {
                  	tc = atoi(cmdStr);
                     if (tc > 255 || tc < 0)
                     {
      		         	gotoxy(RES_X,RES_Y+2);
		                  printf("Value out of range");
                     }
                     else
                     {
                  		_sys_swd_period(atoi(cmdStr)*256 + rslt);
			               gotoxy(RES_X,RES_Y+2);
			               printf("secondary watchdog timeout set");
                     }
                  }
               }
               linesToErase = 3;
            	break;

            case 21:
            	break;

				default:
					printf("What?\n");
			}//switch
	   }//costate
	}//while
} // main()

