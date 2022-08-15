st7/ 	; This is required for specifying the instruction set for the targeted processor         

;****************************************************************************************
;|										
;|		     Common Microcode for RabbitNet Products 
;|			Copyright (c), Z-World 2003				
;|			Copyright (c), Digi International 2022
;|										
;****************************************************************************************
;|  Program Name:	common.asm	
;|  Date:   		4/4/03        
;|  Author:             CMG_MCD Application Team
;|  Revision History:	Rev 1.00 RNTeam	Initial Release  
;|                      Rev 1.01 EK  	Corrected WDT problem...Approx line 138
;|			Changed From:
;|			.soft_reset:	ld	a,cmd07_stat 
;|					and	a,#$40	 
;|			Changed To:
;|			.soft_reset:	ld	a,cmd07_stat 
;|					and	a,#$60	                               
;|                       
;****************************************************************************************
   
 TITLE	"COMMON.ASM"
 MOTOROLA
 #INCLUDE "I:\Projects\Rabbitnet_Common\Microcode\Rev_101\st72264.inc"	
 #INCLUDE "I:\Projects\Rabbitnet_Common\Microcode\Rev_101\common.inc"	
                    
 WORDS  
 	EXTERN  crcEnable
 	EXTERN 	cmd_exit
 	EXTERN	spi_jtbl
 	EXTERN 	main       
 	EXTERN	brd_hard_init 
 	EXTERN	brd_soft_init
 	EXTERN	brd_init   
 	EXTERN  user_handler
 	EXTERN	interval_tmr   

	          	
 	EXTERN	sig_regs			; Location of signature registers
 	EXTERN	prod_id                         ; Location of product id number 
 	EXTERN	prod_rev                        ; Location of product revision
 	EXTERN	code_rev                        ; Location of code revision level
 	EXTERN	prod_sn                         ; Location of the product serial number
 	EXTERN	swdt_ival                       ; Location of the initial value of SWDT timeout

 	
 BYTES
	segment 'Application_ram'
.ch_sum			ds.b 3
.start_page		ds.w 1

.act_tmr		ds.b 1			; Activity LED timer
.swdt_reload		ds.b 1			; Software WDT reload value
.swdt_tmr		ds.b 1			; Software WDT down counter
.rn_cmd			ds.b 1			; Rabbit-Net Command Register
.rn_stat		ds.b 1			; Rabbit-Net Status Byte
.cmd07_stat:		ds.b 1			; Command 07 status
						; Bit 7 ---> Hard Reset
						; Bit 6 ---> Soft Reset
						; Bit 5 ---> Hard Reset Occurred
						; Bit 4 ---> Soft Reset Occurred
						; Bit 3 ---> SPARE
						; Bit 2 ---> SPARE
						; Bit 1 ---> SPARE
						; Bit 0 ---> Flash Programming Enabled
.comm_stat:		ds.b 1			; Communication interface status
						; Bit 7 ---> SPI Data Xfer Flag
						; Bit 6 ---> Write Collision Error
						; Bit 5 ---> Overrun Error
						; Bit 4 ---> Mode Fault
						; Bit 3 ---> Compare Error
						; Bit 2 ---> SPARE
						; Bit 1 ---> SPARE
						; Bit 0 ---> SPARE 

.pend:			ds.b 1			; Action Pending until no Chip Select
						; Bit 7 ---> Hard Reset
						; Bit 6 ---> Soft Reset
						; Bit 5-1 -> Unused
						; Bit 0 ---> Flash Update Pending
.temp			ds.b 1			; Variable for general use


 #ifdef altTable
 #define REV2XX	0 			
 #endif

 
						 						
 BYTES  	
  	SEGMENT 'iap_buffer_ram'
  			                    
.flashkey1   		ds.b    1 		; Flash key #1 sent by the Master device
.flashkey2   		ds.b    1 		; Flash key #2 sent by the Master device
.nbytes  		ds.b    1 		; Number of bytes to be written to flash
.flashaddr 		ds.w	1 		; Long address required for accessing flash
.e2data  		ds.b	32		; Data to be programmed, up to 32 bytes
.cnt			ds.b	1		; Count used by download uCode routine
.ck_sum			ds.b    1		; Check sum used by download uCode routine
.fptrCal		ds.w	1  
 WORDS   
 	SEGMENT  'iap_&_stack_ram'
flashcode	ds.b    60
 
;****************************************************************************************
; Segment:     Checksum                                                              
; Description: Fixed area for the application code checksum. The CRC routine will do
;              a checksum on the area E003-F6FF, when completed it will compare it 
;              against the 3 bytes starting at location E000 looking for a match. If
;              the checksum is invalid the ST7 will go into a download mode waiting
;              for valid application code.
;**************************************************************************************** 
 WORDS
 	SEGMENT 'Checksum'
.CS0			dc.b	$FF		; (MSB) These bytes will be automaicaly
.CS1			dc.b	$FF		;       caculated and inserted into
.CS2			dc.b	$FF		; (LSB) the rndio.s19 S-Record file
			dc.b	$00             ; These bytes fill out the check_sum
			dc.l	$11223344	;  segment to 16 bytes so that subsequent 
			dc.l	$55667788	;  S-Record address will fall on modulo
			dc.l	$99AABBCC	;  16 addresses

;****************************************************************************************
; Segment:      Application_code  
; Description:  This segment is intended for Application microcode only, which can be 
;               updated over the RabbitNet network.     	   
;****************************************************************************************   
 
 WORDS
	SEGMENT 'Application_code'

;**************************************************************************************** 
; Segment:      Application_code
; Routine: 	Startup code  
; Description:  Does some intialization before jumping to MAIN. Also provides an 
;               entry point for both a software and hardware reset.  	   
;****************************************************************************************	
.hard_reset:	clr	rn_stat			; Clear 1st and
		clr	cmd07_stat		;  2nd level status if hard reset
		call	brd_hard_init		; Board Level Hard Initialization
