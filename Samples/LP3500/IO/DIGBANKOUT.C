/**************************************************************************

	digbankout.c
   Z-World, 2002

	This sample program is for the LP3500 series controllers.

	This program demonstrates writing values to a bank of outputs, this
	sample program uses the DEMO board that was provided in your development
	kit so you can see the LED's toggle ON/OFF via the high-current outputs.

	Connections
	===========
	Connect +K to external power source.

	When the controller is plugged into to the demo board the
	following connections are readily available.

	Controller		Demo Board
	----------		----------
			OUT0 <->	DS1
			OUT1 <->	DS2
			OUT2 <->	DS3
			OUT3 <->	DS4

	Instructions
	============
	1. Compile and run this program.

	2. The program will prompt you for your output bank selection, select
	   0 for the bank of OUT0 to OUT9.

	3. After you have made the selection you'll be prompted to a hex byte
		value, enter AA.

	4.	Note that LED's DS1 and DS3 will be lit.

	5. Note that for OUT8 and OUT9, you must still enter a byte value.
		For example, 02.

**************************************************************************/
#class auto

//------------------------------------------------------------------------
// Set to initially disable outputs OUT9-OUT0.
//------------------------------------------------------------------------
#define BANK0 0		//OUT0 to OUT7
#define BANK1 1		//OUT8 and OUT9
#define OUTCONFIG0	0xFF		//configure bank 0
#define OUTCONFIG1	0x00		//congigure bank 1

// Set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto char s[4];
	auto char display[128];
	auto int channel, output_level, output0, output1;
	auto int up4, lo4;

   brdInit();

	// Display user instructions and channel headings
	DispStr(8, 1, " <<< Sinking output channels  = OUT1-OUT7   >>>");
	DispStr(8, 2, " <<< Sourcing output channel  = OUT8-OUT9   >>>");
	DispStr(8, 4, "OUT0\tOUT1\tOUT2\tOUT3\tOUT4\tOUT5\tOUT6\tOUT7");
	DispStr(8, 5, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 9, "OUT8\tOUT9");
	DispStr(8, 10, "-----\t-----");

	DispStr(8, 14, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 15, "(See instructions in sample program for complete details)");
	DispStr(8, 21, "<-PRESS 'Q' TO QUIT->");

	//intialize output values
	output0 = OUTCONFIG0;		// bank 0 OUT0-7
	output1 = OUTCONFIG1;		// bank 1 OUT8-9

	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		digBankOut(BANK0, output0);
		output_level=output0;
		display[0] = '\0';									//initialize for strcat function
		//display output values
		for(channel = 0; channel <= 7; channel++)
		{
			sprintf(s, "%d\t", output_level&0x0001);	//format logic level for display
			strcat(display,s);								//add to display string
			output_level >>= 1;								//output logic level to channel
		}
		DispStr(8, 6, display);

		digBankOut(BANK1, output1);
		output_level=output1;
		display[0] = '\0';
		//display output values
		for(channel = 8; channel <= 9; channel++)
		{
			sprintf(s, "%d\t", output_level&0x0001);	//format logic level for display
			strcat(display,s);								//add to display string
			output_level >>= 1;								//output logic level to channel
		}
		DispStr(8, 11, display);

		// Wait for user to make output channel selection or exit program
		sprintf(display, "Enter '0' to change outputs 0 to 7 or '1' for 8 and 9 .... Bank ");
		DispStr(8, 17, display);
		gets(s);
		if (s[0] == 'Q' || s[0] == 'q')		// check if it's the q or Q key
		{
     		exit(0);
  		}

		channel = atoi(s);

		// Wait for user to select logic level or exit program
		sprintf(display, "Enter hex byte value (ie: 3F or 0A)    ");
		DispStr(8, 18, display);
		gets(s);

		if (s[0] > 0x39)
			up4 = toupper(s[0])-0x37;
		else
			up4 = s[0]-0x30;
		up4 <<= 4;

		if (s[1] > 0x39)
			lo4 = toupper(s[1])-0x37;
		else
			lo4 = s[1]-0x30;

		if (channel == BANK0)
			output0 = up4|lo4;
		else
			output1 = up4|lo4;

  		// Clear channel and logic level selection prompts
  		DispStr(8, 17, "                                                                 ");
  		DispStr(8, 18, "                                                                 ");
   }
}
///////////////////////////////////////////////////////////////////////////



