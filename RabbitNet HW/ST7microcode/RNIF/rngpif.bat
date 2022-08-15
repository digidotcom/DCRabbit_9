REM *****************************************************************
REM * Batch file to compile/assemble/checksum a Rabbitnet Project
REM *****************************************************************

REM  <<<<<<<<<<<<< Important Information >>>>>>>>>>>>>>>>>>
REM Set project variable below to your board specific name...this is 
REM the only change required in this file. 

set rev=Rev_200
set project=rngpif
del cbe.err
del *.obj  
del /F /Q *.map
del *.lst
del *.s19 
   
del I:\Projects\Rabbitnet_Common\Microcode\%rev%\*.obj  
del I:\Projects\Rabbitnet_Common\Microcode\%rev%\*.lst
              
asm -li I:\Projects\Rabbitnet_Common\Microcode\%rev%\st72264.asm       
asm -li I:\Projects\Rabbitnet_Common\Microcode\%rev%\common.asm      
asm -li %project%.asm  
lyn I:\Projects\Rabbitnet_Common\Microcode\%rev%\st72264+I:\Projects\Rabbitnet_Common\Microcode\%rev%\common+%project%,%project%.cod;
asm I:\Projects\Rabbitnet_Common\Microcode\%rev%\st72264 -sym -fi=%project%.map     
asm I:\Projects\Rabbitnet_Common\Microcode\%rev%\common -sym -fi=%project%.map 
asm %project% -sym -fi=%project%.map                                         
obsend %project%.cod,f,%project%.s19,s 
cksum %project%.s19                                                     
copy I:\Projects\Rabbitnet_Common\Microcode\%rev%\*.lst 

