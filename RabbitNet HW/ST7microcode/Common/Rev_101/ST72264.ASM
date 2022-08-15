st7/ 					; The first line is reserved 
                                        ; for specifying the instruction set
                                        ; of the targetted processor
                                  

;+------------------------------------------------------------------------------+
;|										|
;|			    ST7 REGISTER ALLOCATIONS				|
;|										|
;|			Copyright (c), STMicroelectronics, Z-World		|
;|			Copyright (c), Digi International 2022
;|										|
;+------------------------------------------------------------------------------+
;
;+------------------------------------------------------------------------------+
;|										|
;|			File: st72264.asm					|
;|										|
;+----------------------+-----------------------+-------------------------------+
;|	DATE		|	VERSION		|	HISTORY/CHANGES		|
;|	(MM/DD/YY)	|	VX.Y		|				|
;+----------------------+-----------------------+-------------------------------+
;|	03/17/03	|	1.0		|	None			|
;+----------------------+-----------------------+-------------------------------+
;|SOFTWARE DESCRIPTION:								|
;| ST7 H/W registers								|
;|										|
;|PIN ALLOCATION:								|
;| None										|
;+------------------------------------------------------------------------------+


;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+

; #define ST72264G1   	1   		; 4K Flash 
 #define ST72264G2    	1   		; 8K Flash   

;+------------------------------------------------------------------------------+
;|	HARDAWRE REGISTERS							|
;+------------------------------------------------------------------------------+
   
         BYTES                		; following addresses are 8 bits long

         SEGMENT byte at 0-7f 'periph'

;********************************************************************************
;+------------------------------------------------------------------------------+
;|	I/O PORTS REGISTERS							|
;+------------------------------------------------------------------------------+
.PCDR    DS.B    1            		; Port C data register
.PCDDR   DS.B    1            		; Port C data direction register
.PCOR    DS.B    1            		; Port C option register
         DS.B    1            		; Unused
        
.PBDR    DS.B    1            		; Port B data register
.PBDDR   DS.B    1            		; Port B data direction register
.PBOR    DS.B    1            		; Port B option register
         DS.B    1            		; Unused

.PADR    DS.B    1            		; Port A data register
.PADDR   DS.B    1            		; Port A data direction register
.PAOR    DS.B    1            		; Port A option register
        
reserved0
         DS.B    17           		; Unused
         
         
;+------------------------------------------------------------------------------+
;|	INTERRUPT CONTROLLER REGISTERS						|
;+------------------------------------------------------------------------------+
.ISPR0   DS.B    1            		; Interrupt software priority register 0
.ISPR1   DS.B    1            		; Interrupt software priority register 1
.ISPR2   DS.B    1            		; Interrupt software priority register 2
.ISPR3   DS.B    1            		; Interrupt software priority register 3
 
 
;+------------------------------------------------------------------------------+
;|	MISCELLANEOUS REGISTER 1						|
;+------------------------------------------------------------------------------+
.MISCR1  DS.B    1            		; Miscellaneous register 1
 
 
;+------------------------------------------------------------------------------+
;|	SPI REGISTERS								|
;+------------------------------------------------------------------------------+
.SPIDR  DS.B    1             		; SPI data register
.SPICR  DS.B    1             		; SPI control register
.SPICSR DS.B    1             		; SPI control/status register


;+------------------------------------------------------------------------------+
;|	WATCHDOG REGISTER							|
;+------------------------------------------------------------------------------+
.WDGCR   DS.B    1            		; Watchdog register
 
 
;+------------------------------------------------------------------------------+
;|	SYSTEM INTEGRITY REGISTER						|
;+------------------------------------------------------------------------------+
.SICSR   DS.B    1            		; System integrity control/status 
					; register
                                             
                                             
;+------------------------------------------------------------------------------+
;|	MAIN CLOCK CONTROLLER REGISTER						|
;+------------------------------------------------------------------------------+
.MCCSR   DS.B    1            		; Main clock controller register

reserved1
         DS.B    1            		; Unused
             
             
;+------------------------------------------------------------------------------+
;|	I2C REGISTERS								|
;+------------------------------------------------------------------------------+
.I2CCR   DS.B    1            		; I2C control register
.I2CSR1  DS.B    1            		; I2C status register 1
.I2CSR2  DS.B    1            		; I2C status register 2
.I2CCCR  DS.B    1            		; I2C clock control register 
.I2COAR1 DS.B    1            		; I2C own add register 1
.I2COAR2 DS.B    1            		; I2C own add register 2
.I2CDR   DS.B    1            		; I2C data register 

reserved2 
	DS.B   2            		; Unused
	
	
;+------------------------------------------------------------------------------+
;|	TIMER A REGISTERS							|
;+------------------------------------------------------------------------------+
.TACR2   DS.B    1            		; Timer A control register 2
.TACR1   DS.B    1            		; Timer A control register 1
.TACSR   DS.B    1            		; Timer A control/status register
.TAIC1HR DS.B    1            		; Timer A input capture 1 high register 
.TAIC1LR DS.B    1            		; Timer A input capture 1 low register
.TAOC1HR DS.B    1            		; Timer A output compare 1 high register
.TAOC1LR DS.B    1            		; Timer A output compare 1 low register
.TACHR   DS.B    1            		; Timer A counter high register
.TACLR   DS.B    1            		; Timer A counter low register
.TAACHR  DS.B    1            		; Timer A alternate counter high register
.TAACLR  DS.B    1            		; Timer A alternate counter low register
.TAIC2HR DS.B    1            		; Timer A input capture 2 high register 
.TAIC2LR DS.B    1            		; Timer A input capture 2 low register
.TAOC2HR DS.B    1            		; Timer A output compare 2 high register
.TAOC2LR DS.B    1            		; Timer A output compare 2 low register
  
  
;+------------------------------------------------------------------------------+
;|	MISCELLANEOUS REGISTER 2 (compatibility with the ST72C254)		|
;+------------------------------------------------------------------------------+  
.MISCR2  DS.B    1            		; Miscellaneous register 2


