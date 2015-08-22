/**************************************************************************

	dio.c
   Z-World Inc 2003

	This sample program is for RCM3600 series controllers with
	prototyping boards.


	Description
	===========
	This program demonstrates digital input/output capabilities of the
   analog device by configuring two lines to outputs and two lines as
   inputs on JP4.


	Prototyping board connections
	=============================

   Connect JP4-1 to JP4-3.
   Connect JP4-2 to JP4-4.


	Instructions
	============
	1. Compile and run this program.

	2. The program will prompt you for your output selection JP4-1
   	or JP4-2.

	3. After you have made the selection you'll be prompted to enter
   	a logic 0 or 1.

	4.	JP4-3 will reflect the output of JP4-1. JP4-4 will reflect the
   	output of JP4-2.

**************************************************************************/
#class auto

#define CONFIG		0x06	   //configure JP4-1 and 2 as outputs

// screen foreground colors
#define	BLACK		"\x1b[30m"
#define	RED		"\x1b[31m"
#define	GREEN		"\x1b[32m"
#define	BLUE		"\x1b[34m"

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *color, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s%s", x, y, color, s);
}


// update digital inputs for the given channel range
void update_input(int col, int row)
{
	auto char s[40], reading;
	auto char display[80];
	auto int i;

	// display the input status for the given channel range
	display[0] = '\0';							//initialize for strcat function

	sprintf(s, "%c\t", '-');			//format reading in memory
	strcat(display,s);						//append reading to display string
	sprintf(s, "%c\t", '-');
	strcat(display,s);

	for (i=3; i<=4; i++)
	{
		reading = digIn(i);
		sprintf(s, "%d\t", reading);			//format reading in memory
		strcat(display,s);						//append reading to display string
	}

	DispStr(col, row, BLUE, display);			//update input status
}

//------------------------------------------------------------------------
// Set to initially disable outputs OUT9-OUT0.
//------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////

void main()
{
	auto char s[4];
	auto char display[128], output1[8];

	auto int channel;

	brdInit();                 //initialize controller

	//intialize output values
   digOut(1,0);			//output low
   digOut(2,0);      	//output low
	digConfig(CONFIG);  	//configure
   output1[1] = 0;
   output1[2] = 0;
   output1[3] = '-';
   output1[4] = '-';

	//Display user instructions and channel headings
	DispStr(8, 1, GREEN, "<<< Digital inputs >>>");
	DispStr(8, 3, BLACK, "JP4-1\tJP4-2\tJP4-3\tJP4-4");
	DispStr(8, 4, BLACK, "=====\t=====\t=====\t=====");

	DispStr(8, 7, GREEN, "<<< Digital outputs >>>");
	DispStr(8, 9, BLACK, "JP4-1\tJP4-2\tJP4-3\tJP4-4");
	DispStr(8, 10, BLACK, "=====\t=====\t=====\t=====");

	DispStr(8, 20, RED, "<-PRESS 'Q' TO QUIT->");

	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		display[0] = '\0';					//initialize strcat function

		//display output values
		for (channel = 1; channel <= 2; channel++)
		{
			digOut(channel, output1[channel]);
			sprintf(s, "%d\t", output1[channel]);	//format logic level for display
			strcat(display,s);								//add to display string
		}
		sprintf(s, "%c\t", output1[3]);
		strcat(display,s);
		sprintf(s, "%c\t", output1[4]);
		strcat(display,s);
		DispStr(8, 11, BLUE, display);

		// update input channels 1 - 4 (display at col = 8 row = 5)
		update_input(8, 5);

		// Wait for user to make output channel selection or exit program
		channel = -1;
		sprintf(display, "Enter '1' to change output JP4-1 or '2' for JP4-2 .... ");
		DispStr(8, 15, RED, display);
      while ((channel < 1) || (channel > 2))
      {
			DispStr(63, 15, RED, "                   ");
			DispStr(63, 15, RED, " ");
			gets(s);
			channel = atoi(s);
			if (s[0] == 'Q' || s[0] == 'q')		// check if it's the q or Q key
			{
     			exit(0);
  			}
      }

		// Wait for user to select logic level or exit program
      output1[channel] = '-';
		sprintf(display, "Enter logic 0 or 1 ....  ");
		DispStr(8, 16, RED, display);
      while ((output1[channel] < 0) || (output1[channel] > 1))
      {
			DispStr(33, 16, RED, "                   ");
			DispStr(33, 16, RED, " ");
			gets(s);
   	   output1[channel] = atoi(s);
			if (s[0] == 'Q' || s[0] == 'q')		// check if it's the q or Q key
			{
     			exit(0);
	  		}
      }

  		// Clear channel and logic level selection prompts
  		DispStr(8, 15, BLACK, "                                                                 ");
  		DispStr(8, 16, BLACK, "                                                                 ");
   }
}
///////////////////////////////////////////////////////////////////////////



