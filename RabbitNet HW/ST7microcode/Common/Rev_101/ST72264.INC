;+------------------------------------------------------------------------------+
;|										|
;|			Rabbit-Net Register Definitions				|
;|										|
;|			Copyright (c), STMicroelectronics, Z-World		|
;|			Copyright (c), Digi International 2022
;|										|
;+------------------------------------------------------------------------------+
;
;+------------------------------------------------------------------------------+
;|										|
;|			File: st72264.inc					|
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
;|	EXTERN	REGISTERS							|
;+------------------------------------------------------------------------------+ 

 
;********************************************************************************
;+------------------------------------------------------------------------------+
;|	I/O PORTS REGISTERS							|
;+------------------------------------------------------------------------------+
	EXTERN PCDR.b               	; Port C data register
  	EXTERN PCDDR.b              	; Port C data direction register
  	EXTERN PCOR.b               	; Port C option register

  	EXTERN PBDR.b               	; Port B data register
  	EXTERN PBDDR.b              	; Port B data direction register
  	EXTERN PBOR.b               	; Port B option register
                              	
        EXTERN PADR.b          		; Port A data register
  	EXTERN PADDR.b         		; Port A data direction register
  	EXTERN PAOR.b          		; Port A option register                                    
  
  
;+------------------------------------------------------------------------------+
;|	INTERRUPT CONTROLLER REGISTERS						|
;+------------------------------------------------------------------------------+
  	EXTERN ISPR0.b              	; Interrupt software priority register 0
  	EXTERN ISPR1.b              	; Interrupt software priority register 1
  	EXTERN ISPR2.b              	; Interrupt software priority register 2
  	EXTERN ISPR3.b              	; Interrupt software priority register 3
   
   
;+------------------------------------------------------------------------------+
;|	MISCELLANEOUS REGISTER 1						|
;+------------------------------------------------------------------------------+  
  	EXTERN MISCR1.b             	; Miscellaneous register
 
 
;+------------------------------------------------------------------------------+
;|	SPI REGISTERS								|
;+------------------------------------------------------------------------------+
  	EXTERN SPIDR.b              	; SPI data register
  	EXTERN SPICR.b              	; SPI control register
  	EXTERN SPICSR.b             	; SPI control/status register
     
     
;+------------------------------------------------------------------------------+
;|	WATCHDOG REGISTER							|
;+------------------------------------------------------------------------------+
  	EXTERN WDGCR.b              	; Watchdog register
             
             
;+------------------------------------------------------------------------------+
;|	SYSTEM INTEGRITY REGISTER						|
;+------------------------------------------------------------------------------+
	EXTERN SICSR.b              	; System integrity control/status register         
     
     
;+------------------------------------------------------------------------------+
;|	MAIN CLOCK CONTROLLER REGISTER						|
;+------------------------------------------------------------------------------+
	EXTERN MCCSR.b         		; Main clock controller register
               
               
;+------------------------------------------------------------------------------+
;|	I2C REGISTERS								|
;+------------------------------------------------------------------------------+
  	EXTERN I2CCR.b              	; I2C control register
  	EXTERN I2CSR1.b             	; I2C status register 1
  	EXTERN I2CSR2.b             	; I2C status register 2
  	EXTERN I2CCCR.b             	; I2C clock control register 
  	EXTERN I2COAR1.b            	; I2C own add register 1
  	EXTERN I2COAR2.b            	; I2C own add register 2
  	EXTERN I2CDR.b              	; I2C data register 

