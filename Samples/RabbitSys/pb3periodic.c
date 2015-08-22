/*
 This example demonstrates the use of a timer event to toggle bit 3 of
 parallel port B.  Use a multimeter or oscilloscope to view the on and
 off behavior of the pin.

 */


int pbx_ctr;
_sys_event_handle seh;

nodebug
void toggle_pbx(_sys_event_handle *seh, void *data)
{
	if(pbx_ctr % 2)
   {
		asm ld 	a,0x00
      asm ioi	ld (PBDR),a
   }
   else
   {
		asm ld 	a,0x08
      asm ioi	ld (PBDR),a
   }
   pbx_ctr++;
}

void main()
{
	int err;

	_sys_event_data_t ev_data;

	//setup port B data direction
	asm ld		a,0x08
   asm ioi	ld (PBDDR),a

   pbx_ctr = 0;

	//initialize the event data structure
	(int*)seh = NULL;
   ev_data.timer.tflags = _SYS_EVENT_RECUR;
   ev_data.timer.interval = 250;        		// every quarter second

	//register the event with rabbitsys.
	err = _sys_add_event(_SYS_EVENT_TIMER, toggle_pbx, seh, &ev_data);

   while(1)
   {
   	_sys_tick(1);
   }
}