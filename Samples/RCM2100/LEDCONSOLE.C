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
/*******************************************************************************

        Samples\RCM2100\LEDConsole.c

        A basic LED controller interfaces to the ZConsole library.

        The user can type commands into the ZConsole to control the two
        LED's on the COREMODULE protoboard.  Each LED can be either on,
        off, or pulse-width-modulated.  t each change, the LED states
        are written to FLASH, where they are read in the next time the
        board restarts.

        Since the console action handler can be called from either the
        serial port or TELNET consolestate's, they must be re-entrant.
        Dynamic C ses storage class "static" by default, so all local
        vars are explicitly "auto".

        The LED's on are port A, bits 0 (DS2) and 1 (DS3).

        This program is based on "samples\RCM2100\EthCore1.c",
        "samples/ZConsole/basicconsole1.c" and "samples/tcpip/ssi.c" samples.

*******************************************************************************/
#class auto

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 *
 * It is best to explicitly turn off DHCP in your configuration.  This
 * will allow the console to completely manage DHCP, so that it does
 * not acquire the lease at startup in sock_init() when it may not need
 * to (if the user has not turned on DHCP).
 */
#define TCPCONFIG 1


/*  This file uses SER-C (serial port on internal I/O port C) as
 *  a place where CONSOLE can realize itself.  THIS REQUIRES THE
 *  RSR-232 AREA OF THE PROTOBOARAD BE INSTALLED, WHICH IS NOT
 *  THE DEFAULT.  If you don't have these parts installed, then
 *  comment out the line that looks like:
 *   		CONSOLE_IO_SERC(57600),
 */

/*  Define 1/0 to include/exclude support on serial port C: */
#define  DEMO_HAS_SERB	1

#if DEMO_HAS_SERB
#define BINBUFSIZE		255
#define BOUTBUFSIZE		255
#endif
//#define AINBUFSIZE		255
//#define AOUTBUFSIZE		255


#if DEMO_HAS_SERB
#define NUM_CONSOLES  2
#else
#define NUM_CONSOLES  1
#endif

#define CON_HELP_VERSION	// Help by itself shows the version message
#define CON_VERSION_MESSAGE "LED Console Version 1.1\r\n"
#define CON_INIT_MESSAGE CON_VERSION_MESSAGE

/* Maximum number of entries in the TCP servers' object list */
#define SSPEC_MAXSPEC  30

/* Number of logical extents in the filesystem */
#define FS_MAX_LX  2

/* Maximum number of files on the filesystem */
#define FS_MAX_FILES (SSPEC_MAXSPEC + 2)

/* Log(base 2) of the desired Logical Sector size. */
#define MY_LS_SHIFT 12	// 2^12 == 4096

/********************************
 * End of configuration section *
 ********************************/

/* States of the LED: */
#define ONSTATE   1
#define OFFSTATE  0
#define PWMSTATE  2		/* Pulse-width-modulate the LED */

typedef struct {
	char				top; // used to mark the top of the struct
	struct oneled {
		char	state;			/* ONSTATE, OFFSTATE, PWMSTATE */
		char	on_pwm;			/* milliseconds on/2 */
		char	off_pwm;			/* milliseconds off/2 */
	} 		info[2];
} LedBackupInfo;

/*  Store backup info in CORE memory: */
 LedBackupInfo	led_backup_info;

#memmap xmem

#define MAX_TCP_SOCKET_BUFFERS 5
#define DISABLE_DNS

#use "fs2.lib"
#use "dcrtcp.lib"
#use REMOTEUPLOADDEFS.LIB
#use "zconsole.lib"

/* ----------------------------------------------------------------- */


int hello_world(ConsoleState* state)
{
	state->conio->puts("Hello, Z-World!\r\n");
	return 1;
}

int led_show(ConsoleState * state)
{
	auto char   buffer[80];
	auto int   j;

	for( j=0 ; j < 2 ; ++j ) {
		sprintf(buffer, "LED DS%d is ", j+2);
    	switch( led_backup_info.info[j].state )
    	{
    		case ONSTATE :
    			strcat(buffer, "ON");
    			break;
    		case OFFSTATE :
    			strcat(buffer, "OFF");
    			break;
    		case PWMSTATE :
    			sprintf( buffer+strlen(buffer), "Pulsed: %dms on, %dms off",
	    					led_backup_info.info[j].on_pwm*2,
   	 					led_backup_info.info[j].off_pwm*2 );
   	 		break;
   	 	default :
   	 		sprintf( buffer+strlen(buffer), "?%d?", led_backup_info.info[j].state );
   	 		break;
   	}
   	strcat(buffer, "\r\n");
   	state->conio->puts( buffer );
	}

	return 1;
}   /* end led_show() */