;+------------------------------------------------------------------------------+
;|	TIMER B REGISTERS							|
;+------------------------------------------------------------------------------+
.TBCR2   DS.B    1            		; Timer B control register 2
.TBCR1   DS.B    1            		; Timer B control register 1
.TBCSR   DS.B    1            		; Timer B control/status register
.TBIC1HR DS.B    1            		; Timer B input capture 1 high register 
.TBIC1LR DS.B    1            		; Timer B input capture 1 low register
.TBOC1HR DS.B    1            		; Timer B output compare 1 high register
.TBOC1LR DS.B    1            		; Timer B output compare 1 low register
.TBCHR   DS.B    1            		; Timer B counter high register
.TBCLR   DS.B    1            		; Timer B counter low register
.TBACHR  DS.B    1            		; Timer B alternate counter high register
.TBACLR  DS.B    1            		; Timer B alternate counter low register
.TBIC2HR DS.B    1            		; Timer B input capture 2 high register 
.TBIC2LR DS.B    1            		; Timer B input capture 2 low register
.TBOC2HR DS.B    1            		; Timer B output compare 2 high register
.TBOC2LR DS.B    1            		; Timer B output compare 2 low register


;+------------------------------------------------------------------------------+
;|	SCI REGISTERS								|
;+------------------------------------------------------------------------------+
.SCISR   DS.B    1            		; SCI status register    
.SCIDR   DS.B    1            		; SCI data register
.SCIBRR  DS.B    1            		; SCI baud rate register    
.SCICR1  DS.B    1            		; SCI control register 1
.SCICR2  DS.B    1            		; SCI control register 2
.SCIERPR DS.B    1            		; SCI extended receiver prescaler reg
.SCIETPR DS.B    1            		; SCI extended transmitter prescaler reg

reserved3
         DS.B    9            		; Unused   
         
         
;+------------------------------------------------------------------------------+
;|	DEBUG MODULE REGISTERS							|
;+------------------------------------------------------------------------------+
.DMCR   DS.B    1            		; Debug Module control register
.DMSR   DS.B    1            		; Debug Module status register
.DMBK1H DS.B    1            		; Debug Module breakpoint 1 high register
.DMBK1L DS.B    1            		; Debug Module breakpoint 1 low register
.DMBK2H DS.B    1            		; Debug Module breakpoint 2 high register
.DMBK2L DS.B    1            		; Debug Module breakpoint 2 low register

reserved4
         DS.B    9            		; Unused
         
         
;+------------------------------------------------------------------------------+
;|	ADC REGISTERS								|
;+------------------------------------------------------------------------------+
.ADCDRL  DS.B    1            		; ADC data low register
.ADCDRH  DS.B    1            		; ADC data high register
.ADCCSR  DS.B    1            		; ADC control/status register
                     
                     
;+------------------------------------------------------------------------------+
;|	FLASH REGISTER								|
;+------------------------------------------------------------------------------+
.FCSR	DS.B	1	      		; Flash Control Register
;********************************************************************************


;+------------------------------------------------------------------------------+
;|	SEGMENTS SECTION							|
;+------------------------------------------------------------------------------+

 	SEGMENT byte at 80-D6 'Application_ram' 		; Zero Page
  	SEGMENT byte at D7-FF 'iap_buffer_ram'			; In Application Prog Data Buf
         
        WORDS                					; Following addresses are 16 bits long

	SEGMENT byte at 100-17F 'iap_&_stack_ram' 		; In Application Prog & Stack

        
                                        ; 4k (sector 1 + sector0)
 #ifdef ST72264G1
        SEGMENT byte at F000-F002 'Check_sum'			; 3 Bytes    
        SEGMENT byte at F003-F6FF 'Application_code'		; 2K     
        SEGMENT byte at F600-F62F 'Relocatable_vectors'         ; 48 bytes
        SEGMENT byte at F630-F6FF 'Application_parms_1'		; 128 bytes
        SEGMENT byte at F700-F7FF 'Application_parms_2'		; 96 bytes  
        SEGMENT byte at F800-FBFF 'Application_parms_3'		; 1K 
        SEGMENT byte at FC00-FFDF 'Protected'			; 1K                         
 #endif                                     
 
                                        ; 8k (sector 1 + sector0)
 #ifdef ST72264G2
         SEGMENT byte at E000-E00F 'Checksum'			; 16 Bytes         
         SEGMENT byte at E010-F5FF 'Application_code'		; 5.5K 
         SEGMENT byte at F600-F62F 'Relocatable_vectors'       	; 48 bytes
         SEGMENT byte at F630-F6FF 'Application_parms_1'	; 208 bytes
         SEGMENT byte at F700-F7FF 'Application_parms_2'	; 256 bytes 
         SEGMENT byte at F800-FBFF 'Application_parms_3'	; 1K
         SEGMENT byte at FC00-FFDF 'Protected'			; 1K 
 #endif 
                                               
         SEGMENT byte at FFE0-FFFF 'Fixed_vectors'
         
         END        
        