.soft_reset:	ld	a,cmd07_stat		; Entry here preserves cmd07 status bits 5 & 6 
		and	a,#$60			;  (SRES) which indicates soft reset occurred
		ld	cmd07_stat,a		; Save cmd07 status
		btjf	SICSR,#WDGRF,no_wdt 	; Check if HWDT timeout	
		bset	cmd07_stat,#HRES 	; Yes.. set HRES occurred in 2nd level status
		bset	rn_stat,#WDT		; Yes.. Set WDT in 1st level status
no_wdt: 	clr	SICSR			; Disable Sys Integrity Interrupts and clear WDT reset flag  
		call    brd_soft_init           ; Board Level Soft Initialization 
		call	brd_init		; Main Board Initialization
		ld	a,swdt_ival     	; Initialize the
		ld	swdt_reload,a 		;  SWDT reload value
		ld	swdt_tmr,a		;  and SWDT down counter
		clr	act_tmr			; Clear Activity LED timer		
		clr	pend			; Clear action pending bits
		clr	comm_stat		; Clear communitions status byte
		clr	flashkey1		; Clear the two FLASH write
		clr	flashkey2		;  enable keys
 		ld	A,#$0E			; Set RTC interrupt for 25ms
 		ld	MCCSR,A			;  by loading the main clock control 
 						;  register
		ld	a,WDGCR			; Load initial timeout into WDT
		or	a,#$7f			;  while maintaining the 
		ld	WDGCR,a			;  enable/disable (bit7) 
		call 	spi_init		; Initialize SPI port
		bset	rn_stat,#READY 		;  indicates slave ready
	
 		ld	a,rn_stat
		ld	SPIDR,a			; Prepare 1st status byte
		jp	main			; Start Running Board Unique Application  
						;

;****************************************************************************************
;     Common Register Set based Commands 
;**************************************************************************************** 

;+------------------------------------------------------------------------------+
;|	CMD 0 Registers 0 - 7                                                   |
;|    						                                |
;+------------------------------------------------------------------------------+
						; DIAG ECHO, REFLECT MASTER XMIT DATA 
.c0r00:		ld	a,rn_cmd		; Send back command on next MDO
c0r00a:		call	rn_xfer			; Xmit/Rec Reg A
		jra	c0r00a			; Continue until no Slave Select
						;		  

.c0r02:		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer			; Send safe data, get 1st byte of key
		ld	flashkey1,a		; Save 1st key
		call	rn_xfer			; Return 1st byte key get 2nd byte
		ld	flashkey2,a		; Save 2nd key
		call	rn_xfer			; Get Product Revision
		ld	{e2data+0},a		;  and save in buffer
 		 		
 		ld	a,#prod_rev.h 		; Load pointer to flash save registers
		ld	{flashaddr+0},a		; Load high byte of address
		ld	a,#prod_rev.l	 	;
		ld	{flashaddr+1},a		; Load low byte of address
		ld	a,#$01			; Load the number of bytes to write
		ld	nbytes,a		;  into FLASH
		bset	pend,#FLASH		; Set flash write pending 
		jp	cmd_exit		; Must exit after flash write 

						; c0r03 (Update microcode in FLASH)	
.c0r03:		ld	a,SPIDR	 		; Return previous byte
		call	rn_xfer			; Send previous, get 1st key (key_0)
		ld	flashkey1,a		; Save 1st key
		call	rn_xfer			; Send previous, get 2nd key (key_1)
		ld	flashkey2,a		; Save 2nd key
		call	uC			; Call uCode update process
						; FOLLOWING MICROCODE UPDATE PROCESS PROCESSING
						; THE SLAVE IS HARD RESET AND EXECUTION BEGINS
						; AT "MAIN:"
						; If we get here key was in error so 
		clr	flashkey1		;  make sure both 1st and
		clr	flashkey2		;  2nd flash keys are cleared
		jp	cmd_rej			;  and reject command
						
						; Write PRODUCT UNIQUE ID (4 bytes)
.c0r04:		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer			; Send safe data, get 1st byte of key
		ld	flashkey1,a		; Save 1st key
		call	rn_xfer			; Return 1st byte key get 2nd byte
		ld	flashkey2,a		; Save 2nd key
		call	rn_xfer			; Get Serial Number byte 0
		ld	{e2data+0},a		;  and save in buffer
		call	rn_xfer			; Get Serial Number byte 1
		ld	{e2data+1},a		;  and save in buffer 
		call	rn_xfer			; Get Serial Number byte 2
		ld	{e2data+2},a		;  and save in buffer
		call	rn_xfer			; Get Serial Number byte 3
		ld	{e2data+3},a		;  and save in buffer
 		 		
 		ld	a,#prod_sn.h 		; Load pointer to flash save registers
		ld	{flashaddr+0},a		; Load high byte of address
		ld	a,#prod_sn.l	 	;
		ld	{flashaddr+1},a		; Load low byte of address
		ld	a,#$04			; Load the number of bytes to write
		ld	nbytes,a		;  into FLASH
		bset	pend,#FLASH		; Set flash write pending 
		jp	cmd_exit		; Must exit after flash write 
	
	                                        ; WRITE SWDT RELOAD VALUE
.c0r05:		ld	a,SPIDR 		; Send back previous receive byte
		call	rn_xfer			; Send to master and get new byte
		ld	swdt_reload,a 		; Save the reload lo byte
		inc	rn_cmd			; Move to next command
		jp	exitcmd			;  and go process it
						;
						; WRITE CMD 0 REG07 STAT REG
.c0r07:		ld	a,SPIDR			; Send back previous receive byte
		call	rn_xfer			;  send to master and receive new stat data
		ld	temp,a
		and	a,#$C0			; Retain HRES and SRES bits and put in pending
		ld	pend,a			;  for subsequent processing on command exit
		
		btjt    temp,#SWDT,c0r07a 	;If HWDT is enabled it can not
		bres	temp,#SWDT

c0r07a:		ld	a,temp			; Get command byte again
		and	a,#$30			; Mask off the HRES and SRES as they are always	
						;  cleared by a read of cmd07 stat
		or	a,cmd07_stat		; Set desired bits of the command  
		ld	cmd07_stat,a		; Save for status and control		 	
		jp	exitcmd			; Go process this next data byte and/or cmd


