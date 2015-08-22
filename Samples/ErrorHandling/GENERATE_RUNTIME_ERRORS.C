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

	Generate_runtime_errors.c

This program generates run-time errors that
can be displayed with DISPLAY_ERRORLOG.C.

1) Make sure ENABLE_ERROR_LOGGING at the top of the
   .\lib\bioslib\errlogconfig.lib is #defined to 1
   unless you are compiling this under RabbitSys.

2) Uncomment the type of error you wish to generate.

3) Compile this program to flash.

4) Disconnect the programming cable from the target

5) Cycle power on the board or hit the reset button if
   it has one.

6) Wait a moment, then reconnect the programming cable.

7) Compile DISPLAY_ERRORLOG.C and run it to see the
   display of the error log, unless you are compiling
   under RabbitSys.  If this is the case just look at
   the monitor runtime logs through the Console.

 ********************************************************************/
#class auto

//**** Uncomment the type of error you wish to generate
//#define DIVIDE_ZERO
//#define RANGE
//#define DOMAIN
//#define BAD_INTERRUPT
//#define BAD_XALLOC
//#define VIRTUAL_WD_TIMEOUT
#define POINTER       // Pointer checking must be enabled in compiler options
//#define ARRAY_BOUNDS  // Must be enabled in compiler options

#define	MAXARRAYSIZE	5

void main()
{
	int	*ip, j;
	float	f;
	char	mystr[MAXARRAYSIZE];

#ifdef DIVIDE_ZERO
	j = 0;
	j = 234/j;
#endif

#ifdef RANGE
	f = fmod(1, 0);
#endif

#ifdef DOMAIN
	f = acos(2);
#endif

#ifdef BAD_INTERRUPT
	;
	asm rst	10h
#endif

#ifdef BAD_XALLOC
	xalloc(0xFFFFFFFF);
#endif

#ifdef VIRTUAL_WD_TIMEOUT
	j  = VdGetFreeWd(3);  // allocate a VWD
	while(1);             // and never hit it
#endif

#ifdef POINTER
	ip = (int *)0x0000;
	*ip = 4;
#endif

#ifdef ARRAY_BOUNDS
	j = MAXARRAYSIZE+10;
	mystr[j] = 'Q';
#endif

}

