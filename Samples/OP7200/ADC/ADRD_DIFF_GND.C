/***************************************************************************
	adrd_diff_gnd.c
	Z-World, 2002

	This sample program is for the OP7200 series controllers.
	
	Description
	===========
	This program demonstrates reading a differential ADC channel
	when its jumpers JP4, JP5, JP6 or JP7 are connected to AGND.
	(Jumper is in the 2-3 position, which is the default position)


	Connections
	===========
	For this calibration procedure you will need two power supplies.

	NOTE: Before doing the following steps set the power supplies to
	      zero volts and then turn them OFF.
	
	1. Initially connect the positive side of power supply #1 to
	   the positive side to one of the following ADC differential
	   channel pairs.

	    Channel   DIFF Pairs
	    -------  ------------   
	      0 		 +AIN0  -AIN1    
	      2		 +AIN2  -AIN3    
	      4		 +AIN4  -AIN5    
	      6		 +AIN6  -AIN7    
	     	  
	2.	Connect connect the positive side of power supply #2 to
	   the negative side to one of the following ADC differential
	   channel pairs. (Same DIFF pair from step 1)  

	    Channel    DIFF Pairs
	    -------   ------------   
	      0		  +AIN0   -AIN1
	      2		  +AIN2   -AIN3
	      4		  +AIN4   -AIN5
	      6 		  +AIN6   -AIN7

	3.	Connect both power supply grounds to AGND on the controller.

		
	Instructions
	============
	1. Power-on the controller.
	2. Compile and run this program.
	3. Turn ON the power supplies for the ADC calibration.
	4. Follow the instructions as displayed.

***************************************************************************/
#class auto

const char vstr[][] = {
	"0 - 20V  ",
	"0 - 10V  ",
	"0 - 5V   ",
	"0 - 4V   ",
	"0 - 2.5V ",
	"0 - 2V   ",
	"0 - 1.25V",
	"0 - 1V   "
};

void printrange()
{
	printf("\ngain_code\tVoltage range\n");
	printf("---------\t-------------\n");
	printf("\t0\t0 - 20 \n");
	printf("\t1\t0 - 10\n");
	printf("\t2\t0 - 5\n");
	printf("\t3\t0 - 4\n");
	printf("\t4\t0 - 2.5\n");
	printf("\t5\t0 - 2\n");
	printf("\t6\t0 - 1.25\n");
	printf("\t7\t0 - 1\n\n");
}


void main ()
{
	auto int channel, gaincode;
	auto int rawdata;
	auto float voltage;
	auto int key, valid;
	auto int display_mode;
	auto char buffer[64];
	
	brdInit();
	while (1)
	{
		valid = FALSE;
		printf("\nchannel_code\tInputs\n");
		printf("------------\t-------------\n");
		printf("\t0\t+AIN0 -AIN1\n");
		printf("\t2\t+AIN2 -AIN3\n");
		printf("\t4\t+AIN4 -AIN5\n");
		printf("\t6\t+AIN6 -AIN7\n\n");
		printf("\nChoose the AD channel 0,2,4, or 6 = ");
		do
		{
			channel = getchar();
			switch(channel)
			{
				case '0':
				 valid = TRUE;
				 break;
				case '2':
				 valid = TRUE;
				 break;
				case '4':
				 valid = TRUE;
				 break;
				case '6':
				 valid = TRUE;
				 break;
			}
		} while (!valid);
		channel = channel - 0x30;
		printf("%d", channel);
		while(kbhit()) getchar();

		printf("\n");
		printrange();
		printf("Choose gain code (0-7) =  ");
		do
		{
			gaincode = getchar();
		} while (!( (gaincode >= '0') && (gaincode <= '7')) );
		gaincode = gaincode - 0x30;
		printf("%d\n\n", gaincode);
		while(kbhit()) getchar();

		printf("\n\n");
		printf("Display Mode:\n");
		printf("-------------\n");
		printf("1. Display raw data only\n");
		printf("2. Display voltage only\n");
		printf("3. Display both\n");
		printf("Select Mode (1-3) = ");
		do
		{
			display_mode = getchar();
		} while (!( (display_mode >= '1') && (display_mode <= '3')) );
		display_mode = display_mode - 0x30;
		printf("%d\n\n", display_mode);
		while(kbhit()) getchar();

		do 
		{
			switch (display_mode)
			{
				case 1: 
					rawdata = anaIn(channel, DIFF_MODE, gaincode);
					printf("CH%2d raw data %d\n", channel, rawdata);
					break;
				case 2:
					voltage = anaInDiff(channel, gaincode);
					printf("CH%2d is %.5f V\n", channel, voltage);
					break;
				case 3:
					rawdata  = anaIn(channel, DIFF_MODE, gaincode);
					voltage = anaInDiff(channel, gaincode);
					printf("CH%2d is %.5f V from raw data %d\n", channel, voltage, rawdata);
					break;
				default:
				break;
			}
			printf("\nPress ENTER key to read value again or 'Q' or read another channel\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();

		}while(key != 'q' && key != 'Q');
	}          
}
///////////////////////////////////////////////////////////////////////////

