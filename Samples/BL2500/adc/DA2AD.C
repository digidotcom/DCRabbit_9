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
/********************************************************************
	da2ad.c

	This sample program is intended for the BL2500 series controllers.
	
	Description
	===========
	This program asks the user to input a voltage in the program then
	outputs that voltage on DA1.  The user should have DA1 output
	connected to AD0 input.  The program will display the voltage read
	on AD0.

		
	Instructions
	============
	1. Connect output channel DA1 to input channel AD0.
	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.

*********************************************************************/
#class auto

#define DA0	0
#define DA1	1
#define AD0 0

/////
// set the STDIO cursor location and display a string
/////
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}


void scrnInit(void)
{
	auto char buf[64];
	
	//initialize output voltage to zero
	pwmOutVolts(DA1, 0.0);
	
	DispStr(8, 2,  "DAC 1\tADC 0");
	DispStr(8, 3, "-----\t-----");
	sprintf(buf, "%.3f\t%.3f", 0.0, 0.0);
	DispStr(8, 4, buf);
	DispStr(8, 10, "Press Q to quit");
}

	
main()
{
	auto char tmpbuf[64];
	auto float voltbuf[3];
	auto int channum;

	brdInit();
	scrnInit();
	voltbuf[0] = voltbuf[1] = 0.0;
	channum = DA1;
			
	while (1)
	{

		DispStr(8, 8, "Enter a voltage from 0 to 3.1  ");
		gets(tmpbuf);
		if (!strcmp(tmpbuf,"q") || !strcmp(tmpbuf,"Q"))	// check if it's the q or Q key        
		{        
  			exit(0);               
		}
		voltbuf[channum] = atof(tmpbuf);

		pwmOutVolts(channum, voltbuf[channum]);
		voltbuf[2] = anaInVolts(AD0);
		sprintf(tmpbuf, "%6.3f\t%6.3f", voltbuf[1], voltbuf[2]);
		DispStr(8, 4, tmpbuf);
		
		DispStr(8, 7, "                                        ");
		DispStr(8, 8, "                                        ");
	}	
}

