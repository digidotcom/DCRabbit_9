#/****************************************************************/
#/*                                                              */
#/*           RabbitNet Digital I/O card makefile                */
#/*                                                              */
#/****************************************************************/

# This makefile is used to build the ST7 application "rndio.S19"
# with softtec Toolchain
#
# The different targets of this makefile are
#   adc.s19 (default target):	
#      gmake -f rndio.mak rndio.s19   
#      gmake -f rndio.mak
#                             
#   clean:	gmake -f rndio.mak clean
#   rebuild:	gmake -f rndio.mak rebuild
#
#**********************************************************************
PATHC  = I:\Projects\Stm\st7toolchain\asm

APPLI = rndio

# List of source files

ASM_SRC_LIST = main.asm flash.asm vector.asm 

AS = $(PATHC)\asm.exe
LK = $(PATHC)\lyn.exe
OUT = $(PATHC)\obsend.exe 
CS = $(PATHC)\cksum.exe 
  

#************************** END ***************************************
CP = copy
ECHO = echo
RM = del
                                      
rndio.s19: rndio.s18
	rename rndio.s18 rndio.s19  
                                      	                      
rndio.s18: rndio.cod 	                      
	$(OUT) rndio.cod,f,rndio.s18,s   
 
	           
rndio.cod: st72264.obj rndio.obj common.obj
	$(LK) st72264+rndio+common,rndio.cod;
	$(AS) st72264 -sym -fi=rndio.map  
	$(AS) rndio -sym -fi=rndio.map 
	$(AS) common -sym -fi=rndio.map  

rndio.obj: rndio.asm     
	$(AS) rndio.asm -li    

common.obj: common.asm
	$(AS) common.asm -li    

st72264.obj: st72264.asm     
	$(AS) st72264.asm -li


# Rule for forcing the build without taking into account dependencies   
rebuild:
	gmake -f $(APPLI).mak clean
	gmake -f $(APPLI).mak $(APPLI).s19
        
clean : 
	cksum.exe   
	if exist $(APPLI).s19 $(RM)	$(APPLI).s19  
	if exist $(APPLI).s18 $(RM)	$(APPLI).s18
	if exist $(APPLI).cod $(RM)	$(APPLI).cod
	if exist *.obj $(RM) *.obj
	if exist *.lst $(RM) *.lst
	if exist *.map $(RM) *.map  
	             
