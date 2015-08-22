/**********************************************************
	servo_first.c
 	Z-World, 2002

	This program is used with RCM3000 series controllers
	with prototyping boards.

	--------------------------------
	Please read the following instructions COMPLETELY before
	running this with a real servo motor.
	-------------------------------------------------------

	It allows you to experiment with the servo motor in
	OPEN LOOP mode i.e. without any feedback control.  This is
	a necessary prelude to the other servo demo programs,
	since it allows you to determine some of the motor
	control parameters, which should be fed into the other
	programs to get best (and safest) performance.

	It is assumed that you are running this on a Rabbit 3000
	processor with the same clock speed as the processor for
	the final application.
	
	Description
	===========
	This program implements a servo control mechanism.  The PWM0
	output is used to control the drive to the servomotor, and the
	QD0 input is used to read back the motor shaft position.

	You can also set up this program to operate two servos
	simultaneously.

	It is assumed that you have a servo motor (not the RC type,
	but a free-running DC motor with reasonably high resolution
	shaft encoder).  If you don't have a real motor, you could
	electrically simulate one using some op-amps and a bit of
	digital (or analog) logic.

	Important: the motor is assumed to settle to a constant
	velocity given a constant PWM output duty cycle.  This is
	the usual case for voltage control.  If the servo amplifier
	implements current control, then this demo is inappropriate
	and will probably not work.

	The motor(s) should be allowed to spin freely, with minimal
	loading.  Preferably, it should be isolated on a test bench.
	Minimal loading is important because it allows the most
	accurate estimates of power supply current requirements.

	-----------------------------------------------------------
	CAUTION: unrestrained motors may move violently!  Please
	ensure it is restrained, and preferably limit the current
	drive to low levels.
	-----------------------------------------------------------

	Note: the main control loop operates using interrupts from
	timer B, and also uses timer A1 as the prescaler for timer B.
	
	Instructions
	============
	. Alter the #define settings to correspond with your servomotor/
	  encoder combination(s).  Some of these settings are used by this
	  demo only, to help estimate motor performance.

	  The most important macros are WANT_NUM_SERVOS, which determines
	  whether only one, or both, servo drives are to be used; and
	  SERVO_PWM_MODE, which determines the type of control signals
	  which are required (See Technical Note 233 for details).

	. Hook up the 1st motor shaft encoder to PF0/PF1 (quadrature input).
	  The encoder is assumed to output I and Q signals at TTL levels
	  (3 or 5V).
	  
	. Optionally hook up the second motor shaft encoder to PF2/PF3.
	  - do this if you set NUM_SERVOS to 2.
	
	. Hook up the first servo amplifier to the PF4, PF5 and optionally
	  PG6 pins.  You must make connections which are appropriate to
	  the PWM mode which is selected.  See comment for SERVO_PWM_MODE
	  for details.
	  
	. Optionally hook up the second servo amplifier to PF6, PF7 and
	  possibly PG7. - do this if you set NUM_SERVOS to 2.
	  
	. Compile and run this program and follow the on-screen instructions.
	  You may be requested to take some measurements (armature voltage).
	  This is not absolutely necessary, but allows several useful
	  motor parameters to be determined.

*************************************************************************/
#class auto
#define SERVO_VERBOSE							// Print debug messages

#define WANT_NUM_SERVOS				1			// Number of servos in target application (1 or 2).
														// This only affects the final recommendations.

#define SERVO_PWM_MODE				0			// Set to one of:
														//   0 : locked antiphase -
														//       Servo 0: will use PF4 for PWM, PF5 for enable
														//       Servo 1: will use PF6 for PWM, PF7 for enable
														//   1 : direction/magnitude single PWM
														//       Servo 0: will use PF4 for PWM, PF5 for direction signal,
														//         PG6 for enable
														//       Servo 1: will use PF6 for PWM, PF7 for direction signal,
														//         PG7 for enable
														//   2 : direction/magnitude dual PWM
														//       Servo 0: will use PF4 for forward PWM, PF5 for reverse PWM,
														//         PG6 for enable
														//       Servo 1: will use PF6 for forward PWM, PF7 for reverse PWM,
														//         PG7 for enable

#define SERVO_PWM_INVERT						// Define this if PWM output is to be inverted (only used for
														// direction/magnitude modes).
														
//#define SERVO_PWM_SWAP							// Define if PF5/PF7 are to be used as primary PWM outputs
														// otherwise (default) PF4/PF6 are used.
														// In PWM mode 2 only, the other PWMs are used as the secondary
														// of the PWM pair.

