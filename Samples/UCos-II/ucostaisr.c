/******************************************************************

      uCOSTaisr.c
      Z-World, 2001

		This program uses a task aware isr for the rx side of the
		serial port B isr and for the tx size of the serial port C
		isr.  A user entered message is sent from serial port C to
		serial port B.  The message is then displayed along with the
		number of bytes transmitted by serial port C and received by
		serial port B.

		SendMessage - Use this function via the watch window to send
		a message from serial port C to serial port B once the program
		is running.  Open the watch window either via
		Inspect | Add/Del Watch expression, or by pressing ctrl+w.
		In the watch expression dialog, type:
		SendMessage("user message")
		where user message is any string of characters up to USER_MSG_LEN

		- task0 waits at a semaphore which is signaled by the task aware
		isr for serial port C.  When the semaphore is signaled, the task
		increments the number of bytes recieved.

		- task1 waits at a semaphore which is signaled by the task aware
		isr for serial port B.  When a byte arrives at the serial port,
		the semaphore is signaled when and the task increments a counter
		of bytes received.

		- task2 waits for the user to send a message.  The message is sent
		from serial port C to serial port B, and is then displayed in the
		stdio window along with the number of bytes transmitted and received.

		- spb_isr is rewritten at the end of this file.  The rx side of the
		isr is task aware such that if the semaphore task1 is waiting on
		becomes signaled due to the arrival of a byte, and a higher priority
		task is not ready to run, then a context switch to task1 will occur.
		The compiler should generate a warning since spb_isr replaces the
		isr found in rs232.lib.  This warning can be ignored.

		- spc_isr is rewritten at the end of this file.  The tx side of the
		isr is task aware such that if the semaphore task1 is waiting on
		becomes signaled due to the sending of a byte, and a higher priority
		task is not ready to run, then a context switch to task0 will occur.
		The compiler should generate a warning since spc_isr replaces the
		isr found in rs232.lib.  This warning can be ignored.


		Make the following connection on the controller:
		----------------------------
		1. Connect PortC TXC to PortB RXB.

See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.

******************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define BOUTBUFSIZE 255
#define BINBUFSIZE  255
#define CINBUFSIZE  255
#define COUTBUFSIZE 255


// redefine configuration macros as necessary, defaults are found in os_cfg.lib
#define OS_SCHED_LOCK_EN		1
#define OS_SEM_EN					1
#define OS_MAX_TASKS				3
#define OS_MAX_EVENTS			2

// must explicitly use ucos library
#use ucos2.lib

// maximum length of user message
#define USER_MSG_LEN 80



// Semaphores signaled by task aware isrs
OS_EVENT* serBsem;
OS_EVENT* serCsem;

// Counters for number of bytes sent and received
unsigned int bytessent;
unsigned int bytesreceived;

int sendmsg;
char msg[USER_MSG_LEN];

// Function prototypes for tasks
void task0(void* pdata);
void task1(void* pdata);
void task2(void* pdata);

void main()
{
	INT8U err;

	//Uncomment the brdInit() if running a LP35XX board.
	//This is necessary for initializing RS232 functionality
	//of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	bytessent     = 0;
	bytesreceived = 0;
	sendmsg       = 0;

	// Initialize internal OS data structures
	OSInit();

	// Create the three tasks with no initial data and 512 byte stacks
	err = OSTaskCreate(task0, NULL, 512, 0);
	if(err != OS_NO_ERR)
		exit(err);
	err = OSTaskCreate(task1, NULL, 512, 1);
	if(err != OS_NO_ERR)
		exit(err);
	err = OSTaskCreate(task2, NULL, 512, 2);
	if(err != OS_NO_ERR)
		exit(err);

	// open both serial ports, and set baud rates to be used by both
	serBopen(57600);
	serCopen(57600);

	// clear rx and tx data buffers
	serBrdFlush();
	serBwrFlush();
	serCrdFlush();
	serCwrFlush();

	// create semaphores used by task0 and task1
	serBsem = OSSemCreate(0);
	if(!serBsem)
		exit(1);
	serCsem = OSSemCreate(0);
	if(!serCsem)
		exit(2);

	// begin multi-tasking (execution will be transferred to task0)
	OSStart();
}

void task0(void* pdata)
{
	auto INT8U err;

	while(1)
	{
		// wait for a byte to be sent from serial port.  This semaphore
		// will be signaled by the task aware isr for serial port C
		// at the end of this file.
		OSSemPend(serCsem, 0, &err);
		bytessent++;
	}
}

void task1(void* pdata)
{
	auto INT8U err;

	while(1)
	{
		// wait for a byte to arrive at serial port.  This semaphore
		// will be signaled by the task aware isr for serial port B
		// at the end of this file.
		OSSemPend(serBsem, 0, &err);
		bytesreceived++;
	}
}

void task2(void* pdata)
{
	static char buf[USER_MSG_LEN];

	while(1)
	{
		// if sendmsg is true, send bytes from serial port c to serial port b
      if (sendmsg)
      {
			// clear message receive buffer
			memset(buf, 0, sizeof(buf));
			// send user message out serial port C
      	serCputs(msg);
      	while(serCwrFree() != COUTBUFSIZE);

			// read message from serial port B
      	while(serBread((void*)buf, sizeof(buf), 10) == 0);

			// display number of bytes sent and received along with received message
      	printf("tx c: %d \n", bytessent);
      	printf("rx b: %d \n", bytesreceived);
      	printf("Message: %s\n", buf);
      	bytessent = bytesreceived = 0;
      	sendmsg = 0;
      }
	}
}

root void SendMessage(char* Msg)
{
	strcpy(msg, Msg);
	sendmsg = 1;
}

// --------- serial port b task aware interrupt ---------------------------- //
#asm
;
; spb_isr
;

spb_isr::
	push	af							; 7, 	restore registers needed by isr
	push	bc							; 7
	push	de							; 7
	push	hl							; 7

ioi ld	 a,(SBSR)				; 11,	interrupt source rx or tx
	 ld    c,a
	 rla								; 2
	 jp	 c,spb_rx				; 5

spb_tx:
    bit   2,c           		; Tx busy? bit 2 shifted via rla
    jr    nz,spb_txbusy
spb_txidle:             		; Tx idle, safe for enable/disable transition
	 ld	 a, (blongstop)
	 or	 a
	 jr	 nz, spb_dolongstop	; handle request for a loooong (2 byte) stop bit

	 ld 	 a, (brtscts)
	 or	 a
    jr	 z, spb_trytosend 	; try one more time to load a byte
	 call  spb_checkcts
	 bool	 hl
	 jr	 nz, spb_disabletx

spb_trytosend:
	 call	 bEnable
	 call	 spb_txload				; CTS is on, so try to load a normal byte
	 jr	 spb_donomore

spb_dolongstop:
	 xor	 a
	 ld	 (blongstop), a		; clear out the long stop flag
spb_disabletx:
	 call  bDisable
	 ld	 a, 42
ioi ld	 (SBDR), a				; load 42 into SADR as a dummy byte,
										; to keep the interrupt loop going
	 jr	 spb_donomore

spb_txbusy:
    ld    a,(PCFRShadow)		; check to make sure tx is enabled
    bit   BDRIVE_TXD,a			; if not, dummy character still needs to
    									; clear out
    jr    z, spb_donomore
	 ld 	 a, (brtscts)			; are we using CTS?
	 or	 a
    jr	 z, spb_txallgood
	 call  spb_checkcts
	 bool	 hl
	 jr	 nz, spb_donomore		; CTS is off, don't fill SBDR
spb_txallgood:
	 call	 spb_txload				; will load a good byte, it there is one to
	 									; be loaded
spb_donomore:
ioi ld    (SBSR),a      		; 10,	clear interrupt source

	jp		spb_finish

spb_rx:
	ld		hl,bios_intnesting
	inc	(hl)

ioi ld    a,(SBDR)      		; 11,	receive the character
    bool  hl            		; 2,	place character on circular buffer
    ld    l,a           		; 2,		throw it away if no room
	 ld	 b, a						; save the byte for possible parity check
	 ld	 a, (bsevenbit)
	 or	 a
	 jr	 z, spb_rxcontinue
	 ld	 a, L
	 and	 0x7f
	 ld	 L, a						; mask out MSB for 7 bit data
spb_rxcontinue:
	 push	 bc						; hold onto byte copy and status register
    push  hl            		; 10
    ld    hl,spb_icbuf  		; 6
    push  hl            		; 10
    call  cbuf_putch	   		; 12+...
    add   sp,4          		; 4

	 ld	 a,(OSRunning)			; signal ucos semaphore that byte has arrived
	 ld	 b,a
	 or	 a
	 jr    z,spb_nopost
	 push  iy						; OSSemPost trashes iy (other regs already saved)
	 ld	 hl,(serBsem)
    push	 hl
    call	 OSSemPost
	 add	 sp,2
    pop   iy

spb_nopost:

    pop	 bc						; get copy and status reg back

	 bit	 5, c						; check for receiver overrun
	 jr	 z, spb_checkparity
	 ld	 a, 0x01
	 ld	 (boverrun), a

spb_checkparity:
	 ld	 a, (bparity)
	 or	 a								; see if we need to check parity of
	 										; incoming byte
	 jr	 z, spb_rx_flowcontrol
	 ld	 a, (bsevenbit)
	 or	 a
	 jr	 nz, spb_check7bit
spb_check8bit:
	 ld	 L, b
	 ld	 a, (bparitytype)
	 ld	 h, a
	 call	 ser8_getparity
	 bool  hl
	 jr	 z, spb_check9low
spb_check9high:						; 9th parity bit should be high,
											; otherwise we have a parity error
	 bit	 6, c							; C reg should still have a snapshot of
	 										; the status register, checking bit 9
	 jr	 z, spb_rx_flowcontrol	; no 9th bit detected (9th bit high),
	 										; so we are OK
	 ld	 a, 0x01
	 ld	 (bparityerror), a		; signal a parity error
	 jr	 spb_rx_flowcontrol
spb_check9low:
	 bit	 6, c
	 jr	 nz, spb_rx_flowcontrol	; 9th bit detected (9th bit low),
	 										; which is what we expect
	 ld	 a, 0x01
	 ld	 (bparityerror), a
	 jr	 spb_rx_flowcontrol
spb_check7bit:
	 ld	 L, b
	 ld	 a, (bparitytype)
	 ld	 h, a
	 call	 ser7_checkparity
	 bool	 hl
	 jr	 nz, spb_rx_flowcontrol
	 ld	 a, 0x01
	 ld	 (bparityerror), a

spb_rx_flowcontrol:
    ld    a,(brtscts)
    or    a
    jr    z,spb_rx0
    ld    hl,spb_icbuf
    push  hl
    call  cbuf_used
    add   sp,2
    ld    d,h
    ld    e,l
    ld    b,d
    ld    c,e           ; copy number bytes used to bc
    ld    hl,(serBrtsHi)
    ld    a,d
    cp    h
    jr    c,spb_rx0
    ld    a,e
    cp    l
    jr    c,spb_rx0
spb_overHi:					; turn off the RTS line
	 ld	 hl, spb_rx0
	 push	 hl				;set return address
	 ld	 hl, (b_rtsoff)
	 jp	 (hl)

spb_rx0:
	ld		a,(OSRunning)
	or		a
	jr		z,spb_decnesting

	call	OSIntExit

spb_decnesting:
	push	ip
	ipset	1
	ld		hl,bios_intnesting
	dec	(hl)
	jr		nz,spb_finish_rx

	ld		a,(bios_swpend)
	or		a
	jr		z,spb_finish_rx

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

spb_finish_rx:
	pop	ip

spb_finish:
	ipres

   pop   hl            ; 7,  restore registers needed by isr
   pop   de            ; 7
   pop   bc            ; 7
   pop   af            ; 7

   ret                 ; 13
#endasm


// --------- serial port c task aware interrupt ---------------------------- //
#asm
;
; spc_isr
;

spc_isr::
	 push	af							; 7, 	restore registers needed by isr
	 push	bc							; 7
	 push	de							; 7
	 push	hl							; 7

ioi ld	 a,(SCSR)				; 11,	interrupt source rx or tx
	 ld    c,a
	 rla								; 2
	 jp	 c,spc_rx				; 5

spc_tx:
	ld		hl,bios_intnesting
	inc	(hl)

    bit   2,c           		; Tx busy? bit 2 shifted via rla
    jr    nz,spc_txbusy
spc_txidle:             		; Tx idle, safe for enable/disable transition
	 ld	 a, (clongstop)
	 or	 a
	 jr	 nz, spc_dolongstop	; handle request for a long stop bit

	 ld 	 a, (crtscts)
	 or	 a
    jr	 z, spc_trytosend 	; try one more time to load a byte
	 call  spc_checkcts
	 bool	 hl
	 jr	 nz, spc_disabletx

spc_trytosend:
	 call	 cEnable
	 call	 spc_txload				; CTS is on, so try to load a normal byte
	 jr	 spc_signalsem

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
	 call	 spc_txload				; will load a good byte,
	 									; if there is one to be loaded
spc_signalsem:
	 ld	 a,(OSRunning)			; signal ucos semaphore that a byte has been sent
	 ld	 b,a
	 or	 a
	 jr    z,spc_donomore
	 push  iy						; OSSemPost trashes iy (other regs already saved)
	 ld	 hl,(serCsem)
    push	 hl
    call	 OSSemPost
	 add	 sp,2
    pop   iy

spc_donomore:
ioi ld    (SCSR),a      		; 10,	clear interrupt source
	ld		a,(OSRunning)
	or		a
	jr		z,spc_decnesting

	call	OSIntExit

spc_decnesting:
	push	ip
	ipset	1
	ld		hl,bios_intnesting
	dec	(hl)
	jr		nz,spc_finish_tx

	ld		a,(bios_swpend)
	or		a
	jr		z,spc_finish_tx

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

spc_finish_tx:
	pop	ip

    ipres               ; 4
    pop   hl            ; 7, 	restore registers needed by isr
    pop   de            ; 7
    pop   bc            ; 7
    pop   af            ; 7
    ret                 ; 13

spc_rx:
ioi ld    a,(SCDR)      ; 11,	receive the character
    bool  hl            ; 2,	place character on circular buffer
    ld    l,a           ; 2,		throw it away if no room
	 ld	 b,a				;save for later possible parity check
	 ld	 a, (csevenbit)
	 or	 a
	 jr	 z, spc_rxcontinue
	 ld	 a, L
	 and	 0x7f
	 ld	 L, a				;mask out MSB for 7 bit data
spc_rxcontinue:
	 push	 bc				;save byte copy and status register copy
    push  hl            ; 10
    ld    hl,spc_icbuf  ; 6
    push  hl            ; 10
    call  cbuf_putch	   ; 12+...
    add   sp,4          ; 4
    pop	 bc

    bit	 5, c				;test for receiver overrun
    jr	 z, spc_checkparity
    ld	 a, 0x01
    ld	 (coverrun), a

spc_checkparity:
	 ld	 a, (cparity)
	 or	 a								;see if we need to check parity of
	 										; incoming byte
	 jr	 z, spc_rx_flowcontrol
	 ld	 a, (csevenbit)
	 or	 a
	 jr	 nz, spc_check7bit
spc_check8bit:
	 ld	 l, b
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
	 ld	 l, b
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
    ld    c,e           ; copy number bytes used to bc
    ld    hl,(serCrtsHi)
    ld    a,d
    cp    h
    jr    c,spc_rx0
    ld    a,e
    cp    l
    jr    c,spc_rx0
spc_overHi:    			; turn off the RTS line
	 ld	 hl, spc_rx0
	 push	 hl				;set return address
	 ld	 hl, (c_rtsoff)
	 jp	 (hl)

spc_rx0:
    ipres               ; 4
    pop   hl            ; 7,  restore registers needed by isr
    pop   de            ; 7
    pop   bc            ; 7
    pop   af            ; 7
    ret                 ; 13

#endasm