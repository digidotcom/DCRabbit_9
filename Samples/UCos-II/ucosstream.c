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
/******************************************************************

      uCOSStream.c

		This program uses a task aware isr for the rx side of the
		serial port C isr.  Bytes are sent from one task to another
		via serial port C, and status information is sent to the stdio
		window every second.

		- task0 is responsible for sending the number of bytes transmitted
		by task2 and received by task1 for the previous second.

		- task1 waits at a semaphore which is signaled by the task aware
		isr for serial port C.  The semaphore is signaled when a byte
		arrives at the serial port, and the task receives the byte and
		increments a counter of bytes received.

		- task2 sends bytes out serial port C and increments a counter of
		bytes sent.

		- spc_isr is rewritten at the end of this file.  The rx side of the
		isr is task aware such that if the semaphore task1 is waiting on
		becomes signaled due to the arrival of a byte, and a higher priority
		task is not ready to run, then a context switch to task1 will occur.
		The compiler should generate a warning since spc_isr replaces the
		isr found in rs232.lib.  This warning can be ignored.

		Make the following connections:
		----------------------------
 		1. Connect serial TXC to RXC.

See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.

******************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define COUTBUFSIZE 15
#define CINBUFSIZE  15

#define OS_MAX_TASKS		3
#define OS_MAX_EVENTS	1
#define OS_SEM_EN 		1
// must explicitly use ucos library
#use ucos2.lib

// Function prototypes for tasks
void task0(void* pdata);
void task1(void* pdata);
void task2(void* pdata);

// Semaphore signaled by task aware isr
OS_EVENT* serCsem;

// Counters for number of bytes sent and received
unsigned int bytessent;
unsigned int bytesreceived;


void main()
{

	//This is necessary for initializing RS232 functionality
	//of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif
	bytesreceived = 0;
	bytessent     = 0;

	// Initialize internal OS data structures
	OSInit();

	// Create the three tasks with no initial data and 512 byte stacks
	OSTaskCreate(task0, NULL, 512, 0);
	OSTaskCreate(task1, NULL, 512, 1);
	OSTaskCreate(task2, NULL, 512, 2);

	// open serial port and set baud rate
	serCopen(2400);

	// clear rx and tx data buffers
	serCrdFlush();
	serCwrFlush();

	// create semaphore used by task1
	serCsem = OSSemCreate(0);

	// display start message
	printf("*********************************\n");
	printf("Start\n");
	printf("*********************************\n");

	// begin multi-tasking (execution will be transferred to task0)
	OSStart();
}

void task0(void* pdata)
{
	static int count;

	count = 0;

	while(1)
	{
		// wait for a second and let stats be collected
		OSTimeDly(1 * OS_TICKS_PER_SEC);

		// format stats and send to stdio
		printf("%04d rx: %6u, tx: %6u\n\r", count, bytesreceived, bytessent);

		bytessent = bytesreceived = 0;
		count++;
	}
}

void task1(void* pdata)
{
	auto INT8U err;

	while(1)
	{
		// wait for a byte to arrive at serial port.  This semaphore
		// will be signaled by the task aware isr for serial port C
		// at the end of this file.
		OSSemPend(serCsem, 0, &err);

		// if byte matches byte transmitted, count it.
		if(serCgetc() == 'Z')
			bytesreceived++;
	}
}

void task2(void* pdata)
{
	while(1)
	{
		// Send byte out serial port C and count it as sent
		if(serCputc('Z'))
			bytessent++;
	}
}

// --------- serial port c task aware interrupt ---------------------------- //

#asm
;
; spc_isr
;

spc_isr::
	push	af							; 7, 	save registers needed by isr
	push	bc							; 7
	push	de							; 7
	push	hl							; 7

ioi ld	 a,(SCSR)				; 11,	interrupt source rx or tx
	 ld    c,a
	 rla								; 2
	 jp	 c,spc_rx				; 5

spc_tx:
    bit   2,c           		; Tx busy? bit 2 shifted via rla
    jr    nz,spc_txbusy
spc_txidle:             		; Tx idle, safe for enable/disable transition
	 ld	 a, (clongstop)
	 or	 a
	 jr	 nz, spc_dolongstop	; handle request for a loooong (2 byte) stop bit

	 ld 	 a, (crtscts)
	 or	 a
    jr	 z, spc_trytosend 	; try one more time to load a byte
	 call  spc_checkcts
	 bool	 hl
	 jr	 nz, spc_disabletx

spc_trytosend:
	 call	 cEnable
	 call	 spc_txload				; CTS is on, so try to load a normal byte
	 jr	 spc_donomore

spc_dolongstop:
	 xor	 a
	 ld	 (clongstop), a		; clear out the long stop flag
spc_disabletx:
	 call  cDisable
	 ld	 a, 42
ioi ld	 (SCDR), a				; load 42 into SCDR as a dummy byte,
										; to keep the interrupt loop going
	 jr	 spc_donomore

spc_txbusy:
    ld    a,(PCFRShadow)		; check to make sure tx is enabled
    bit   CDRIVE_TXD,a			; if not, dummy character still needs to
    									; clear out
    jr    z, spc_donomore
	 ld 	 a, (crtscts)			; are we using CTS?
	 or	 a
    jr	 z, spc_txallgood
	 call  spc_checkcts
	 bool	 hl
	 jr	 nz, spc_donomore		; CTS is off, don't fill SCDR
spc_txallgood:
	 call	 spc_txload				; will load a good byte, it there is one to
	 									; be loaded
spc_donomore:
ioi ld    (SCSR),a      		; 10,	clear interrupt source

	jp		spc_finish

spc_rx:
	ld		hl,bios_intnesting
	inc	(hl)

ioi ld    a,(SCDR)      		; 11,	receive the character
    bool  hl            		; 2,	place character on circular buffer
    ld    l,a           		; 2,		throw it away if no room
	 ld	 b, a						; save the byte for possible parity check
	 ld	 a, (csevenbit)
	 or	 a
	 jr	 z, spc_rxcontinue
	 ld	 a, L
	 and	 0x7f
	 ld	 L, a						; mask out MSB for 7 bit data
spc_rxcontinue:
	 push	 bc						; hold onto byte copy and status register
    push  hl            		; 10
    ld    hl,spc_icbuf  		; 6
    push  hl            		; 10
    call  cbuf_putch	   		; 12+...
    add   sp,4          		; 4

; signal ucos semaphore that byte has arrived
	 ld	 a,(OSRunning)
	 ld	 b,a
	 or	 a
	 jr    z,nopost
	 push  iy						; OSSemPost trashes iy (other regs already saved)
    exx
    push  hl
    exx
	 ld	 hl,(serCsem)
    push	 hl
    call	 OSSemPost
	 add	 sp,2
    exx
    pop   hl
    exx
    pop   iy

nopost:

    pop	 bc						; get copy and status reg back

	 bit	 5, c						; check for receiver overrun
	 jr	 z, spc_checkparity
	 ld	 a, 0x01
	 ld	 (coverrun), a

spc_checkparity:
	 ld	 a, (cparity)
	 or	 a							; see if we need to check parity of
	 									; incoming byte
	 jr	 z, spc_rx_flowcontrol
	 ld	 a, (csevenbit)
	 or	 a
	 jr	 nz, spc_check7bit
spc_check8bit:
	 ld	 L, b
	 ld	 a, (cparitytype)
	 ld	 h, a
	 call	 ser8_getparity
	 bool  hl
	 jr	 z, spc_check9low
spc_check9high:						; 9th parity bit should be high,
											; otherwise we have a parity error
	 bit	 6, c							; C reg should still have a snapshot of
	 										; the status register, checking bit 9
	 jr	 z, spc_rx_flowcontrol	; no 9th bit detected (9th bit high),
	 										; so we are OK
	 ld	 a, 0x01
	 ld	 (cparityerror), a				;signal a parity error
	 jr	 spc_rx_flowcontrol
spc_check9low:
	 bit	 6, c
	 jr	 nz, spc_rx_flowcontrol	; 9th bit detected (9th bit low),
	 										; which is what we expect
	 ld	 a, 0x01
	 ld	 (cparityerror), a
	 jr	 spc_rx_flowcontrol
spc_check7bit:
	 ld	 L, b
	 ld	 a, (cparitytype)
	 ld	 h, a
	 call	 ser7_checkparity
	 bool	 hl
	 jr	 nz, spc_rx_flowcontrol
	 ld	 a, 0x01
	 ld	 (cparityerror), a

spc_rx_flowcontrol:
    ld    a,(crtscts)
    or    a
    jr    z,spc_rx0
    ld    hl,spc_icbuf
    push  hl
    call  cbuf_used
    add   sp,2
    ld    d,h
    ld    e,l
    ld    b,d
    ld    c,e           			; copy number bytes used to bc
    ld    hl,(serCrtsHi)
    ld    a,d
    cp    h
    jr    c,spc_rx0
    ld    a,e
    cp    l
    jr    c,spc_rx0
spc_overHi:								; turn off the RTS line
	 ld	 hl, spc_rx0
	 push	 hl							; set return address
	 ld	 hl, (c_rtsoff)
	 jp	 (hl)

spc_rx0:
	ld		a,(OSRunning)
	or		a
	jr		z,spc_decnesting

	call	OSIntExit

spc_decnesting:
	push	ip
	ipset	1
	ld		hl,bios_intnesting
	dec	(hl)
	jr		nz,spc_finish_rx

	ld		a,(bios_swpend)
	or		a
	jr		z,spc_finish_rx

	ex		af,af'
	push	af
	exx
	push	hl
	push	de
	push	bc
	push	iy
	push	ix

	lcall	bios_intexit

	pop	ix
	pop	iy
	pop	bc
	pop	de
	pop	hl
	exx
	pop	af
	ex		af,af'

spc_finish_rx:
	pop	ip

spc_finish:
	ipres

   pop   hl            ; 7,  restore registers needed by isr
   pop   de            ; 7
   pop   bc            ; 7
   pop   af            ; 7

   ret                 ; 13
#endasm