int led_reset(ConsoleState * state)
{
	/*  Put ourselves into a clean state. */
	led_backup_info.info[0].state = OFFSTATE;
	led_backup_info.info[1].state = OFFSTATE;
	con_backup();
	state->conio->puts(".. reset ..\n\r");
	return 1;
}

/* ------------------------------------------------------------------- */

int led2_off(ConsoleState * state)
{
	led_backup_info.info[0].state = OFFSTATE;
	con_backup();
	return 1;
}

int led2_on(ConsoleState * state)
{
   led_backup_info.info[0].state = ONSTATE;
	con_backup();
	return 1;
}   /* end led2_on() */

int led2_pwm(ConsoleState * state)
{
	auto int 	on_millis, off_millis;
	auto char *	ptr;

	if( state->numparams != 4 ) {
		state->error = CON_ERR_BADPARAMETER;
		return -1;
	}

	on_millis = (int) strtol( con_getparam(state->command,2), & ptr, 10);
	off_millis = (int) strtol( con_getparam(state->command,3), & ptr, 10);
	if( (on_millis < 0 || on_millis >= 256*2) ||
		 (off_millis < 0 || off_millis >= 256*2) ) {
		state->error = CON_ERR_BADVARVALUE;
		return -1;
	}

	led_backup_info.info[0].state = PWMSTATE;
	led_backup_info.info[0].on_pwm = on_millis >> 1;
	led_backup_info.info[0].off_pwm = off_millis >> 1;
	con_backup();
	return 1;
}   /* end led3_pwm() */

/* ------------------------------------------------------------------- */

int led3_off(ConsoleState * state)
{
	led_backup_info.info[1].state = OFFSTATE;
	con_backup();
	return 1;
}

int led3_on(ConsoleState * state)
{
	led_backup_info.info[1].state = ONSTATE;
	con_backup();
	return 1;
}   /* end led3_on() */

int led3_pwm(ConsoleState * state)
{
	auto int 	on_millis, off_millis;
	auto char *	ptr;

	if( state->numparams != 4 ) {
		state->error = CON_ERR_BADPARAMETER;
		return -1;
	}

	on_millis = (int) strtol( con_getparam(state->command,2), & ptr, 10);
	off_millis = (int) strtol( con_getparam(state->command,3), & ptr, 10);
	if( (on_millis < 0 || on_millis >= 256*2) ||
		 (off_millis < 0 || off_millis >= 256*2) ) {
		state->error = CON_ERR_BADVARVALUE;
		return -1;
	}

	led_backup_info.info[1].state = PWMSTATE;
	led_backup_info.info[1].on_pwm = on_millis >> 1;
	led_backup_info.info[1].off_pwm = off_millis >> 1;
	con_backup();
	return 1;
}   /* end led3_pwm() */

/* ------------------------------------------------------------------- */

#ximport "samples\rcm2100\ledconsole_help\help_show.txt" help_show_txt
#ximport "samples\rcm2100\ledconsole_help\help_echo.txt" help_echo_txt
#ximport "samples\rcm2100\ledconsole_help\help_reset.txt" help_reset_txt
#ximport "samples\rcm2100\ledconsole_help\help_led_off.txt" help_led_off_txt
#ximport "samples\rcm2100\ledconsole_help\help_led_on.txt" help_led_on_txt
#ximport "samples\rcm2100\ledconsole_help\help_led_pwm.txt" help_led_pwm_txt
#ximport "samples\rcm2100\ledconsole_help\help_help.txt" help_help_txt
#ximport "samples\rcm2100\ledconsole_help\help.txt" help_txt

const ConsoleIO console_io[] =
{
#if DEMO_HAS_SERB
	CONSOLE_IO_SERB(57600),
#endif
	CONSOLE_IO_TELNET(23)
};

const ConsoleCommand console_commands[] =
{
	{ "HELLO WORLD", hello_world, 0 },
	{ "ECHO", con_echo, help_echo_txt },	/* Use with TELNET! */
	{ "SHOW", led_show, help_show_txt },
	{ "RESET",	led_reset, help_reset_txt },
	{ "LED2 ON",  led2_on, help_led_on_txt },
	{ "LED2 OFF", led2_off, help_led_off_txt },
	{ "LED2 PWM", led2_pwm, help_led_pwm_txt },
	{ "LED3 OFF", led3_off, help_led_off_txt },
	{ "LED3 ON",  led3_on, help_led_on_txt },
	{ "LED3 PWM", led3_pwm, help_led_pwm_txt },
	{ "HELP", con_help, help_help_txt },
	{ "", 	NULL, 	help_txt }
};

