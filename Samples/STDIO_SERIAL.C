/***********************************************************

     	Samples\SERIAL_STDIO.C
      Z-World, 2002

      Demonstration of redirection of STDIO output from
      Dynamic C's STDIO window to a serial port (this
      feature was added in Dynamic C 7.25).

      NOTE: these macros are designed for debugging use only!
		They allow STDIO functions to be redirected to the
		serial ports for viewing in run mode -- these macros
		are not designed to be a standard part of a Dynamic C
		program (character input is not buffered, for example).

		When in debug mode, STDIO will go to the STDIO window
		as usual.  When in run mode, STDIO will go to the
		designated serial port.  This is especially useful since
		the programming port (serial port A) can be used with
		the PROG header for debug mode and the DIAG header
		in run mode without recompiling the program, keeping the
		other serial ports free for application use.

		The desired serial port is selected by adding ONE of the
		following lines to your program:

				#define	STDIO_DEBUG_SERIAL	SADR
				#define	STDIO_DEBUG_SERIAL	SBDR
				#define	STDIO_DEBUG_SERIAL	SCDR
				#define	STDIO_DEBUG_SERIAL	SDDR

		Then specify the desired baud rate with a define like
		the following:

				#define	STDIO_DEBUG_BAUD	57600

		Many PC-based terminal programs expect a carriage return
		as well as a newline character at the end of each line.
		A carriage return ('\r') can be added to each linefeed ('\n')
		in a printed string by adding the following line:

				#define	STDIO_DEBUG_ADDCR

		If serial debugging is desired even when in programming mode,
		it can be forced by adding the following line:

				#define	STDIO_DEBUG_FORCEDSERIAL

************************************************************/
#class auto


// Add these lines to redirect run-mode printf output at 57600
// baud to serial port B.

#define	STDIO_DEBUG_SERIAL	SBDR
#define	STDIO_DEBUG_BAUD		57600

// Add this line to add carriage returns ('\r') to each newline
// char ('\n') when sending printf output to a serial port.

#define	STDIO_DEBUG_ADDCR

// Add this line to force both run-mode and program-mode printf
// output to a serial port.

//#define	STDIO_DEBUG_FORCEDSERIAL


void main()
{
	char	s[256];

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	printf("This is a test of printf redirect.\n");
	printf("this is the second line of the test.\n");

	printf("Please enter a string: ");
	gets(s);
	printf("You entered '%s'.\n", s);

	while (1)
		;	// avoid watchdog timeout and restart in run mode
}