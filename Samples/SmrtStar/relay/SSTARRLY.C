/**********************************************************
	sstarrly.c

   Z-World, 2001
	This sample program is used with Smart Star products.
	
	This program demonstrates a simple relay on/off.

	This setup defaults to a board with 6 relays.
	Change the macro below to match your board.
**********************************************************/
#class auto

#define NUMRELAYS 6  //change here for 6 or 8 relays


void sdelay (long sd)
{
	auto unsigned long t1;

	t1 = MS_TIMER + sd*1000;
	while ((long)(MS_TIMER-t1) < 0);
}

//////////////////////////////////////////////////////////

void main()
{
	auto int slotnum, relay;
	
	brdInit();

	printf("Please enter Relay board slot position, 0 thru 6....");
	do {
		slotnum = getchar();
		} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	
	for (relay=0; relay<NUMRELAYS; relay++)
	{
		relayOut(ChanAddr(slotnum, relay),1);	//on
		printf("Relay %d is on\n", relay);
		sdelay(1);
		relayOut(ChanAddr(slotnum, relay),0);	//off
		printf("Relay %d is off\n", relay);
		sdelay(1);
	}
		
}
//////////////////////////////////////////////////////////

