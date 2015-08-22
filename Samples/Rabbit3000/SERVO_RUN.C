/**********************************************************
	servo_run.c
 	Z-World, 2002

	This program is used with RCM3000 series controllers
	with prototyping boards.

	It is recommended that you run servo_first.c before running
	this demo, since it will allow you to set the parameters
	correctly.

	----
	You can copy and paste the #defines which are
	written by servo_first into this and other demo programs.
	----
	
	Description
	===========
	This program implements a servo control mechanism.  The PWM0
	output is used to control the drive to the servomotor, and the
	QD0 input is used to read back the motor shaft position.

	It is assumed that you have a servo motor (not the RC type,
	but a free-running DC motor with reasonably high resolution
	shaft encoder).  If you don't have a real motor, you could
	electrically simulate one using some op-amps and a bit of
	digital (or analog) logic.

	PID control is used in a servo update loop (interrupt driven)
	to keep the encoder reading equal to the commanded position.
	Initially, only a weak proportional control is used.  Once basic
	control is established, you can experiment with the PID
	coefficients until the required degree of control is reached.

	This demo uses one or two servos.  The library supports up to
	2 independent servos, but both use the same loop update rate.

	Instructions
	============
	. Hook up the motor shaft encoder to PF0/PF1 (quadrature input).
	  The encoder is assumed to output I and Q signals at TTL levels
	  (3 or 5V).
	. Hook up the servo amplifier input to PF4 (PWM output).  The PWM
	  output may need to be filtered and offset so that zero output
	  becomes full reverse drive, 50/50 is neutral (zero drive) and
	  100% becomes full forward drive.  Forward drive means that the
	  I input leads the Q input, and the encoder counts "up".
	  -- if the motor goes in the wrong direction or is impossible to
	  stabilize, either swap the I/Q connections or the motor connections.
	  If you ran SERVO_FIRST.C, then the definitions printed will
	  automatically compensate for any reversed connections if pasted
	  into this demo.
	. Alter the #define settings to correspond with your servomotor/
	  encoder combination (or preferably use the output from SERVO_FIRST.C).
	. Compile and run this program and follow the on-screen instructions.
	  Generally, press keys (while the stdio window has the keyboard focus)
	  to get the motor to perform various tricks.

*************************************************************************/
#class auto

#define SERVO_VERBOSE							// Print debug messages

/*--------- Copy recommended #define settings from execution of SERVO_FIRST.C ------------*/
/*                      Overwrite the following definitions                               */
#define NUM_SERVOS 1                                                            
#define SERVO_LOOP_RATE_HZ 1024                                                 
#define SERVO_HOOK_PERIODIC 1                                                   
#define SERVO_IP_LEVEL 1                                                        
#define SERVO_PWM_FREQ 19000                                                    
#define SERVO_PWM_MODE 0                                                        
#define SERVO_PWM_INVERT                                                        
#define SERVO_VS_UPSHIFT 5                                                      
#define SERVO_VS_DOWNSHIFT 0                                                    
#define SERVO_REVERSE_0  // High PWM duty gives decreasing count                
#define SERVO_COUNT_PER_REV_0 4096L                                             
#define SERVO_MAX_RPM_0 4173L                                                   
#define SERVO_PWM_INIT_OFFS_0 513                                               
#define SERVO_PWM_INIT_RANGE_0 256                                              
#define SERVO_PWM_MAXDELTA_0 140                                                
#define SERVO_PWM_FAC_0 -4117                                                   
#define SERVO_PWM_MINSTART_0 4496                                               
#define SERVO_PWM_MAXSTART_0 199                                                
/*------------------------------- End of copy and paste ----------------------------------*/


#use "servo.lib"

void help(void)
{
	printf("Use keys:\n");
	printf("  ,.   Forward/back one rev (jump).\n");
	printf("  <>   Forward/back 10 revs (jump).\n");
	printf("  pP   Increase/decrease proportional coeff by 1/4.\n");
	printf("  iIdD Similar, for integral and differential coeffs.\n");
	printf("  xX   Increase/decrease all gain by 1/8.\n");
	printf("  []   Increase/decrease velocity by 10RPM.\n");
	printf("  {}   Increase/decrease velocity by 60RPM.\n");
	printf("  z    Halt.\n");
	printf("  gG   Step position 1/16 [1/4] rev and graph.\n");
	printf("  vV   Step velocity 10 [60] RPM and graph.\n");
	printf("  qQ   Smooth step 1/4 rev over 100 ticks [a=80/20].\n");
	printf("  f    Toggle current limiting.\n");
	printf("  t    Toggle controlled torque mode.\n");
#if NUM_SERVOS > 1
	printf("  r    Toggle gear mode.\n");
	printf("  s    Toggle servo to control.\n");
#endif
	printf("  h    Print this message.\n");
}