;+------------------------------------------------------------------------------+
;|	CMD 1 Registers 0 - 7    						|
;+------------------------------------------------------------------------------+ 
.c1r00:						; READ TIMING PARAMETERS
		ld	a,{sig_regs + 0}	; Get SS2CMD parameter
		call	rn_xfer			;  and send to master
		ld	a,{sig_regs + 1}	; Get CMD2DAT parameter
		call	rn_xfer			;  and send to master
		ld	a,{sig_regs + 2}	; Get DAT2DAT parameter
		call	rn_xfer			;  and send to master
		ld	a,{sig_regs + 3}	; Get DAT2SS parameter
		call	rn_xfer			;  and send to master
		ld	a,{sig_regs + 4}	; Get SS Active Max parameter
		call	rn_xfer			;  and send to master

		ld	a,{prod_id + 1} 	; Get product ID register lo
		call	rn_xfer			;  and send to master
                ld	a,{prod_id + 0} 	; Get product ID register hi
		call	rn_xfer			;  and send to master
						; READ PRODUCT REVISION (1 BYTE)
		ld	a,prod_rev		; Get product revision register 
		call	rn_xfer			;  and send to master
						; READ PRODUCT CODE REVISION (1 BYTE)
		ld	a,{code_rev + 1}	; Get code revision register 
		call	rn_xfer			;  and send to master  
		ld	a,{code_rev + 0}	; Get code revision register 
		call	rn_xfer			;  and send to master
						; READ PRODUCT UNIQUE ID (4 bytes)
		ld	a,{prod_sn + 0} 	; Get product serial number byte0
		call	rn_xfer			; Send byte to master
		ld	a,{prod_sn + 1} 	; Get product serial number byte1
		call	rn_xfer			; Send byte to master
		ld	a,{prod_sn + 2} 	; Get product serial number byte2
		call	rn_xfer			; Send byte to master
		ld	a,{prod_sn + 3} 	; Get product serial number byte3
		call	rn_xfer			; Send byte to master
						; READ SWDT TIMOUT PERIOD
		ld	a,swdt_tmr 		; Get SWDT down counter
		call	rn_xfer			;  and send to master
						; READ COMMUNICATIONS STATUS
		ld	a,comm_stat		; Setup to send comm status
		call	rn_xfer			; Send on next MDO
		clr	comm_stat		; Clear status byte after read
						; c1r07..RD CMD07 STATUS REGISTER
		ld	a,cmd07_stat		; Get current cmd07 status byte
		call	rn_xfer			; Send cmd07 status to master
		bres	cmd07_stat,#HRES 	; HRES and SRES status bits are
		bres	cmd07_stat,#SRES 	;  are cleared on read of cmd07 status
		jp	exitcmd			; Go process this next data byte and/or cmd		
 						; READ PRODUCT ID (2 bytes)			

.c1r01:		ld	a,{prod_id + 1} 	; Get product ID register lo
		call	rn_xfer			;  and send to master
                ld	a,{prod_id + 0} 	; Get product ID register hi
		call	rn_xfer			;  and send to master
		jp	exitcmd			; Go process this next data byte and/or cmd
						; READ PRODUCT REVISION (1 BYTE)
.c1r02:		ld	a,prod_rev		; Get product revision register 
		call	rn_xfer			;  and send to master
		jp	exitcmd			; Go process this next data byte and/or cmd
						; READ PRODUCT CODE REVISION (1 BYTE)
.c1r03:		ld	a,{code_rev + 1}	; Get code revision register 
		call	rn_xfer			;  and send to master  
		ld	a,{code_rev + 0}	; Get code revision register 
		call	rn_xfer			;  and send to master
		jp	exitcmd			; Go process this next data byte and/or cmd
						; READ PRODUCT UNIQUE ID (4 bytes)
.c1r04:		ld	a,{prod_sn + 0} 	; Get product serial number byte0
		call	rn_xfer			; Send byte to master
		ld	a,{prod_sn + 1} 	; Get product serial number byte1
		call	rn_xfer			; Send byte to master
		ld	a,{prod_sn + 2} 	; Get product serial number byte2
		call	rn_xfer			; Send byte to master
		ld	a,{prod_sn + 3} 	; Get product serial number byte3
		call	rn_xfer			; Send byte to master
		jp	exitcmd			;  and go process it		
						; READ SWDT TIMOUT PERIOD
.c1r05:		ld	a,swdt_tmr 		; Get SWDT down counter
		call	rn_xfer			;  and send to master
		jp	exitcmd			; Go process this next command
		
						; READ COMMUNICATIONS STATUS
.c1r06:		ld	a,comm_stat		; Setup to send comm status
		call	rn_xfer			; Send on next MDO
		clr	comm_stat		; Clear status byte after read
		jp	exitcmd			;  and go process it

						; c1r07..RD CMD07 STATUS REGISTER
.c1r07:		ld	a,cmd07_stat		; Get current cmd07 status byte
		call	rn_xfer			; Send cmd07 status to master
		bres	cmd07_stat,#HRES 	; WDT, HRES and SRES status bits are
		bres	cmd07_stat,#SRES 	;  are cleared on read of cmd07 status
		bres	rn_stat,#WDT
c1r07b:		jp	exitcmd
;
;********************************************************************************    

;+------------------------------------------------------------------------------+
;|										|
;|			R-NET TRANSFER						|
;|										|
;+------------------------------------------------------------------------------+
;********************************************************************************
; 	ROUTINE:	RN_XFER
;	PARAMETERS:	ON ENTRY A REG CONTAINS DATA TO BE SENT TO MASTER
;			ON EXIT A REG CONTAINS DATA SENT TO SLAVE
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;********************************************************************************
;
.rn_xfer:	btjt	SPICSR,#OVR,rn_xfer20	; Check for overrun (OVR is cleared)
		ld	SPIDR,a			; Prepare so send data to master
rn_xfer10:	btjt	SPICSR,#SPIF,rn_xfer50	; NO..Then check for next MDO while
		btjf	PBDR,#SS,rn_xfer10 	;  monitoring Slave Select
		pop	a			; Adjust
		pop	a			;  stack pointer and
		jp	cmd_exit		;  exit the current command
