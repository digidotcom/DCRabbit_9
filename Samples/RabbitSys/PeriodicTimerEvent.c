/* START PROGRAM DESCRIPTION *********************************************


DESCRIPTION: 	This sample program illustrates how to use the RabbitSys
event system to create periodic events.  A one-second timer is created
and causes RabbitSys to call a user-supplied function to display the
current time.  The user-function accesses the _sys_event_data_t structure
supplied to all user-supplied event callback functions.

END DESCRIPTION **********************************************************/
typedef struct
{
	_sys_event_handle seh;
   char eventString[24];
} eventStruct;

long diff;
long maxdiff;
long diffcnt;
eventStruct evntStrs;
int  ecnt;

debug
void calc_diff (_sys_event_data_t *sd)
{
	long t;

   t = MS_TIMER - sd->timer.timeout;
	diff += t;
	if (t>maxdiff)
		maxdiff = t;
	++diffcnt;
}
debug
void eClockProc (_sys_event_handle *seh, void *data)
{
	struct tm rtc;					// time struct
	char hrs[4], min[4], sec[4];

	calc_diff(data);
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
	//gotoxy(55,0);
	printf("Event Procedure: %s -->%s:%s:%s\r\n",
		((eventStruct *)(((_sys_event_data_t *)data)->timer.data))->eventString,
		hrs,min,sec);
} //eClockProc()

void SetupInitialTimers (void)
{
	_sys_event_data_t edata;
	_sys_event_handle seh;
	int rslt;

   edata.timer.tflags = _SYS_EVENT_RECUR;//_SYS_EVENT_SYSTEM | _SYS_EVENT_RECUR;
   edata.timer.interval = 1000;
   strcpy(evntStrs.eventString,"1 second timer");
   edata.timer.data = &evntStrs;
	rslt = _sys_add_event(_SYS_EVENT_TIMER,eClockProc,&evntStrs.seh,&edata);
	++ecnt;
}

void main (void)
{
	ecnt = 0;
	SetupInitialTimers();
	while (1) ;
}

