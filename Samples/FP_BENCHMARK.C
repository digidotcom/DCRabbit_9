/*****************************************************

     samples\fp_benchmark.c
     Z-World, 2000

     Demonstration of computation times for various functions
     (nodebug generates faster code for some tests)


     This program will time various functions and print out the timing results.
   Board Clock Speed  18.432001 MHz  ("static" vars)
empty loop                     2 microseconds
float multiply                23 microseconds
float add                     20 usec
float divide                  46 usec
float square root             53 usec
float sine                   175 usec
float exp                    156 usec
float log                    236 usec
float arccos                 437 usec
float arctan (atan2)         379 usec
empty costatement             12 usec
empty cofunction              53 usec

 Board Clock Speed  22.118399 MHz  ("static" vars)
empty loop                     2 microseconds
float multiply                19 microseconds
float add                     15 usec
float divide                  38 usec
float square root             43 usec
float sine                   145 usec
float exp                    126 usec
float log                    190 usec
float arccos                 361 usec
float arctan (atan2)         311 usec
empty costatement             11 usec
empty cofunction              43 usec

 Board Clock Speed  22.118399 MHz  ("auto" vars)
empty loop                     2 microseconds
float multiply                20 microseconds
float add                     15 usec
float divide                  39 usec
float square root             43 usec
float sine                   146 usec
float exp                    126 usec
float log                    190 usec
float arccos                 359 usec
float arctan (atan2)         310 usec
empty costatement             10 usec
empty cofunction              43 usec

******************************************************/
#class auto
int rundemo();
cofunc void nullcof(void)
{
	// empty cofunction for cofunction timing test
}

void main()
{
	rundemo();
}

nodebug
rundemo()
{
	unsigned long int	timer, emptyloop, emptycostate;
	unsigned int	j,dif;
	float				x, y, z;

	// compute clock speed
	x=19200.*32.*freq_divider/1000000.; // clock speed in mhz
	printf("\n\n Board Clock Speed %10.6f MHz\n",x);


	// compute time for an empty loop
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<1000u; j++) {
		// empty loop
	}
	emptyloop = MS_TIMER-timer;				// save this value to adjust later measurements
	printf("empty loop\t\t%8ld microseconds\n", MS_TIMER-timer); // print time in microseconds


	// chose some values to use in next few calculations
	x=5.68; y=34567.99; z=-2345.67;

	// time float multiplication
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
   	x*y;
		z*x;
	}

	printf("float multiply\t\t%8ld microseconds\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time float addition
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		x+y;
		z+x;
	}
	printf("float add\t\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time float division
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		x/y;
		z/x;
	}
	printf("float divide\t\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time sqrt function
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		sqrt(x);
		sqrt(y);
	}
	printf("float square root\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time sine function
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		sin(0.34);
		sin(-3.0);
	}
	printf("float sine\t\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time exp function
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		exp(5.0);
		exp(0.01);
	}
	printf("float exp\t\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time log function
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		log(5.0);
		log(0.01);
	}
	printf("float log\t\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds



	// time arccosine function
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		acos(0.3567);
		acos(-0.89);
	}
	printf("float arccos\t\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time arctangent function
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<500u; j++) {
		atan2(0.5,-0.3444);
		atan2(0.60888,0.2341);
	}
	printf("float arctan (atan2)\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time empty costatement
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<1000u; j++) {
		costate{}
	}
	emptycostate = MS_TIMER - timer - emptyloop;			// save this value to adjust next measurement
	printf("empty costatement\t%8ld usec\n", MS_TIMER-timer-emptyloop); // print time in microseconds


	// time empty cofunction
	timer = MS_TIMER;   // get current time in milliseconds
	for(j=0; j<1000u; j++) {
		costate {
			waitfordone{ nullcof(); }
		}
	}
	printf("empty cofunction\t%8ld usec\n", MS_TIMER-timer-emptyloop-emptycostate); // print time in microseconds


}  // done with program