rn_xfer20:	ld	a,SPIDR			; Insure xfer flag is cleared
		bset	comm_stat,#OVR		; Overrun condition exists, Save overrun
		ld	a,comm_stat		;  status bit
		or	a,SPICSR		; Update the existing comm status with that 
		ld	comm_stat,a		;  just obtained
		bset	rn_stat,#COMM_ERR	; Set Communications Error in 1st level status 
		bset	rn_stat,#REJECT		; Set Command REJECT in 1st level status status		
rn_xfer30:	btjt	SPICSR,#SPIF,rn_xfer40	; Check for next MDO while
		btjf	PBDR,#SS,rn_xfer30 	;  monitoring Slave Select
		pop	a			; No Slave Select so adjust
		pop	a			;  stack pointer and
		jp	cmd_exit		;  exit the current command
rn_xfer40:	ld	a,SPIDR			; Clear xfer flag and throw away data
		jra	rn_xfer30		; Continue until command ends with no Slave Select
rn_xfer50:	ld	a,SPIDR			; Get master receive data
		ret				;  and return

;********************************************************************************
; 	ROUTINE:	EXITCMD
;	PARAMETERS:	None
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;********************************************************************************
;
.exitcmd:	btjt	SPICSR,#OVR,exitcmd20	; Check for overrun (OVR is cleared)
exitcmd10:	btjt	SPICSR,#SPIF,exitcmd50	; NO..Then check for next MDO while
		btjf	PBDR,#SS,exitcmd10 	;  monitoring Slave Select
		jp	cmd_exit		;  exit the current command

exitcmd20:	ld	a,SPIDR			; Insure xfer flag is cleared
		bset	comm_stat,#OVR		; Overrun condition exists, Save overrun
		ld	a,comm_stat		;  status bit
		or	a,SPICSR		; Update the existing comm status with that 
		ld	comm_stat,a		;  just obtained
		bset	rn_stat,#COMM_ERR	; Set Communications Error in 1st level status 
		bset	rn_stat,#REJECT		; Set Command REJECT in 1st level status status		
exitcmd30:	btjt	SPICSR,#SPIF,exitcmd40	; Check for next MDO while
		btjf	PBDR,#SS,exitcmd30 	;  monitoring Slave Select
		jp	cmd_exit		;  exit the current command

exitcmd40:	ld	a,SPIDR			; Clear xfer flag and throw away data
		jra	exitcmd30		; Continue until command ends with no Slave Select
exitcmd50:	ld	a,SPIDR			; Get master receive data
		jp	exitcmd
		
		
;
;********************************************************************************
;		COMMAND PROCESSING
;********************************************************************************
		                                ; SPI NEW COMMAND
.spi_nc:	btjt	SPICSR,#SPIF,spi_nc5	; Read SPICSR then SPIDR to reset SPIF
spi_nc5:	btjf	SPIDR,#7,spi_nc10 	; Check for Slave command
                jp	cmd_rej			; Reject HUB commands
spi_nc10:	ld	a,rn_stat		; Clear all the
		and	a,#$E1			;  transient status
		ld	rn_stat,a		;  bits
		ld	a,SPIDR			; Get Command Byte
		ld	rn_cmd,a		; Save the Rabbit Net Command
				
.spi_pc: 					; SPI PROCESS COMMAND
		ld	a,rn_cmd		; Process next byte or command
		and	a,#$7F			; Mask off bit7
		sll	a			; Multiply by 2 and add 1 to
		inc	a			;  form index to label address low byte 
		ld	x,a			;  and put in index reg X
		ld	a,(spi_jtbl,x)		; Get low address of label
		push	a			;  and put on stack
		dec	x			; Index to high byte of 
		ld	a,(spi_jtbl,x)		;  label and
		push	a			;  put on stack
		ret				; Now branch to label

.cmd_rej:	ld	a,SPIDR			; Send back the previous byte (command)
cmd_rej_10:	call	rn_xfer			; Yes..wait for next MDO and
		bset	rn_stat,#REJECT		;  set REJECT in 1st level status and
		jra	cmd_rej			;  continue reject until no Chip Select
						;
.cmd_exit:	ld	a,swdt_reload		; Reload the software WDT at
		ld	swdt_tmr,a		;  the end of every command
		btjt	pend,#HRES,cmd_exit_10 	; Check for hard reset pending
		btjt	pend,#SRES,cmd_exit_30 	; Check for soft reset pending
		btjt	pend,#FLASH,cmd_exit_40 ; Check for FLASH write pending
		btjt    pend,#USER,cmd_exit_60  ; Check for USER handler to be executed
		ld	a,SPICSR		; Clear the SPIF flag again (just for insurance)
		ld	a,rn_stat		;  then return
		ld	SPIDR,a			;  from interrupt routine to
		iret				;  main loop
						; Return from int routine since no chip select
						;
cmd_exit_10:	sim				; Hard reset pending,  disable all interrupts
		ld	a,#$FF			; Enable hardware WDT just in case
		ld	WDGCR,a			;  it was not enabled  
		ld	a,#$80			; Then clear T6 bit to force
		ld	WDGCR,a			;  hardware reset
cmd_exit_20:	jra	cmd_exit_20		; Execution will begin again at "main:"
cmd_exit_30:	sim				; Disable interrupts
		bset	cmd07_stat,#SRES 	; Set soft reset occurred in 2nd level status
		bset	rn_stat,#WDT		; Set WDT in 1st level status
		rsp				; Reset the stack pointer
		jp	soft_reset		; Restart microcode at "soft_reset:"
						;	
cmd_exit_40:	sim				; Disable interrupts
		bres	pend,#FLASH		; Clear FLASH update pending
		call	flashwrite		; Call flash_wr to process key and wr data
		rim				; ReEnable interrupts
		tnz	a			; Test flashwrite return value (0=OK,FF=BAD)
		jreq	cmd_exit_50		; OK..so exit with existing status
		bset	rn_stat,#REJECT		; BAD..so set Reject bit in 1st level status

