/***************************************************************************
	sstar20ma.c
	
   Z-World, 2001
	This sample program is used with Smart Star products.

	This program demonstrates how to recalibrate an ADC channel using
	two known currents to generate two coefficients, gain and offset,
	which will be rewritten into the ADC's EEPROM. It will also
	continuously display the current that is being monitored.

	Instructions:

	1.Connect a power supply of 0-10 volts with a series 220 ohm resistor
	to one of the AIN channel 0 - 10 on the SmartStar board.


	----------------|									|---------------------------
						 |									| SmartStar 4-20ma board	
	Power supply	 | 220 ohm	 |-------|		| 
	0 - 10v		  + |-/\/\/\----|current|-----| AIN channels 0 - 10
	                |	1/4 watt  | meter |		|
	                |				 ---------		| 
	              - |-------------------------| AGND
					    |									|
	----------------|									|---------------------------

	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.
		Note:  When entering current values, milli-Amp values will produce
		a milli-Amp value. Similarly, Amp values will produce an Amp value.
	4. Vary voltage (0-10v) on power supply to see the CURRENT meter track
	whats being displayed by Dynamic C (4-20ma). 

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/
#class auto

#define NUM_SAMPLES	100	// Number of samples per data point for A/D calibration   

///// Displays the current from a A/D channel in the STDIO window 
void DispCurrentNormal( int slotnum, int inputnum )
{
	auto float currentequ;
	
	currentequ = anaInmAmps(ChanAddr(slotnum, inputnum));
	if (currentequ < 1.0)
		printf("Current at CH%d is %.5f A\n", inputnum, currentequ);
	else
		printf("Current at CH%d is %.4f mA\n", inputnum, currentequ);
}

//// Displays the current and a raw data value from a A/D channel in the STDIO window 
void DispCurrentRobust( int slotnum, int inputnum )
{
	auto unsigned int rawdata, chan;
	auto float currentequ;
	auto int channel;

	///// Calculate a calibrated current value
	channel = ChanAddr(slotnum, inputnum);
	chan = ((channel>>7) & 0x0007)*ADCHANNELS + (channel & 0x000F); 	// Calc offset into calibration table	
	rawdata = anaIn(channel);														// Get raw A/D data 
	currentequ = _adcCalib[chan][0] * (_adcCalib[chan][1] - rawdata); // Convert raw A/D data to a calibrated current

	// The above 4 lines of code is equivalent to the following function:
	// currentequ = anaInmAmps(ChanAddr(slotnum, inputnum));

	// Display the calibrated current and raw data value 
	if (currentequ < 1.0)
		printf("Current at CH%d is %.5f A from raw data %d\n", inputnum, currentequ, rawdata);
	else
		printf("Current at CH%d is %.4f mA from raw data %d\n", inputnum, currentequ, rawdata);
}


void main ()
{
	auto unsigned int rawdata, data1, data2, i;
	auto float locurrent, hicurrent;
	auto unsigned long avg;
	auto int inputnum, slotnum, msgcode;
	auto char buffer[64];

	brdInit();

	printf("Please enter ADC board slot position, 0 thru 6....");
	do
	{
		slotnum = getchar();
	}	while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	if (msgcode = anaInEERd(ChanAddr(slotnum, 0)))
	{
		printf("Error %d: eeprom unreadable or empty slot\n", msgcode);
		exit(0);
	}
	
	printf("Please enter an input channel, 0 thru A (10)....");
	do
	{
		inputnum = getchar();
	} 	while (!((inputnum >= '0') && (inputnum <= '9')) && (inputnum != 'a') && (inputnum != 'A'));
	if ((inputnum == 'a') || (inputnum == 'A'))
	{
		printf("channel 10 chosen.\n");
		inputnum=10;
	}
	else
	{
		printf("channel %d chosen.\n", inputnum-=0x30);
	}

	/////Get two data points using known currents
	printf("\n\nAdjust the current to approx. 5 mA and then ENTER the actual\n");
	printf("current being measured, (floating point value) = ");
	gets(buffer);
	locurrent = atof(buffer);
	for(i=0, avg=0; i<NUM_SAMPLES; i++) // Average the A/D reading
	{
		avg += anaIn(ChanAddr(slotnum, inputnum));
	}
	data1 = (unsigned int) (avg/NUM_SAMPLES);

	printf("\n\nAdjust the current to approx. 19 mA and ENTER the actual\n");
	printf("current being measured, (floating point value) = ");
	gets(buffer);
	hicurrent = atof(buffer);
	for(i=0, avg=0; i<NUM_SAMPLES; i++) // Average the A/D reading
	{
		avg += anaIn(ChanAddr(slotnum, inputnum));
	}
	data2 = (unsigned int) (avg/NUM_SAMPLES);

			
	/////Calculate gains and offsets
	if	(anaInCalib(ChanAddr(slotnum, inputnum), data1, locurrent, data2, hicurrent))
	{
		printf("Cannot make coefficients\n");
	}
	else
	{
		/////Store coefficients into eeprom
		while (anaInEEWr(ChanAddr(slotnum, inputnum)));
		printf("Wrote coefficients to eeprom\n");
		
		/////Read back coefficients from eeprom
		printf("Read coefficients from eeprom\n");
		if (msgcode = anaInEERd(ChanAddr(slotnum, inputnum)))
		{
			printf("Error %d: eeprom unreadable; channel %d\n", msgcode,inputnum);
			exit(0);
		}
		
		printf("Vary current on channel %d\n", inputnum);
		///// Display the current for a given channel
		while (1)
		{
			// Display the current 4 - 20ma 
			//DispCurrentNormal(slotnum, inputnum);

			// Display the raw data and the current 4 - 20ma
			DispCurrentRobust(slotnum, inputnum);
		}
	}
}	//end main


