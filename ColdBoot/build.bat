@echo off

:: Automate build of BIOS/somefile.bin
:: Tom Collins - August 2020

:menu
echo Recompile Pilot and Coldloader files
echo ------------------------------------
echo    1) ColdLoad
echo    2) Pilot

echo    X) Exit

set /p choice=Choice:
:: Take first character of input
set choice=%choice:~0,1%

if "%choice%"=="X" goto exit
if "%choice%"=="x" goto exit
if "%choice%"=="1" goto coldload
if "%choice%"=="2" goto pilot

echo Invalid Choice
goto menu

:coldload
set FILE=coldload
set TRIPLET=1
goto build

:pilot
set FILE=pilot
set TRIPLET=0
goto build

:build
:: remove previous file so we don't mask failed builds
del %FILE%.bin

echo Recompiling %FILE%.c
..\dccl_cmp.exe %FILE%.c -pf coldboot.dcp
if %TRIPLET%==1 (
	echo Using makecold.exe to create BIOS\%FILE%.bin
	:: makecold.exe generates errors if target file exists, so delete it
	del ..\bios\%FILE%.bin
	makecold.exe %FILE%.bin ..\bios\%FILE%.bin
) else (
	echo Copying %FILE%.bin, skipping first 0x6000 bytes, to BIOS\%FILE%.bin
	:: skip over the first 0x6000 bytes when copying to BIOS directory
	powershell -Command "& { $file = (Get-Content %FILE%.bin -Encoding byte -ReadCount 0) | Select-Object -Skip 0x6000 | Set-Content ..\bios\%FILE%.bin -encoding byte }"
)
:exit