#define SERVO_LOOP_RATE_HZ			1024		// Set to the desired loop update rate for your application.
														// The closest possible value will be printed out.

#define SERVO_PWM_FREQ				19000		// PWM frequency for PWM output(s).  If over 3kHz, pulse
														// spreader automatically enabled.


/*
 * The following values depend on your encoder hardware and motor parameters.
 * Fill them in as accurately as you can, or make exactly 0.0 if not known.
 * The macros starting with "SERVO_" are used by
 * the library.  Other macros are for this demo only.
 */														
#define SERVO_COUNT_PER_REV_0		4096L		// You MUST put in at least a non-zero guess for this.
														// Must be a long (L) constant.
#define SERVO_COUNT_PER_REV_1		4096L		// Only required if running 2 servos.

#define ARMATURE_RESISTANCE_0		2.0		// Motor armature resistance (ohms, as float value)
#define ARMATURE_RESISTANCE_1		3.0		// Only required if running 2 servos.

#define CURRENT_LIMIT_0				5.5		// Maximum permissible armature current under operating condition
														// (Amperes, as float value).
#define CURRENT_LIMIT_1				5.5		// Only required if running 2 servos.

#define ARMATURE_INDUCTANCE_0		2.7		// Armature inductance (milliHenrys, as float value)
#define ARMATURE_INDUCTANCE_1		2.8		// Only required if running 2 servos.

#define ALLOW_NONLINEARITY			0.05		// Declare limits of PWM output when nonlinearity of RPM vs pwm
														// exceeds this value (used for both servos).														


#define NUM_SERVOS					2			// Do not change this!  2 required for test.
#use "servo.lib"


int dut;			// Device under test: 0 or 1.

void stabilize(word ms, int quiet)
{
	word t;
	if (!quiet)
		printf("Waiting %ums to settle (hit 'q' to cut short)...", ms);
	t = (word)MS_TIMER + ms;
	while ((int)((word)MS_TIMER - t) < 0)
		if (kbhit() && getchar() == 'q')
			break;
	if (!quiet)
		printf("OK\n");
}

float velocity(word ms, int quiet)
{
	word t, u;
	long R1, R2;
	float rpm;
	
	if (!quiet)
		printf("Reading average velocity over %ums\n(hit 'q' to cut short)...", ms);
	t = (u = (word)MS_TIMER) + ms;
	R1 = dut ? servo_qd_read_1() : servo_qd_read_0();
	while ((int)((word)MS_TIMER - t) < 0)
		if (kbhit() && getchar() == 'q')
			break;
	t = (word)MS_TIMER - u;
	if (!t) t = 1;
	R2 = dut ? servo_qd_read_1() : servo_qd_read_0();
	rpm = (float)(R2 - R1) / ((float)SERVO_COUNT_PER_REV_0*t) * 60000;
	if (!quiet)
		printf("OK, RPM = %.2f\n", rpm);
	return rpm;	
}

void accelerate_to(int pwm, word ms, int quiet)
{
	word t, u;
	float v;
	int ipwm;
	if (!quiet)
		printf("Changing PWM setting to %d over %ums\n(hit 'q' to cut short)...\n", pwm, ms);
	ipwm = dut ? _sp2.pwm_out : _sp1.pwm_out;
	t = (u = (word)MS_TIMER) + ms;
	while ((int)((word)MS_TIMER - t) < 0) {
		if (kbhit() && getchar() == 'q')
			break;
		v = (float)((word)MS_TIMER - u) / ms;
		servo_openloop(dut, (word)(ipwm + v * (pwm - ipwm)));
	}
	servo_openloop(dut, pwm);
	if (!quiet)
		printf("OK\n");
}


char s[80];

float ask_voltage(void)
{
	printf("\nMeasure average motor voltage and enter value  (enter 0 if not known)\n");
	gets(s);
	return atof(s);
}

long get_clkspd(void)
{
	auto word ms;
	auto long count;
	
	printf("\nMeasuring CPU clock rate...");
	
	// Count clock cycles while waiting for 1 second to pass.
	count = 0;
	ms = (word)MS_TIMER + 1000;
	while ((int)((word)MS_TIMER - ms) < 0) {
		count++;
	}
	count *= 485;	// Fudge factor to convert to Hz.
	printf("done (%ld)\n", count);
	return count;
}

word loop_rate_hz;

word get_hook_divisor(void)
{
	// If desired rate is within 10% of 2048Hz/(2*NUM_SERVOS) then we
	// suggest hooking to the periodic interrupt instead of timers A1/B
	// This saves timer resources as well as slightly reducing
	// ISR overhead.
	word irate;
	word divisor;

	irate = 2048 / (2*WANT_NUM_SERVOS);
	divisor = (word)((float)irate/loop_rate_hz + 0.5);
	if (loop_rate_hz * divisor > irate - 160 && loop_rate_hz * divisor < irate + 200) {
		loop_rate_hz = irate / divisor;
		return divisor;
	}
	return 0;
}