char s[80];


main()
{
	auto word ms;
	auto char c;
	auto int pr, graph;
	auto long x, step;
	auto word samples, nlines;
	auto int tmode, torque, gmode;
	auto int dut;
	auto servo_parms * sp;
	auto long scpr;

	tmode = 0;
	gmode = 0;
	dut = 0;
	sp = &_sp1;
	scpr = SERVO_COUNT_PER_REV_0;

	servo_init();
	servo_set_coeffs(0, 128, 500, 80);
#if NUM_SERVOS > 1
	servo_set_coeffs(1, 128, 500, 80);
#endif
	//servo_set_coeffs(0, 1316, 358, 67);
	//servo_set_coeffs(0, 10, 0, 0);

	help();
	
	SERVO_ENABLE_0();
	servo_alloc_table(0, 512);
#if NUM_SERVOS > 1
	SERVO_ENABLE_1();
	servo_alloc_table(1, 512);
#endif

	graph = 0;
	samples = 160;
	nlines = 48;

	for (;;)
	{
		_sp1.ctl_count = 0;
#if NUM_SERVOS > 1
		_sp2.ctl_count = 0;
#endif
		ms = (word)MS_TIMER + 1000;
		pr = 0;
		while ((int)((word)MS_TIMER - ms) < 0) {
			if (kbhit()) {
				pr = 1;
				c = getchar();
				if (c == '.')
					if (tmode)
						servo_torque(dut, torque += 100);
					else
						servo_set_pos(dut, sp->cmd + scpr, sp->vcmd);
				else if (c == ',')
					if (tmode)
						servo_torque(dut, torque -= 100);
					else
						servo_set_pos(dut, sp->cmd - scpr, sp->vcmd);
				else if (c == '>')
					if (tmode)
						servo_torque(dut, torque += 1000);
					else
						servo_set_pos(dut, sp->cmd + scpr * 10, sp->vcmd);
				else if (c == '<')
					if (tmode)
						servo_torque(dut, torque -= 1000);
					else
						servo_set_pos(dut, sp->cmd - scpr * 10, sp->vcmd);
				else if (c == ']')
					if (tmode)
						sp->pwm_fac = (int)(sp->pwm_fac * 1.05);
					else
						servo_set_vel(dut, sp->vcmd + servo_millirpm2vcmd(dut, 10000));
				else if (c == '[')
					if (tmode)
						sp->pwm_fac = (int)(sp->pwm_fac / 1.05);
					else
						servo_set_vel(dut, sp->vcmd - servo_millirpm2vcmd(dut, 10000));
				else if (c == '}')
					servo_set_vel(dut, sp->vcmd + servo_millirpm2vcmd(dut, 60000));
				else if (c == '{')
					servo_set_vel(dut, sp->vcmd - servo_millirpm2vcmd(dut, 60000));
				else if (c == 'p' && sp->prop < 24000)
					sp->prop = (int)(sp->prop * 1.25)+1;
				else if (c == 'P')
					sp->prop = (int)(sp->prop / 1.25);
				else if (c == 'i' && sp->integral < 24000)
					sp->integral = (int)(sp->integral * 1.25) + 1;
				else if (c == 'I')
					sp->integral = (int)(sp->integral / 1.25);
				else if (c == 'd' && sp->diff < 24000)
					sp->diff = (int)(sp->diff * 1.25) + 1;
				else if (c == 'D')
					sp->diff = (int)(sp->diff / 1.25);
				else if (c == 'x') {
					sp->prop = (int)(sp->prop * 1.125);
					sp->integral = (int)(sp->integral * 1.125);
					sp->diff = (int)(sp->diff * 1.125);
				}
				else if (c == 'X') {
					sp->prop = (int)(sp->prop / 1.125);
					sp->integral = (int)(sp->integral / 1.125);
					sp->diff = (int)(sp->diff / 1.125);
				}
				else if (c == 'z')
					servo_move_to(dut, sp->cmd, 1, 0, 0);
				else if (c == 'g') {
					servo_stats_reset(dut);
					servo_move_to(dut, sp->cmd + scpr/32, 1, 0, sp->vcmd);
					graph = 1;
				}
				else if (c == 'G') {
					servo_stats_reset(dut);
					servo_move_to(dut, sp->cmd + scpr/4, 1, 0, sp->vcmd);
					graph = 1;
				}
				else if (c == 'q') {
					servo_stats_reset(dut);
					servo_move_to(dut, sp->cmd + scpr/4, 100, 80, sp->vcmd);
					graph = 1;
				}
				else if (c == 'Q') {
					servo_stats_reset(dut);
					servo_move_to(dut, sp->cmd + scpr, 400, 320, sp->vcmd);
					graph = 1;
				}
				else if (c == 'v') {
					servo_stats_reset(dut);
					servo_move_to(dut, sp->cmd, 1, 0, sp->vcmd + servo_millirpm2vcmd(dut, step = 10000));
					step /= 1000;
					graph = 1;
				}
				else if (c == 'V') {
					servo_stats_reset(dut);
					servo_move_to(dut, sp->cmd, 1, 0, sp->vcmd + servo_millirpm2vcmd(dut, step = 60000));
					step /= 1000;
					graph = 1;
				}
				else if (c == 'f') {
					if (!sp->pwm_fac) {
						printf("Current limit ON\n");
						sp->pwm_fac = SERVO_PWM_FAC_0;
					}
					else {
						/* Removed, for safety! - NOT */
						printf("Current limit OFF\n");
						sp->pwm_fac = 0;
						sp->pwm_low = sp->pwm_abslow;
						sp->pwm_high = sp->pwm_abshigh;
					}
				}
				else if (c == 't') {
					tmode = !tmode;
					if (tmode) {
						printf("Torque mode ON\n");
						torque = 0;
						servo_torque(dut, 0);
					}
					else {
						printf("Torque mode OFF\n");
						servo_closedloop(dut, 1);
					}
				}
#if NUM_SERVOS > 1
				else if (c == 'r') {
					gmode = !gmode;
					if (gmode) {
						printf("Gear mode ON\n");
						servo_gear(200,1,1,1);
					}
					else {
						printf("Gear mode OFF\n");
						servo_gear(0,0,0,0);
					}
				}
				else if (c == 's') {
					dut = !dut;
					printf("Now controlling servo #%d\n", dut);
					if (dut) {
						sp = &_sp2;
						scpr = SERVO_COUNT_PER_REV_1;
					}
					else {
						sp = &_sp1;
						scpr = SERVO_COUNT_PER_REV_0;
					}
				}
#endif
				else if (c == 'h')
					help();
			}
		};
		if (pr) {
			printf("-----------------\n");
			printf("int/sec=%u  QD%d=%ld  pwm_int=%d  pwm_out=%u  vel=%d  pwm_nl=%d\n",
				 sp->ctl_count, dut,
#if NUM_SERVOS > 1
				 dut ? servo_qd_read_1() : servo_qd_read_0(),
#else
				servo_qd_read_0(),
#endif
				 sp->pwm_int, sp->pwm_out, sp->vel, sp->pwm_nl);
			printf("pwm_low=%u  pwm_high=%u  abslow=%u  abshigh=%u  maxdelta=%u\n",
				 sp->pwm_low, sp->pwm_high, sp->pwm_abslow, sp->pwm_abshigh, sp->pwm_maxdelta);
			printf("cmd=%ld  vcmd=%ld\n", sp->cmd, sp->vcmd);
			printf("error=%d  ierror=%d  derror=%d  max_error=%d  aw=%d\n",
		  		sp->error, sp->ierror, sp->derror, sp->max_error, sp->antiwindup);
			printf("P=%d  I=%d  D=%d\n", sp->prop, sp->integral, sp->diff);
			//printf("pwm_int = %d  pwm_offs = %u  pwm_low = %u  pwm_high = %u\n",
		  	//	sp->pwm_int, sp->pwm_offs, sp->pwm_low, sp->pwm_high);
		}
		if (graph && sp->table_offs == sp->table_ent) {
			graph = 0;
			printf("\n");
			printf("Response graph: step command = %ld\n", step);
			servo_graph(dut, 0, nlines, samples, 0, -50, 25);
		}
	}
	
	return 0;
}
		