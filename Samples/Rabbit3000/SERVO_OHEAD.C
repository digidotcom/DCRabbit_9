/**********************************************************
	servo_ohead.c
 	Z-World, 2002

	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	Description
	===========
	This program tries to estimate the overhead of running the
	servo control loop.
	
*************************************************************************/

#define SERVO_VERBOSE							// Print debug messages

/*--------- Copy recommended #define settings from execution of SERVO_FIRST.C ------------*/
/*                      Overwrite the following definitions                               */
#define NUM_SERVOS 1
#define SERVO_LOOP_RATE_HZ 1024                                                 
// Following setting allows use of 2048Hz periodic interrupt hook.              
// This frees up timer A1 and timer B resources.                                
#define SERVO_HOOK_PERIODIC 1                                                   
#define SERVO_IP_LEVEL 1                                                        
#define SERVO_PWM_FREQ 19000                                                    
#define SERVO_PWM_USE_0 0                                                       
#define SERVO_REVERSE_0  // High PWM duty gives decreasing count                
#define SERVO_COUNT_PER_REV_0 4800L                                             
#define SERVO_MAX_RPM_0 2000                                                    
#define SERVO_VS_UPSHIFT_0 6                                                    
#define SERVO_VS_DOWNSHIFT_0 0                                                  
#define SERVO_PWM_INIT_OFFS_0 381                                               
#define SERVO_PWM_INIT_RANGE_0 200                                              
#define SERVO_PWM_MAXDELTA_0 88                                              
#define SERVO_PWM_MINSTART_0 367                                                
#define SERVO_PWM_MAXSTART_0 396                                                

/*------------------------------- End of copy and paste ----------------------------------*/

#use "servo.lib"



main()
{
	auto word ms;
	auto long count;
	auto long count0;
	auto word icount;

	// Get a baseline CPU throughput before enabling ISRs.
	count0 = 0;
	ms = (word)MS_TIMER + 1000;
	// Following is our one-second sample workload.
	while ((int)((word)MS_TIMER - ms) < 0) {
		count0++;
	}
	
	servo_init();
	servo_set_coeffs(0, 680, 768, 102);
	_sp1.vcmd = 65536;	// Make it spin
	SERVO_ENABLE_0();

	// Do it again, with ISRs enabled.
	count = 0;
	ms = (word)MS_TIMER + 1000;
	icount = _sp1.ctl_count;
	while ((int)((word)MS_TIMER - ms) < 0) {
		count++;
	}
	icount = _sp1.ctl_count - icount;
	
	printf("Non-ISR work units = %ld per second\n", count0);
	printf("ISR work units     = %ld per second\n", count);
	printf("%%CPU used by ISRs  = %f\n", (count0 - count)*100.0/count0);
	printf("Loop rate          = %u Hz\n", icount);
	printf("Total updates/sec  = %u\n", icount * NUM_SERVOS);
	printf("usec per update    = %ld\n", (long)(1000000.0 * (count0 - count)/(count0*icount*NUM_SERVOS)));
	
	return 0;
}
		