word get_a1_divisor(long clkspd)
{
	// Find timer A1 divisor to give closest loop update rate to that requested.
	// Assumes that timer B is cascaded from A1.  The update rate (per servo) will
	// be clkspd / 1024 / 2 / (A1+1) / (2 * NUM_SERVOS)
	int a1;

	a1 = (int)(clkspd / (4096.0 * NUM_SERVOS * loop_rate_hz) + 0.5) - 1;
	if (a1 < 0)
		a1 = 0;
	if (a1 > 255)
		a1 = 255;
	loop_rate_hz = (word)(clkspd / (4096L * NUM_SERVOS * (a1+1)));
	return a1;
}



main()
{
	auto word ms, hook_periodic, a1_divisor, upshift, maxvel;
	auto long clkspd, scpr;
	auto float lovolt, hivolt, zvolt;
	auto float lovel, hivel, zvel, v, xv, minrpm, maxrpm;
	auto float pwmperRPM, voltperRPM, voltperpwm, pwm_offs, pwm_yintercept, volt_yintercept, vmax, velperpwm;
	auto float R, I, L;
	auto float kt, kte, akte, inl, radps;
	auto int pwm_range, zpwm, gotvolt, ipwm;
	auto word p, minpwm, maxpwm, startminpwm, startmaxpwm;

	loop_rate_hz = SERVO_LOOP_RATE_HZ;
	dut = 0;
	clkspd = get_clkspd();	// Do this before servo ISRs installed!
	
	servo_init();

	printf("Servo motor parameter determination program\n");
	printf("-------------------------------------------\n");
	hook_periodic = get_hook_divisor();
	if (!hook_periodic)
		a1_divisor = get_a1_divisor(clkspd);

_next_motor:
	printf("\nPress enter to enable servo #%d and proceed.\n", dut+1);
	printf("...or press 'x <enter>' if you have motor datasheet parameters.\n");
_restart:
	gets(s);
	if (toupper(s[0]) == 'X') {
		// Get datasheet parameters as well as performing empirical test
		printf("Enter motor torque constant in Newton-meters per Ampere.\n");
		printf("(This is the same as the back-EMF constant in Volt-seconds per radian):\n");
		gets(s);
		kt = atof(s);
	}
	else
		kt = 0;	// unknown from datasheet

	printf("Enabling and setting output drive to center (512).\n");
	servo_openloop(dut, 512);
	dut ? SERVO_ENABLE_1() : SERVO_ENABLE_0();

	accelerate_to(512+256, 3000, 0);
	stabilize(2000, 0);
	hivel = velocity(500, 0);
	hivolt = ask_voltage();

	accelerate_to(512-256, 6000, 0);
	stabilize(2000, 0);
	lovel = velocity(500, 0);
	lovolt = ask_voltage();

	if (fabs(hivel - lovel) < 1.0) {
		dut ? SERVO_DISABLE_1() : SERVO_DISABLE_0();
		printf("Velocity difference seems small or non-existent (< 1 RPM).\n");
		printf("Maybe the encoder or motor drive is not connected?\n");
		printf("Press enter to restart.\n");
		gets(s);
		goto _restart;
	}

	pwmperRPM = 512 / (hivel - lovel);
	voltperpwm = (hivolt - lovolt) / 512;
	voltperRPM = (hivolt - lovolt) / (hivel - lovel);
	pwm_yintercept = lovel - 256 / pwmperRPM;
	volt_yintercept = lovolt - 256 * voltperpwm;
	pwm_offs = -pwm_yintercept * pwmperRPM;
	ipwm = (int)pwm_offs;

	zpwm = ipwm > 0 && ipwm < 1024;
	if (zpwm) {
		accelerate_to(ipwm, 3000, 1);	// Stop the motor
		stabilize(2000, 1);
		zvel = velocity(500, 1);
		printf("Velocity at interpolated neutral pwm (%d):\n", ipwm);
		printf("  RPM:  %f\n", zvel);

		if (fabs(zvel) < 1.0) {
			// Good, motor stopped (or nearly stopped) when expected.
			printf("\nFinding start in forward direction...\n");
			for (p = ipwm - 1; p > 1; p--) {
				accelerate_to(p, 5, 1);
				stabilize(50, 1);
				v = velocity(50, 1);
				// Continue until over 1 RPM
				if (fabs(v) > 1.0)
					break;
			}
			startminpwm = p;
			printf("  ... Starting minimum pwm = %u\n", p);
			accelerate_to(ipwm, 300, 1);
			printf("\nFinding start in reverse direction...\n");
			for (p = ipwm + 1; p < 1024; p++) {
				accelerate_to(p, 5, 1);
				stabilize(50, 1);
				v = velocity(50, 1);
				// Continue until over 1 RPM
				if (fabs(v) > 1.0)
					break;
			}
			startmaxpwm = p;
			printf("  ... Starting maximum pwm = %u\n", p);
			printf("  ... Dead zone from %u to %u\n", startminpwm+1, startmaxpwm-1);
		}

		printf("\nFinding control limit in forward direction...\n");
		accelerate_to(256, 3000, 1);
		for (p = 255; p > 1; p--) {
			accelerate_to(p, 5, 1);
			stabilize(25, 1);
			v = velocity(25, 1);
			xv = pwm_yintercept + p / pwmperRPM;
			if (fabs(xv) < 1.0)
				continue;
			// Continue until specified nonlinearity
			if (v / xv < (1.0 - ALLOW_NONLINEARITY) || v / xv > (1.0 + ALLOW_NONLINEARITY))
				break;
		}
		minpwm = p;
		printf("  ... Minimum pwm = %u\n", p);
		
		printf("\nFinding control limit in reverse direction...\n");
		accelerate_to(768, 6000, 1);
		for (p = 769; p < 1024; p++) {
			accelerate_to(p, 5, 1);
			stabilize(25, 1);
			v = velocity(25, 1);
			xv = pwm_yintercept + p / pwmperRPM;
			if (fabs(xv) < 1.0)
				continue;
			// Continue until specified nonlinearity
			if (v / xv < (1.0 - ALLOW_NONLINEARITY) || v / xv > (1.0 + ALLOW_NONLINEARITY))
				break;
		}
		maxpwm = p;
		printf("  ... Maximum pwm = %u\n", p);

		accelerate_to(ipwm, 6000, 1);
	}

	dut ? SERVO_DISABLE_1() : SERVO_DISABLE_0();

	gotvolt = hivolt != lovolt;
	if (dut) { R = ARMATURE_RESISTANCE_1; I = CURRENT_LIMIT_1; L = ARMATURE_INDUCTANCE_1; }
	else { R = ARMATURE_RESISTANCE_0; I = CURRENT_LIMIT_0; L = ARMATURE_INDUCTANCE_0; }
	vmax = R * I;
	
	// Empirically determined torque constant
	if (gotvolt) {
		kte = voltperRPM * (60.0 / (2 * 3.14159));
		akte = fabs(kte);
		if (kt != 0.0) {
			// Compare with datasheet value to determine running friction.
			if (kt > akte)
				printf("Datasheet torque constant did not agree with measured value.  Using measured value.\n");
			else {
				// The RMP "drop" is used to deduce the no-load current at 1/2 max speed.
				radps = fabs(hivel - lovel) * (3.14159 / 60.0);
				inl = ((akte - kt) * radps) / R;
				printf("\nNo-load current at %f rad/sec = %fA\n", radps, inl);
				printf("  ... friction load torque = %fNm\n", inl * kt);

				// Use the datasheet value for computing the rest of the data
				hivel *= (akte / kt);
				lovel *= (akte / kt);
				kte *= (kt / akte);
				pwmperRPM = 512 / (hivel - lovel);
				voltperRPM = (hivolt - lovolt) / (hivel - lovel);

			}
		}
	}

	printf("\nResults and recommendations:\n");
	printf(  "----------------------------\n");
	if (gotvolt) {
		printf("RPM/Volt = %f\n", 1.0 / voltperRPM);
		printf("  ... Volt-seconds/radian = %f\n", kte);
		printf("  ...                     = Newton-meters/Ampere\n");
		printf("Volt/pwm-step = %f\n", voltperpwm);
		printf("  ... Voltage at 0 pwm    = %f\n", volt_yintercept);
		printf("  ... Voltage at 1024 pwm = %f\n", volt_yintercept + voltperpwm * 1024);
	}
	printf("RPM/pwm-step  = %f\n", 1.0 / pwmperRPM);
	printf("  ...     RPM at 0 pwm    = %f\n", minrpm = pwm_yintercept);
	printf("  ...     RPM at 1024 pwm = %f\n", maxrpm = pwm_yintercept + 1024 / pwmperRPM);
	minrpm = fabs(minrpm);
	maxrpm = fabs(maxrpm);
	if (minrpm > maxrpm)
		maxrpm = minrpm;
	if (maxrpm < (dut ? SERVO_MAX_RPM_1 : SERVO_MAX_RPM_0))
		maxrpm = (dut ? SERVO_MAX_RPM_1 : SERVO_MAX_RPM_0);
	printf("pwm for zero RPM = %d\n", ipwm);
	if (!zpwm) {
		printf("  ... this seems to be a unidirectional application!\n");
	}
	scpr = dut ? SERVO_COUNT_PER_REV_1 : SERVO_COUNT_PER_REV_0;
	maxvel = (word)((float)maxrpm * scpr / (60.0 * loop_rate_hz));
	velperpwm = scpr / (60.0 * pwmperRPM * loop_rate_hz);
	upshift = 0;
	while (upshift < 6 && maxvel < 8192 >> upshift) {
		upshift++;
		velperpwm *= 2.0;
	}

	if (vmax > 0.0 && gotvolt) {
		printf("Max voltage diff to remain within current limit of %fA:\n", I);
		printf("                    = %f\n", vmax);
		printf("  ... max pwm-diff  = %f\n", vmax / voltperpwm);
	}
	if (R > 0 && L > 0)
		printf("Winding L/R time constant = %fms\n", L/R);
	
	printf("\n/* Recommended definitions for servo #%d.  Copy and paste into following samples... */\n", dut+1);
	if (!dut) {
		printf("#define NUM_SERVOS %u\n", (word)WANT_NUM_SERVOS);
		printf("#define SERVO_LOOP_RATE_HZ %u\n", loop_rate_hz);
		if (hook_periodic)
			printf("#define SERVO_HOOK_PERIODIC %d\n", hook_periodic);
		else
			printf("#define SERVO_A1_DIVISOR %d\n", a1_divisor);
		printf("#define SERVO_IP_LEVEL %u\n", (word)SERVO_IP_LEVEL);
		printf("#define SERVO_PWM_FREQ %u\n", (word)SERVO_PWM_FREQ);
		printf("#define SERVO_PWM_MODE %u\n", (word)SERVO_PWM_MODE);
#ifdef SERVO_PWM_OPENDRAIN
		printf("#define SERVO_PWM_OPENDRAIN\n");
#endif
#ifdef SERVO_PWM_INVERT
		printf("#define SERVO_PWM_INVERT\n");
#endif
#ifdef SERVO_PWM_SWAP
		printf("#define SERVO_PWM_SWAP\n");
#endif
		printf("#define SERVO_VS_UPSHIFT %u\n", upshift);
		printf("#define SERVO_VS_DOWNSHIFT 0\n");
	}
	if (pwmperRPM > 0.0)
		printf("#define SERVO_FORWARD_%d  // High PWM duty gives increasing count\n", dut);
	else
		printf("#define SERVO_REVERSE_%d  // High PWM duty gives decreasing count\n", dut);
	printf("#define SERVO_COUNT_PER_REV_%d %ldL\n", dut, (long)scpr);
	printf("#define SERVO_MAX_RPM_%d %luL\n", dut, (unsigned long)maxrpm);
	if (zpwm) {
		printf("#define SERVO_PWM_INIT_OFFS_%d %u\n", dut, ipwm);
		pwm_range = ipwm - 1;
		if (pwm_range >= ipwm - minpwm)
			pwm_range = ipwm - minpwm - 1;
		if (pwm_range >= maxpwm - ipwm)
			pwm_range = maxpwm - ipwm - 1;
		if (ipwm + pwm_range >= 1024)
			pwm_range = 1023 - ipwm;
		printf("#define SERVO_PWM_INIT_RANGE_%d %u\n", dut, pwm_range);
		if (vmax > 0.0 && gotvolt) {
			printf("#define SERVO_PWM_MAXDELTA_%d %u\n", dut, (word)fabs(floor(vmax / voltperpwm)));
			printf("#define SERVO_PWM_FAC_%d %d\n", dut, (int)(65536.0 / velperpwm));
		}
		printf("#define SERVO_PWM_MINSTART_%d %u\n", dut, startminpwm);
		printf("#define SERVO_PWM_MAXSTART_%d %u\n", dut, startmaxpwm);
	}
	else
		printf("// Cannot make recommendation for SERVO_PWM_INIT_OFFS_%d\n", dut);
	printf("/* End of copy and paste section */\n");

	dut = !dut;
	printf("\n\nRun test for servo #%d? (y/n)\n", dut+1);
	gets(s);
	if (*s == 'y' || *s == 'Y')
		goto _next_motor;
	while (1);
	return 0;
}
		