cmd_exit_50:	btjf	SPICSR,#SPIF,cmd_exit_55
		ld	a,rn_stat		; Set slave status
		and	a,#$BF			;  by turning off Ready
		or	a,#$80			;  and setting Busy and CMD Reject
		ld	SPIDR,a			; Send on next MDO

cmd_exit_55:	btjf	PBDR,#SS,cmd_exit_50 	;  monitoring Slave Select
		ld	a,SPICSR		; Clear the SPIF flag again (just for insurance)
		bres	rn_stat,#BUSY   
		bset	rn_stat,#READY
		ld	a,rn_stat		;  then return
		ld	SPIDR,a			;  from interrupt routine to
		iret				;  main loop

cmd_exit_60:    bres	pend,#USER		; Clear FLASH update pending
		call    user_handler 
		tnz	a			; Test user handler return value (0=OK,FF=BAD)
		jreq	cmd_exit_70		; OK..so exit with existing status
		bset	rn_stat,#REJECT		; BAD..so set Reject bit in 1st level status
cmd_exit_70:	ld	a,SPICSR		; Clear the SPIF flag again (just for insurance)
		ld	a,rn_stat		;  then return
		ld	SPIDR,a			;  from interrupt routine to		
		iret

;****************************************************************************************
;     INTERRUPT SUB-ROUTINES SECTION
;****************************************************************************************
;+------------------------------------------------------------------------------+
;|	SPI Interrupt Routine    						|
;+------------------------------------------------------------------------------+ 	 
.spi_rt:	btjt	SPICSR,#SPIF,spi_rt_10 	; Wait and/or clear xfer flag
		btjf	PBDR,#SS,spi_rt		; Test for RN Slave Select
		ld	a,rn_stat		; No CS so setup 1st level status
		ld	SPIDR,a			;  to be sent on next MDO
		iret				;  and return since no CS

spi_rt_10:	ld	a,#2			; Set Activity Delay for
		ld	act_tmr,a		;  two tics of 25ms each 
		btjf	rn_stat,#BUSY,spi_rt_20 ; Is Slave currently busy
spi_rt_15:   	btjf	SPICSR,#SPIF,spi_rt_17	
		ld	a,SPIDR			;  with busy set
		ld	a,rn_stat		; Yes..Return 1st level status
		ld	SPIDR,a			;  with busy set 
spi_rt_17:	btjf	PBDR,#SS,spi_rt_15	; Test for RN Slave Select            
		iret				; Return to outer loop		

spi_rt_20:	jp	spi_nc			; Go process command


;+------------------------------------------------------------------------------+
;|	Timer Interrupt Routine    						|
;+------------------------------------------------------------------------------+ 	 
						;
.mcc_rt:	ld	a,MCCSR			; Clear the MCC interrupt flag
		call	interval_tmr		; R-Net Board Interval Timer 
		btjf	cmd07_stat,#SWDT,mcc_30 ; Check if SWDT enabled
		tnz	swdt_tmr		; Test SWDT down counter for zero
		jrne	mcc_20			; Not zero..Decrement SWDT
		sim				; Yes zero..Disable all interrupts
		rsp				; Reset stack pointer
		bset	cmd07_stat,#SRES 	; SWDT is zero set 2nd level status bit (SWDT)
		bset	rn_stat,#WDT		;   and the 1st level status bit (WDT)
		jp	soft_reset		;  Go to soft start
mcc_20:		dec	swdt_tmr		; Decrement SWDT down counter
mcc_30:		iret				; Done      		
      

;+------------------------------------------------------------------------------+
;|	Interrupt routine for vectors not being used				|
;+------------------------------------------------------------------------------+ 

dummy:		iret    			; Empty subroutine	


      
;****************************************************************************************  
; Segment:      Protected
; Routine: 	SPI Communication Intialization   
; Description:  This routine initializes the SPI port for communication over 
;               Rabbitnet network.   
;****************************************************************************************               
	;----------------------------------------------------------------------------
	; Protect Segment contains code which will not reloaded in the field.    
	;----------------------------------------------------------------------------   
 WORDS                    
  	SEGMENT 'Protected'

.spi_init:	clr	a			; Put x'FF' into SPI data register
		cpl	a			;  so any master transmission prior
		ld	SPIDR,a			;  to enabling interrupts will return
		clr	a			;  status indicating no device present.
						; For SPI Slave; SSM=SSI=0
		ld	SPICSR,a		; Load SPI Ctrl/Status Reg
		ld	a,#$CC			; Setup Slave,clock phase, polarity, interrtuts
		ld	SPICR,a			; and enable SPI alternate pin functions 
        					;
		ld	a,SPICSR		; Purge the R-Net receive 
		ld	a,SPIDR			;  buffer and clear
		ld	a,SPICSR		;  the xfer flag
		ld	a,SPIDR			; (Rec buffer is 2 bytes)
        	ret 


	


;****************************************************************************************
; Segment:      Protected
; Routine: 	Flash Write Driver   
; Description:  This ram based flash driver handles writing to the ST7 microprocessor 
;               on-chip Flash. The flashwrite routine handles copying this driver to 
;               ram and jumping to it for execution.
;****************************************************************************************               
flash_driver:   
    		clr  	x
flash_enable: 
		bset 	FCSR,#1	 		; Enable Emul. EEPROM latches
		btjt	FCSR,#1,flash_loop	; Are Flash writes enabled..Yes continue
		ld	a,#$FF			; No..Set bad status
		ret				;  and return
flash_loop:		
		ld 	a,(e2data,x)
		ld 	[flashaddr.W],a		; Set address/data to be programmed   
		inc   	{flashaddr+1}
		jrne  	flash_bypass
		inc   	{flashaddr+0}
flash_bypass:
		dec	nbytes
		jreq	flash_prog
		inc   	x         
		ld    	a,#$1F
		and   	a,{flashaddr+1}
		jrne	flash_loop          
flash_prog:	
		bset	FCSR,#0     		; Signal the flash to write the data block
