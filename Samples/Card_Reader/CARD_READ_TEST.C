/*****************************************************************************
card_read_test.c
ZWorld, 2002


This sample program demonstrates the use of the magnetic card reader library
for the Rabbit 3000. To use this sample you will need a card reader device
that outputs card data in clocked format. Please see the related appnote for
more details.
Wire the card reader to the Rabbit as follows:

Rabbit	Card Reader
PG1		/CARD PRESENT
PG2		TRACK1 DATA
PG3		TRACK1 STROBE
PG4		TRACK2 DATA
PG5		TRACK2 STROBE
PG6		TRACK3 DATA
PG7		TRACK3 STROBE

If your card reader device supports fewer than three tracks, you can leave
the extra track signals on the rabbit unconnected. Since there is no set
standard for card reader hardware, you will need to consult the datasheet
the reader to make the correct connections.
Next, set the macro READER_TRACKS to however many tracks the card reader
hardware has. 
The program should now be ready to run.
Swiping a card through the reader should cause the card data to be printed
to STDIO if any of the tracks have BCD or ALPHA data on them.

****************************************************************************/

//priority for interrupts on card reader lines
#define CR_IPLEVEL 1
#define CR_USEPORTG

#use "card_reader.lib"

// set to number of tracks on card reader hardware
#define READER_TRACKS 2

//need a place to store bits as they are read in
#define TRACK_BUF_SIZE 128
char track_buffer[READER_TRACKS][TRACK_BUF_SIZE];
			

main()
{
	int i;
	int bit_count;
	char track_string[150];
	
	CRinit(READER_TRACKS);

	//assign buffers to each track to be read
	for(i = 0;i < READER_TRACKS;i++)
	{
		CRsetBuffer(i, track_buffer[i], TRACK_BUF_SIZE);
	}

	while(1)
	{
		CRstartRead();
		printf("Waiting for card swipe.\n");
		while(!CRcheckRead());	//just wait

		for(i = 0;i < READER_TRACKS;i++)
		{
			bit_count = CRgetBuffer(i, NULL);	
			printf("Track %d(%d bits)\n", i+1, bit_count);
			if(CRalphaDecode(track_buffer[i], bit_count, track_string))
			{
				printf("\nAppears to be ALPHA data\n%s\n", track_string);
			}
			else if(CRbcdDecode(track_buffer[i], bit_count, track_string))
			{
				printf("\nAppears to be BCD data\n%s\n", track_string);
			}
			else
			{
				printf("Could not decode bits\n");
			}
			printf("\n");
		}
		printf("\n");
	}
}
	
	