const ConsoleError console_errors[] = {
	CON_STANDARD_ERRORS
};

const ConsoleBackup console_backup[] =
{
	CONSOLE_BASIC_BACKUP ,
	{ &led_backup_info, sizeof(LedBackupInfo), NULL, NULL }
};

/* ---------------------------------------------------------------------- */

void main(void)
{
	FSLXnum ext1, ext2;
	long lxsize;
	long backupsize;
	word newsize;

	// assert( NUM_CONSOLES == sizeof(console_io)/sizeof(console_io[0]) );
	if( !(NUM_CONSOLES == sizeof(console_io)/sizeof(console_io[0])) ) {
		printf("ERROR: NUM_CONSOLES incorrect\n");
		exit(239);
	}

	// Write 84 hex to slave port control register that initializes parallel
	// port A as an output port (port A drives the LEDs on the Prototyping Board).
	WrPortI(SPCR, &SPCRShadow, 0x84);

	// now write all ones to port A which sets outputs high and LEDs off
	//  Hopefully the costate's below will take FLASH values and do something
	//  interesting.  If it's not our data in FLASH, then record both LED's
	//  off in official state vars.
	WrPortI(PADR, &PADRShadow, 0xff);

	sock_init();

	/* File system setup and partitioning */
	ext1 = fs_get_flash_lx();
	if (ext1 == 0) {
		printf("No flash available!\n");
		exit(1);
	}
	lxsize = fs_get_lx_size(ext1, 1, MY_LS_SHIFT);
	backupsize = ((con_backup_bytes() + (1 << MY_LS_SHIFT)) * 2) +
	             (1 << MY_LS_SHIFT) + (1 << MY_LS_SHIFT);
	newsize = (word)((backupsize * 65536L) / lxsize);
	ext2 = fs_setup(ext1, MY_LS_SHIFT, 0, NULL, FS_PARTITION_FRACTION,
	                newsize, MY_LS_SHIFT, 0, NULL);
	if (ext2 == 0) {
		printf("Could not create backup extent!\n");
		exit(1);
	}

	con_set_files_lx(ext1);
	con_set_backup_lx(ext2);
	fs_init(0, 0);

	if (console_init() != 0) {
		printf("Console did not initialize!");
		lx_format(ext1, 0);
		lx_format(ext2, 0);

		/*  Put ourselves into a clean state. */
		led_backup_info.info[0].state = OFFSTATE;
		led_backup_info.info[1].state = OFFSTATE;

		con_backup();
	}

#if DEMO_HAS_SERB
	printf( "Note: Serial port B active..\n" );
#endif

	while (1) {
		console_tick();
		tcp_tick(NULL);

		/*  XXX - Maybe use IntervalMS() instead of DelayMS() for
		 *  			more accurate delay timings (less jitter).
		 */

		/***  DS2  ***/
		costate {
			switch(led_backup_info.info[0].state)
			{
				case ONSTATE :
					BitWrPortI(PADR, &PADRShadow, 0, 0);
					break;
				case OFFSTATE :
					BitWrPortI(PADR, &PADRShadow, 1, 0);
					break;
				case PWMSTATE :
					BitWrPortI(PADR, &PADRShadow, 0, 0);
					waitfor(DelayMs(led_backup_info.info[0].on_pwm*2));
					BitWrPortI(PADR, &PADRShadow, 1, 0);
					waitfor(DelayMs(led_backup_info.info[0].off_pwm*2));
					break;
			}
		}  /* end costate */

		/***  DS3  ***/
		costate {
			switch(led_backup_info.info[1].state)
			{
				case ONSTATE :
					BitWrPortI(PADR, &PADRShadow, 0, 1);
					break;
				case OFFSTATE :
					BitWrPortI(PADR, &PADRShadow, 1, 1);
					break;
				case PWMSTATE :
					BitWrPortI(PADR, &PADRShadow, 0, 1);
					waitfor(DelayMs(led_backup_info.info[1].on_pwm*2));
					BitWrPortI(PADR, &PADRShadow, 1, 1);
					waitfor(DelayMs(led_backup_info.info[1].off_pwm*2));
					break;
			}
		}  /* end costate */


	}   /* for-ever.. */

}   /* end main() */