;+------------------------------------------------------------------------------+
;|	TIMER A REGISTERS							|
;+------------------------------------------------------------------------------+
 	EXTERN TACR2.b              	; Timer A control register 2
  	EXTERN TACR1.b              	; Timer A control register 1
  	EXTERN TACSR.b              	; Timer A control/status register
  	EXTERN TAIC1HR.b            	; Timer A input capture 1 high register 
  	EXTERN TAIC1LR.b            	; Timer A input capture 1 low register
  	EXTERN TAOC1HR.b            	; Timer A output compare 1 high register
  	EXTERN TAOC1LR.b            	; Timer A output compare 1 low register
  	EXTERN TACHR.b              	; Timer A counter high register
  	EXTERN TACLR.b              	; Timer A counter low register
  	EXTERN TAACHR.b             	; Timer A alternate counter high register
  	EXTERN TAACLR.b             	; Timer A alternate counter low register
  	EXTERN TAIC2HR.b            	; Timer A input capture 2 high register 
  	EXTERN TAIC2LR.b            	; Timer A input capture 2 low register
  	EXTERN TAOC2HR.b            	; Timer A output compare 2 high register
  	EXTERN TAOC2LR.b            	; Timer A output compare 2 low register

;+------------------------------------------------------------------------------+
;|	MISCELLANEOUS REGISTER 2 (compatibility with the ST72C254)		|
;+------------------------------------------------------------------------------+  
  	EXTERN MISCR2.b             	; Miscellaneous register 2

;+------------------------------------------------------------------------------+
;|	TIMER B REGISTERS							|
;+------------------------------------------------------------------------------+
  	EXTERN TBCR2.b              	; Timer B control register 2
  	EXTERN TBCR1.b              	; Timer B control register 1
  	EXTERN TBCSR.b              	; Timer B control/status register
  	EXTERN TBIC1HR.b            	; Timer B input capture 1 high register 
  	EXTERN TBIC1LR.b            	; Timer B input capture 1 low register
  	EXTERN TBOC1HR.b            	; Timer B output compare 1 high register
  	EXTERN TBOC1LR.b            	; Timer B output compare 1 low register
  	EXTERN TBCHR.b              	; Timer B counter high register
  	EXTERN TBCLR.b              	; Timer B counter low register
  	EXTERN TBACHR.b             	; Timer B alternate counter high register
  	EXTERN TBACLR.b             	; Timer B alternate counter low register
  	EXTERN TBIC2HR.b            	; Timer B input capture 2 high register 
  	EXTERN TBIC2LR.b            	; Timer B input capture 2 low register
  	EXTERN TBOC2HR.b            	; Timer B output compare 2 high register
  	EXTERN TBOC2LR.b            	; Timer B output compare 2 low register

;+------------------------------------------------------------------------------+
;|	SCI REGISTERS								|
;+------------------------------------------------------------------------------+
  	EXTERN SCISR.b              	; SCI status register    
  	EXTERN SCIDR.b              	; SCI data register
  	EXTERN SCIBRR.b             	; SCI baud rate register    
  	EXTERN SCICR1.b             	; SCI control register 1
  	EXTERN SCICR2.b             	; SCI control register 2
  	EXTERN SCIERPR.b            	; SCI extended receiver prescaler register
  	EXTERN SCIETPR.b            	; SCI extended transmitter prescaler register

;+------------------------------------------------------------------------------+
;|	DEBUG MODULE REGISTERS							|
;+------------------------------------------------------------------------------+

  	EXTERN DMCR.b              	; Debug Module control register
  	EXTERN DMSR.b              	; Debug Module status register
  	EXTERN DMBK1H.b            	; Debug Module breakpoint 1 high register
  	EXTERN DMBK1L.b            	; Debug Module breakpoint 1 low register
  	EXTERN DMBK2H.b            	; Debug Module breakpoint 2 high register
  	EXTERN DMBK2L.b            	; Debug Module breakpoint 2 low register
    
;+------------------------------------------------------------------------------+
;|	ADC REGISTERS								|
;+------------------------------------------------------------------------------+
  	EXTERN ADCDRL.b             	; ADC data low register
  	EXTERN ADCDRH.b             	; ADC data high register
  	EXTERN ADCCSR.b             	; ADC control/status register    
  
;+------------------------------------------------------------------------------+
;|	FLASH REGISTER								|
;+------------------------------------------------------------------------------+
 	EXTERN FCSR.b			; Flash Control Register
  

      
        
