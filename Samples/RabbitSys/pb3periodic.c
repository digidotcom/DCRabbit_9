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