flash_wait:
		btjf	cmd07_stat,#HWDT,flash_no_hwdt ; Test for HWDT enabled
		ld	a,#HWDT_TIMEOUT		; Load max HWDT count
		ld	WDGCR,a			;  into watch dog down counter
flash_no_hwdt:
		btjf	SPICSR,#SPIF,flash_no_xfr ;
		ld	a,rn_stat		; Prepare programming status
		and	a,#$BF			;  by turning off Ready
		or	a,#$80			;  and setting Busy and CMD Reject
		ld	SPIDR,a			; Send on next MDO
flash_no_xfr:
		btjt 	FCSR,#0,flash_wait  	; Wait for end of programming (~5ms)	        
                tnz	nbytes                  ; Check if there's more bytes to be written
		jrne 	flash_enable		; A reg not zero 		

		clr	a
		ret                             ; Return with A reg equal to zero
flash_end:	; Must be after the last instruction!      
; Calculation of number of bytes for the Flash Driver routine 	      
FSIZE equ {flash_end - flash_driver}  
    
;--------------------------------------------------------------------------------------
; Segment:	Protected
; Subroutine: 	Support Routine for Flash Driver  
; Description:  This routine copies the flashdriver to RAM and then jumps to it for
;               flash write operations. 	
;--------------------------------------------------------------------------------------
.flashwrite
		ld	a,flashkey1		; Add the two keys and check sum for 
		add	a,flashkey2		;  X'104' = X'56' + X'AE'
		jrnc	flashkeyerr		; Check bit 8 for a 1, No..so error 
		cp	a,#$04			; Yes.. Then check low 8 bits
		jreq	flashkeyok		; Yes.. key1 and key2 are good
flashkeyerr:					; No.. key1 or key2 is bad
		ld	a,#$FF			; Set BAD status
		ret				;  and return

flashkeyok:					;
		btjt	cmd07_stat,#FLASH,flashwrok ; Check if flash is previously enabled    
 		ld	a,flashkey1		; NO..Apply the
 		ld	FCSR,a			;  1st key
 		clr	flashkey1		;  then erase
 		ld	a,flashkey2		; Apply the
 		ld	FCSR,a			;  2nd key
 		clr	flashkey2		;  then erase
 		bset	cmd07_stat,#FLASH	; Indicate flash OK for writes   
flashwrok:
		ld	a,rn_stat		; Prepare programming status
		and	a,#$BF			;  by turning off Ready
		or	a,#$80			;  setting Busy and CMD Reject
		ld	SPIDR,a			; Send on next MDO   				  
		ld 	x,#FSIZE.L   		; Load reg X with length of flash driver	 
flashcopy: 					; Copy programming software driver
		btjf	SPICSR,#SPIF,flash_no_stat ;
		ld	a,rn_stat		; Prepare programming status
		and	a,#$BF			;  by turning off Ready
		or	a,#$80			;  and setting Busy and CMD Reject
		ld	SPIDR,a			; Send on next MDO
flash_no_stat:
		ld 	a,(flash_driver,x)      ; into RAM from address 0100h
		ld 	(flashcode,x),a
		dec 	x
		
		jrpl 	flashcopy
		jp 	flashcode 		; Call Flash Driver located in RAM
                                                                                         
                                                                                         
;****************************************************************************************
; Segment:      Protected
; Routine: 	Application Microcode Download Routine  
; Description:  This routine downloads new application microcode via the Rabbitnet
;               network.  
;****************************************************************************************
.uC:		ld	a,flashkey1		; Add the two keys and check sum for 
		add	a,flashkey2		;  X'104' = X'56' + X'AE'
		jrnc	uC_err			; Check bit 8 for a 1, No..so error 
		cp	a,#$04			; Yes.. Then check low 8 bits
		jreq	uC_01			; Yes.. key1 and key2 are good
						; No.. key1 or key2 is bad
uC_err:		ld	a,#$FF			; Set BAD status
		ret				;  and return

uC_01:		sim				; Disable all interrupts                                        ;
uC_05:		ld	a,#$40			; Load Ready Status
		ld	SPIDR,a			;  to send on next MDO
uC_10:		call	uC_chr			; Get ASCII Data	
		cp	a,#'S'			; Check for beginning of S-Record
		jrne	uC_05			; No..Continue checking
		ld	a,#$40			; Load Ready Status
		ld	SPIDR,a			;  to send on next MDO
		call	uC_chr			; Get Data
		cp	a,#'1'			; In the 2nd byte a 1
		jreq	uC_30			; Yes..Then this is a data record to process
		cp	a,#'9'			; No..Then is it a 9
		jrne	uC_05			; No..Then start again since only type 1 and 9 S-Records
						;  are recognized
		jp	uC_90			; Yes..Then this is the last S-Record (Set PC)
	                                	;
uC_30:		call	uC_bin			; Get count high nibble
		swap	a			;  and save in temporary variable
		ld	cnt,a			;  cnt
		call	uC_bin			; Get count low nibble 
		or	a,cnt			;  and assemble whole			
		ld	cnt,a			;  byte count 
		ld	ck_sum,a		; Start check sum with the count byte
		dec	cnt			; Decrement S-Record byte counter
						;
		call	uC_bin			; Get high nibble of high address byte
		swap	a			;
		ld	{flashaddr+0},a		; 
		call	uC_bin			;
		add	a,{flashaddr+0}		; Get low nibble of high address byte
		ld	{flashaddr+0},a		; Assemble whole byte and place in wr buffer
		add	a,ck_sum		; Update check sum
		ld	ck_sum,a		;  with high address byte
		dec	cnt			; Decrement S-Record byte counter
						;
		call	uC_bin			; Get high nibble of low address byte
		swap	a			;
		ld	{flashaddr+1},a		; 
		call	uC_bin			;
		add	a,{flashaddr+1}		; Get low nibble of low address byte
		ld	{flashaddr+1},a		; Assemble whole byte and place in wr buffer
		add	a,ck_sum		; Update check sum
		ld	ck_sum,a		;  with low address byte
		clr	nbytes			; Clear data byte count
		clr	x			; Clear data wr data buffer index 
