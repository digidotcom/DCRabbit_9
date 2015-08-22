/***************************************************************************
	Samples\BL2000\ADC\ad1.c
	
   Z-World, 2001
	This sample program is used with the BL20XX series controllers.

	This program demonstrates how to access the A/D internal test voltages
	in both the TLC2543 and TLC1543 A/D chips. The program reads the A/D
	internal voltages and then displays the RAW data in the STDIO window. 

	Instructions:
	-------------

	1. Compile and run this program.
	2. The program will display the raw data for a given internal test
	   channel.
	   
	   A/D Channel 								 TLC2543	  	TLC1543
      ---------------------------------    -------		-------
	   channel 11  =  (Vref+  - Vref-)/2     0x800      0x200
	   channel 12  =  Vref-						  0x000      0x000
	   channel 13  =  Vref+						  0xFFF		 0x3FF

	   TLC2543 A/D chip is installed on the BL2000 and BL2020 controllers.
	   TLC1543 A/D chip is installed on the BL2010 and BL2030 controllers.
	
***************************************************************************/
#class auto

void main ()
{
	auto int rawdata11, rawdata12, rawdata13;
	auto int numbits;

	brdInit();
	if(_BOARD_TYPE_ == 0x0800 || _BOARD_TYPE_ == 0x0802)
	{
		numbits = 12;
	}
	else
	{
		numbits = 10;
	} 
	printf("\n\n\r");
	printf("Displaying RAW data for the %d bit A/D internal test voltages\n\r", numbits);
	printf("------------------------------------------------------------\n\r");
	
	rawdata11 = anaIn(11);	//read A/D internal test voltage (Vref+ - Vref-)/2
	rawdata12 = anaIn(12);	//read A/D internal test voltage  Vref-
	rawdata13 = anaIn(13);  //read A/D internal test voltage  Vref+

	printf("Rawdata count for ch11 is 0x%03x (Vref+ - Vref-)/2 \r\n", rawdata11);
	printf("Rawdata count for ch12 is 0x%03x  Vref-\r\n", rawdata12);
	printf("Rawdata count for ch13 is 0x%03x  Vref+\r\n", rawdata13);
	
}
