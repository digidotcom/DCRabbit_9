/********************************************************************
	dac.c
	Z-World, 2002

	This sample program is intended for the BL2500 series controllers.
	
	Description
	===========
	This program demonstrates pulse-width modulation as an analog
	output voltage by displaying voltage entered and measuring
	voltage output.
	
	
	Instructions
	============
	1. Compile and run this program.
	2. Connect a voltmeter to DA0 or DA1 to measure the voltage
		output.

*********************************************************************/
#class auto

#define DA0	0
#define DA1	1

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
	pwmOutVolts(DA0, 0.0);
	pwmOutVolts(DA1, 0.0);
	
	DispStr(8, 2,  "DAC 0\tDAC 1");
	DispStr(8, 3, "-----\t-----");
	sprintf(buf, "%.3f\t%.3f", 0.0, 0.0);
	DispStr(8, 4, buf);
	DispStr(8, 10, "Press Q to quit");
}

	
main()
{
	auto char tmpbuf[64];
	auto float voltbuf[2];
	auto int channum;

	brdInit();
	scrnInit();
	voltbuf[0] = voltbuf[1] = 0.0;
			
	while (1)
	{
		do
		{
			DispStr(8, 7, "Enter 0 for DA0 or 1 for DA1  ");
			gets(tmpbuf);
			if (!strcmp(tmpbuf,"q") || !strcmp(tmpbuf,"Q"))	// check if it's the q or Q key        
			{        
  				exit(0);               
  			}
			channum = atoi(tmpbuf);
		}	while (channum != 0 && channum != 1);

		DispStr(8, 8, "Enter a voltage from 0 to 3.1  ");
		gets(tmpbuf);
		voltbuf[channum] = atof(tmpbuf);

		sprintf(tmpbuf, "%.3f\t%.3f", voltbuf[0], voltbuf[1]);
		DispStr(8, 4, tmpbuf);
		pwmOutVolts(channum, voltbuf[channum]);
		
		DispStr(8, 7, "                                        ");
		DispStr(8, 8, "                                        ");
	}	
}
