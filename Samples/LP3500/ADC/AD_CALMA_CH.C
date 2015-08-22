/***************************************************************************
	ad_calma_ch.c

   Z-World, 2002
	This sample program is used with LP3500 series controllers

	Description
	===========
	This program demonstrates how to recalibrate an ADC milli-Amp channel using
	two known currents to generate two coefficients, gain and offset,
	which will be rewritten into to. It will also continuously display
	the current that is being monitored.

	Instructions
	============
	1. Jumpers must be installed on J3 for each input to be read.

	2.Connect a power supply of 0-3 volts with a series 0 ohm resistor
	to one of the AIN channel 0 - 3 on the controller


	----------------|									|---------------------------
						 |									| 4-20ma board
	Power supply	 |  100 ohm	 |-------|		|
	0 - 4v		  + |-/\/\/\----|current|-----| AIN channels 0 - 3
	                |	1/4 watt  | meter |		|
	                |				 ---------		|
	              - |-------------------------| AGND
					    |									|
	----------------|									|---------------------------

	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Vary voltage (0-4v) on power supply to see the CURRENT meter track
	what is displayed by Dynamic C (4-20ma).

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/
#class auto					/* Change local var storage default to "auto" */

#define STARTCHAN	0
#define ENDCHAN 3
#define GAINCODE GAIN_10   //milli-Amp multiplier for 0-2V range


void main()
{
	auto unsigned int rawdata, data1, data2;
	auto float locurrent, hicurrent;
	auto float currentequ;
	auto int inputnum;
	auto char buffer[64];

	brdInit();

	while (1)
	{
		printf("\nChoose the AD mAmp channel %d to %d .... ", STARTCHAN, ENDCHAN);
		gets(buffer);
		inputnum = atoi(buffer);

		/////Get two data points using known currents
		printf("\n\nAdjust the current to approx. 5.0 mA and then ENTER the actual\n");
		printf("current being measured, (floating point value) = ");
		gets(buffer);
		locurrent = atof(buffer);
		data1 = anaIn(inputnum, mAMP, GAINCODE);
		printf("data1 = %d\n", data1);

		printf("\n\nAdjust the current to approx. 19.0 mA and ENTER the actual\n");
		printf("current being measured, (floating point value) = ");
		gets(buffer);
		hicurrent = atof(buffer);
		data2 = anaIn(inputnum, mAMP, GAINCODE);
		printf("data2 = %d\n", data2);

		/////Calculate gains and offsets
		anaInCalib(inputnum, mAMP, GAINCODE, data1, locurrent, data2, hicurrent);

		/////Store coefficients into eeprom
		anaInEEWr(inputnum, mAMP, GAINCODE);
		printf("Wrote coefficients to eeprom\n");

		/////Read back coefficients from eeprom
		anaInEERd(inputnum, mAMP, GAINCODE);
		printf("Read coefficients from eeprom\n");

		printf("Vary current on channel %d\n", inputnum);
		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			currentequ = anaInmAmps(inputnum);
			printf("Current at CH%d is %.2fma\n", inputnum, currentequ);
			printf("\npress enter key to read value again or 'Q' to calibrate another channel\n\n");
			gets(buffer);
		}
	}

}	//end main