uC_40:		dec	cnt			; Decrement S-Record byte counter
		tnz	cnt			; Check if S-Record data byte processing is complete
		jreq	uC_50			; Yes..process check sum
		call	uC_bin			; No..Get high nibble of data byte
		swap	a			;
		ld	(e2data,x),a		;
		call	uC_bin			; Get low nibble of data byte
		add	a,(e2data,x)		; Assemble whole byte and place in flash wr data buffer
		ld	(e2data,x),a		;
		add	a,ck_sum		; Update the
		ld	ck_sum,a		;  check sum
		inc	x			; Move data pointer to next position in wr data buffer
		inc	nbytes			; Increment the number of bytes to wr to flash
		jra	uC_40			; Do the next byte
						;
uC_50:		call	uC_bin			; Get high nibble of check sum
		swap	a			;
		ld	cnt,a			;
		call	uC_bin			; Get low nibble of check sum
		add	a,cnt			; Assemble whole check sum byte temporarily in cnt
		add	a,ck_sum		; Compare the transmitted and caculated
		inc	a			;  check sums
		jrne	uC_70			; If check sum is not zero record is in error
		btjt	cmd07_stat,#FLASH,uC_60 ; Check if flash is previously enabled 
		call	flashwrite		; No..Check sum is OK so write the S-Record data to flash
		cp	a,#$00			; Check if flash write completed OK
		jrne	uC_70                   ;  If return code not zero process error
		ld	a,#$40			; Load Ready Status
		ld	SPIDR,a			;  into SPIDR and send back on next MDO
		jp	uC_10			; Start processing next record
uC_60:		call	flashwrok		; Yes..Write record without using flashkeys
		cp	a,#$00			; Check if flash write completed OK
		jrne	uC_70			;  If return code not zero process error
		ld	a,#$40			; Load Ready Status 
		ld	SPIDR,a			;  into SPIDR to send on next MDO
		jp	uC_10			; Start processing next record
						;
uC_70:		ld	a,#$42			; Load Ready, Cmd Reject Status
		ld	SPIDR,a			;  into SPIDR to send on next MDO
		jp	uC_10			;  start again
		                        	;
uC_90:		ld	a,SPIDR			; Once type 9 S-Record is detected throw away
		btjf	PBDR,#SS,uC_90		;  any subsequent bytes until no Slave Select
		ld	a,#$FF			; Then force
		ld	WDGCR,a			;  a hard
		ld	a,#$80			;  reset of the
		ld	WDGCR,a			;  slave board
uC_99:		jra	uC_99			; Wait here until execution restarts after reset
	
uC_chr:		btjf	SPICSR,#SPIF,uC_chr	; Wait for character to come in
		ld	a,SPIDR			;  put it in A reg
		ret                             ;
						;	
uC_bin:		ld	a,#$40			; Load Ready status and
		ld	SPIDR,a			;  return on next MDO
uC_bin10:                               	;
		btjf	SPICSR,#SPIF,uC_bin10	; Wait for char to come in
		ld	a,SPIDR 		;  and convert it from ASCII 
		cp	a,#'9'			;  to hex, placing it in
		jrugt	uC_bin20		;  the low 4 bits and leaving
		sub	a,#'0'			;  the high 4 bits unchanged
		ret				;
uC_bin20:                               	;
		sub	a,#'7'			;
		ret				;

;****************************************************************************************
; Segment:      Protected
; Routine: 	Application Microcode CRC Verification  
; Description:  This routine calculates the FLASH checksum and compares correct one 
;               stored at CS0-CS2.  If the comparison is ok, the program jumps to main, 
;               if not OK the program waits for a microcode download. 	   
;****************************************************************************************
crc_check:      call	crcEnable		; Get CRC enable flag, returned in REG A 
		cp	a,#$FF                  ; Check if CRC check is enabled
		jreq	crc_check10	        ; Jump if enabled, otherwise start application
                jp	reset_jp                ; CS OK, Start normal microcode execution

crc_check10:	call    crc			; Calculate the checksum
		ld      a,{ch_sum+2}   	 	; Get calculated checksum (LSB)
		cp      a,CS2          		; Compare {ch_sum+2} with CS2
      		jrne    cs_err          	; If not equal -> check sum error
       		ld      a,{ch_sum+1}    	; Get calculated checksum
   		cp      a,CS1                   ; Compare {ch_sum+1} with CS1
       		jrne    cs_err                  ; If not equal -> check sum error
        	ld      a,ch_sum		; Get calculated checksum (MSB)
        	cp      a,CS0                   ; Compare ch_sum with CS0
        	jrne    cs_err                  ; If not equal -> check sum error
		jp	reset_jp                ; CS OK, Start normal microcode execution
                
cs_err: 	clr	cmd07_stat		; Clear FLASH Wr Enabled flag (bit 0)
		call	spi_init		; Initialize SPI port
dnld10:		ld	a,#$40			; Setup to send READY on
		ld	SPIDR,a			;  next MDO
dnld20: 	btjt	SPICSR,#SPIF,dnld25	; Wait for command
		btjf	PBDR,#SS,dnld20		;
		jra	dnld10			;
dnld25:		ld	a,SPIDR			;  and put in A reg 
		cp	a,#$03			; Check for download command oct'003'
		jreq	dnld30			; Yes..Wait for 1st key
dnld27:		ld	a,#$42 			; No..Reject remainder
		ld	SPIDR,a			;  of command
dnld29:		btjt	SPICSR,#SPIF,dnld27	;
		btjf	PBDR,#SS,dnld29		;
		jra	dnld10			;
dnld30:		ld	a,#$40			;
		ld	SPIDR,a			;
dnld35:		btjt	SPICSR,#SPIF,dnld40	; Wait on next MDO 
		btjf	PBDR,#SS,dnld35		;
		jra	dnld10			;	
dnld40:		ld	a,SPIDR			; Send back command, get 1st key
		ld	flashkey1,a		; Save 1st key
		ld	a,#$40			;
		ld	SPIDR,a			;
dnld50:		btjt	SPICSR,#SPIF,dnld60	; Wait on next MDO
		btjf	PBDR,#SS,dnld50		;
		jra	dnld10			;
