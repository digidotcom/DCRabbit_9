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
/********************************************************************

DISPLAY_ERRORLOG.C

Program to display the contents of the error log buffer.  Errors are
logged if ENABLE_ERROR_LOGGING in .\lib\bioslib\errlogconfig.lib is
#defined to 1.

This or other programs in this directory can be used to generate run
time errors or watchdog timeouts.  If run in debug mode, this
program's current error log will not be displayed until after this
program is recompiled and rerun on the target.  (In debug mode, the
flash copy of the previous program's error log is displayed.)

When a Dynamic C cold boot takes place, the error log header in RAM
is zeroed out to initialize it, but first it's contents are copied to
an address in the BIOS code before the BIOS in RAM is copied to
flash.  This means that on the second cold boot, the data structure
in flash will be zeroed out.  The configuration of the log buffer may
still be read, and the log buffer entries are not affected.

Because the fatal exception mechanism resets the processor by causing
a watchdog time out, the number of watchdog time outs reported is the
number of actual WDTOs plus the number of fatal exceptions.

The meaning of the bit-coded status byte is as follows:
bit 0   - An error has occurred since deployment.
bit 1   - The count of SW resets has rolled over.
bit 2   - The count of HW resets has rolled over.
bit 3   - The count of WDTOs has rolled over.
bit 4   - The count of exceptions has rolled over.
bit 5-6 - Not used.
bit 7   - The error log has been initialized.

The index of the last exception is the index from the start of the
error log entries.  If this index does not equal the total exception
count minus one, the error log entries have wrapped around the log
buffer.  The index of the last exception is reported as -1 when none
has occurred.

********************************************************************/
#if (_USER)
   #error RabbitSys contains error logging support by default.
   #error Non-RabbitSys error logging is not supported.
#endif


#class auto

// Add these lines to redirect printf output at 57600 baud to serial port A.
#define	STDIO_DEBUG_SERIAL	SADR
#define	STDIO_DEBUG_BAUD		57600

// Add this line to add carriage returns ('\r') to each newline char ('\n')
//  when sending printf output to a serial port.
#define	STDIO_DEBUG_ADDCR

#define	AINBUFSIZE	31
#define	AOUTBUFSIZE	31

main()
{
	char mystr[10], *whichstr;
	int *ip, j;
	unsigned k, n;
	unsigned long ulPAddr;
	float f;

	printf("\n\n\n\n%s\n", errlogGetHeaderInfo());
	// the default is to display only valid entries
	n = (errLogInfo.ExceptionIndex > (unsigned) ERRLOG_NUM_ENTRIES)
	    ? (unsigned) ERRLOG_NUM_ENTRIES : errLogInfo.ExceptionIndex;

	printf("Type a digit to display a selection of error log entries.\n\n");
	whichstr = ((OPMODE & 0x08) == 0x08) ? " previous " : " ";
	printf("0:  no error log entries\n");
	printf("1:  the %u valid%serror log entries only\n", n, whichstr);
	printf("2:  all %u%serror log entries\n", ERRLOG_NUM_ENTRIES, whichstr);
	switch ((int) getchar()) {
	case (int) '0':
		// have chosen to display no entries instead
		n = 0;
	case (int) '1':
	default:
		// have chosen to keep the default display entries
		break;
	case (int) '2':
		// have chosen to display all entries instead
		n = ERRLOG_NUM_ENTRIES;
		break;
	}
	printf("\n\n\n\n");

	// initial displayed entry is the nth one before the current exception index
	k = (errLogInfo.ExceptionIndexMod + ERRLOG_NUM_ENTRIES - n)
	    % ERRLOG_NUM_ENTRIES;

	for (j = 0; j < n; ++j, ++k) {
		k %= (unsigned) ERRLOG_NUM_ENTRIES;
		if (!errlogGetNthEntry(k)) {
			ulPAddr = error_message(errLogEntry.errType);
			if (ulPAddr != 0ul) {
				printf("Entry %d (%ls):\n", k, ulPAddr);
			} else {
				printf("Entry %d (Unknown exception code!):\n", k);
			}
			printf("%s\n", errlogFormatEntry());
			printf("%s\n", errlogFormatStackDump());
			printf("%s\n", errlogFormatRegDump());
		} else {
			printf("Checksum Error\n");
		}
	}

	// Explain why, if in debug mode and if the following offer to generate a
	//  run time error is taken up, such generated run time errors will not be
	//  correctly displayed until this program is recompiled and rerun on the
	//  target, so that the error log is copied into flash.
	if ((OPMODE & 0x08) == 0x08) {
		// display information about flash copy of error log only in debug mode!
		printf("\n\n\n\nNB:  In debug mode, this program's displayed error log ");
		printf("header\n     information is taken from the flash copy of the pr");
		printf("evious\n     program's error log.  Thus, even though the curren");
		printf("t error log\n     is updated in RAM, the displayed error log he");
		printf("ader information\n     will not change until this program is re");
		printf("compiled and rerun.\n");
	}

	printf("\n\nType a digit to generate a run time error or reset,\n");
	printf("or press the Enter key to bypass all run time errors.\n\n");
	printf("0:  bad pointer\n");
	printf("1:  bad array index\n");
	printf("2:  domain\n");
	printf("3:  range\n");
	printf("4:  integer divide by zero\n");
	printf("5:  unexpected interrupt\n");
	printf("6:  virtual watchdog time out\n");
	printf("7:  bad xalloc\n");
	if ((OPMODE & 0x08) == 0x00) {
		// these items are available only in run mode!
		printf("8:  software reset (no error logged)\n");
		printf("9:  watchdog time out (no error logged)\n");
	}
	switch ((int) getchar()) {
	case (int) '0':	// bad pointer
		ip = (int *) 0x0000;
		*ip = 4;
		break;
	case (int) '1':	// bad array index
		j = sizeof(mystr);
		mystr[j] = 'Q';
		break;
	case (int) '2':	// domain
		f = acos(2);
		break;
	case (int) '3':	// range
		f = fmod(1, 0);
		break;
	case (int) '4':	// integer divide by zero
		j = 0;
		1 / j;
		break;
	case (int) '5':	// unexpected interrupt
		asm   rst   10h
		break;
	case (int) '6':	// virtual watchdog time out
		j = VdGetFreeWd(3);
		while(1);
		break;
	case (int) '7':	// bad xalloc
		xalloc(0xFFFFFFFFL);
		break;
	case (int) '8':	// software reset (no error logged)
		if ((OPMODE & 0x08) == 0x00) {
			// this item is available only in run mode!
			//  (it would confuse the debug kernel)
			forceSoftReset();
		}
		break;
	case (int) '9':	// watchdog time out (no error logged)
		if ((OPMODE & 0x08) == 0x00) {
			// this item is available only in run mode!
			//  (it would cause loss of target communication)
			asm   ipset 3
			while(1);
		}
	default:				// no error or reset
		break;
	}
}

