/**********************************************************
	sstario.c
	
   Z-World, 2001
	This sample program is used with Smart Star products.
	
	This program demonstrates a simple digital I/O using
	individual channels and whole banks.
	
	This setup defaults to 16 inputs and 8 outputs.
	Change the macros below to match your board.
	
**********************************************************/
#class auto

#define INPUTS 16		//change here for 8 or 16 inputs
#define OUTPUTS 8		//change here for 8 or 16 outputs


void msdelay (long sd)
{
	auto unsigned long t1;

	t1 = MS_TIMER + sd;
	while ((long)(MS_TIMER-t1) < 0);
}

void sdelay (long sd)
{
	auto unsigned long t1;

	t1=SEC_TIMER + sd;
	while( (long)(SEC_TIMER-t1) < 0 );
}


//////////////////////////////////////////////////////////

void main()
{
	auto char slotnum, ionum, i;
	
	brdInit();

	printf("Please enter I/O board slot position, 0 thru 6....");
	do {
		slotnum = getchar();
		} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	///// read inputs
	printf("Read channels 0 thru %d:\n", INPUTS-1);
	for (i=0; i<INPUTS; i++)
	{
		ionum = digIn(ChanAddr(slotnum, i));				//inputs individually
		printf("%d", ionum);
	}
		
	///// read whole bank 0	
	ionum = digBankIn(BankAddr(slotnum, 0));			//same thing except whole bank
	printf("\necho Bank 0 reads %x\n", ionum);
	
	///// read whole bank 2
	if (INPUTS == 16)
	{
		ionum = digBankIn(BankAddr(slotnum, 2));			//same thing except whole bank
		printf("echo Bank 2 reads %x\n", ionum);
	}
		
	///// write outputs
	for (ionum=0; ionum<OUTPUTS; ionum++)
	{				//outputs, write 1 individually
		printf("Write '1' and '0' to channel %d\n", ionum);
		digOut(ChanAddr(slotnum, ionum), 1);
		msdelay(500);
		digOut(ChanAddr(slotnum, ionum), 0);
		msdelay(500);
	}
		
	///// write whole banks one
	printf("Write '0xAA' to Bank 0 \n");
	digBankOut(BankAddr(slotnum, 1), 0xAA);					//same thing except whole bank
	sdelay(1);
	if (OUTPUTS == 16)
	{
		printf("Write '0xAA' to Bank 1 \n");
		digBankOut(BankAddr(slotnum, 2), 0xAA);					//same thing except whole bank
	}
	sdelay(1);
		
	///// write whole banks zero
	printf("Write '0' to Bank 0 \n");
	digBankOut(BankAddr(slotnum, 1), 0);					//same thing except whole bank
	sdelay(1);
	if (OUTPUTS == 16)
	{
		printf("Write '0' to Bank 1 \n");
		digBankOut(BankAddr(slotnum, 2), 0);					//same thing except whole bank
	}
}
//////////////////////////////////////////////////////////