dnld60:		ld	a,SPIDR			; Send 1st key, get 2nd key
		ld	flashkey2,a		; Save 2nd key
		call	uC			; Call uCode update process
						; FOLLOWING MICROCODE UPDATE PROCESS THE
						; SLAVE IS HARD RESET AND EXECUTION BEGINS
						; AT "hard_reset:"
						; If we get here key was in error so 
		clr	flashkey1		;  make sure both 1st and
		clr	flashkey2		;  2nd flash keys are cleared
dnld70:		ld	a,#$42			; Setup status for
		ld	SPIDR,a			;  ready/cmd reject
dnld75:		btjt	SPICSR,#SPIF,dnld70	; Throw away all subsequent bytes until
		btjf	PBDR,#SS,dnld75		;  Slave Select goes inactave
		jra	dnld10			; Then go wait for new download command
	
;-------------------------------------------------------------------------------------- 
; Segment:      Protected
; Subroutine: 	Support Routine for CRC microcode verification  
; Description:  This routine determines the microcode checksum (crc) over the address 	
;		address range E003-F6FF inclusive.	 	
;--------------------------------------------------------------------------------------
crc:        	clr	{start_page+1}		; Clear low byte of page pointer                	
        	ld      A,#$E0                  ; Load start_page for uP ST72264G2
       	 	ld      start_page,A            ; Start_page=$E000 (8k of FLASH)		
		ld      x,#$03                  ; Offset to start at E003
		clr     {ch_sum+0}		; Clear the calculated (MSB)      
		clr     {ch_sum+1}              ;  check sum 
		clr     {ch_sum+2}              ;  storage locations (LSB)	    
        	clr     a
add_byte: 
        	add     a,([start_page.w],x)    ; Addition for calculating the checksum
        	ld	{ch_sum+2},a		; Save LSB of ch_sum
        	jrc     carry           
		inc	x
        	jreq    plus          
  		jra     add_byte

carry:		inc     {ch_sum+1}              ; Add carry to {ch_sum+1}
        	jreq    carry1
        	inc	x
        	jreq    plus     
		jra     add_byte

carry1: 	inc     ch_sum          	; When {ch_sum+1} overflows, add carry 
    		inc	x
        	jreq    plus    		; to ch_sum (MSB)
        	jra     add_byte           

plus:   	inc     start_page		; When X overflows: start_page+=$100
		push	a
		ld	a,start_page            ; Check for end of application
		cp	a,#$F7			;  program area.  Max is x'F6FF'
		pop	a			;
        	jrne    add_byte
        	ret				; When start_page overflows -> exit


;****************************************************************************************
; Segment: 	Relocatable_vectors 
; Description: 	The primary interrupt vector table is located in the read only area 
;               of flash (segment 0) and can not be programmed via the application.
;	
; 		This second jump table allows interrupt routines to be placed in the 
;		writable flash area (segment 1) and made relocatable.
;****************************************************************************************
 
 WORDS
	SEGMENT 'Relocatable_vectors'
						  					
unused1_jp:	jp    dummy           	;FFE0-FFE1h Unimplemented Interrupt 
unused2_jp:    	jp    dummy           	;FFE2-FFE3h Unimplemented Interrupt
i2c_jp		jp    dummy          	;FFE4-FFE5h I2C Interrupt  
sci_jp:        	jp    dummy          	;FFE6-FFE7h Serial Comm Interrupt  
unused3_jp:    	jp    dummy          	;FFE8-FFE9h Unimplemented Interrupt  
unused4_jp:    	jp    dummy           	;FFEA-FFEBh Unimplemented Interrupt  
avd_jp         	jp    dummy          	;FFEC-FFEDh Aux Voltage Detector Interrupt               
timb_jp:	jp    dummy          	;FFEE-FFEFh Timer B Interrupt
mcc_jp:		jp    mcc_rt         	;FFF0-FFF1h Master Clock Control Interrupt                 
tima_jp:	jp    dummy          	;FFF2-FFF3h Timer A Interrupt                       
spi_jp:		jp    spi_rt         	;FFF4-FFF5h SPI Interrupt   
css_jp:      	jp    dummy          	;FFF6-FFF7h Clock Security System Interrupt             
ext1_jp:	jp    dummy          	;FFF8-FFF9h Port B External Interrupt 
ext0_jp:	jp    dummy         	;FFFA-FFFBh Port A External Interrupt                      
trap_jp:       	jp    dummy          	;FFFC-FFFDh interrupt vector location                       
reset_jp:  	jp    hard_reset        ;FFFE-FFFFh interrupt vector location  

  
;****************************************************************************************
; Segment:     Fixed_vectors                                                              
; Description: Fixed Vector locations for ST7 microprocessor interrupts, the vector 
;              has been setup to jump to a relocatable jump lookup table which
;              allows us to update application code in the field.         
;****************************************************************************************
 WORDS
	SEGMENT 'Fixed_vectors'

               	DC.W    unused1_jp      	;FFE0-FFE1h location 
                DC.W    unused2_jp      	;FFE2-FFE3h location
i2c_it:		DC.W	i2c_jp          	;FFE4-FFE5h location  
                DC.W    sci_jp          	;FFE6-FFE7h location  
                DC.W    unused3_jp		;FFE8-FFE9h location  
                DC.W    unused4_jp		;FFEA-FFEBh location  
                DC.W    avd_jp          	;FFEC-FFEDh location
timb_it:	DC.W    timb_jp         	;FFEE-FFEFh location
mcc_it:		DC.W    mcc_jp          	;FFF0-FFF1h location   
tima_it:	DC.W    tima_jp         	;FFF2-FFF3h location
spi_it:		DC.W    spi_jp         		;FFF4-FFF5h location  
                DC.W    css_jp          	;FFF6-FFF7h location
ext1_it:	DC.W    ext1_jp         	;FFF8-FFF9h location 
ext0_it:        DC.W    ext0_jp        		;FFFA-FFFBh location
softit:         DC.W    trap_jp         	;FFFC-FFFDh location 
reset	        DC.W    crc_check        	;FFFE-FFFFh location    

	END
	
	