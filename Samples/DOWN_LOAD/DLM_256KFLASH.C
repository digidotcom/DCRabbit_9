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
/***********************************************************
	Samples\DOWN_LOAD\DLM_256kFlash.C

	THIS PROGRAM IS PROVIDED AS A SAMPLE ONLY!

	This Download Manager is intended for use ONLY with Rabbit-based
	boards with 1 256K flash chip in conjunction with the sample
	program DLP_256kFlash.C and Technical Note 218.

	A18 off of the processor must be connected to A17 on the flash
	for this to work. Many Z-World boards have a jumper for this
	purpose.

	This Download Manager is set up to use serial port B at 57600 baud.
	To configure for another serial port or method of download,
	change the functions:

		 PutChar(char ch)
		 GetChar()
		 InitComm()
		 CloseComm()
		 PauseComm()
		 ResumeComm()

	See the functions' descriptions for specifications. The stub
	UserTask() can be redefined to handle control tasks while
	downloading.

	Directions:
	1)	Before compiling this program, compile the program
	   DLP_256kFlash.C to a HEX file. See the instructions in
	   DLP_256kFlash.C.

   2) Connect serial port B on the target to a free comm port on your
      PC. If you only have one available port and you need it for
      the programming cable, then wait until after step 4 for this
      step, or use a second PC.

	3) In the Compile | Options dialog box in Dynamic C 7.21,
	   click on the "Defines" button, add COMPILE_PRIMARY_PROG and
	   INVERT_A18_ON_PRIMARY_FLASH (reduces usable space in first flash)
	   to the list of macros, and make sure COMPILE_SECONDARY_PROG is NOT
	   on the list. Compile this program to the target.

	4) Disconnect the programming cable.

	5) Start up your dumb terminal program. A good terminal
	   program can be found here:

		http://hp.vector.co.jp/authors/VA002416/teraterm.html

	6) Set the terminal program to use the correct serial port,
		57,600 baud, 1 stop bit, no parity, and software flow control
		(XON/XOFF).

	7) Recycle power on the Rabbit target. A menu should appear on the
	   dumb terminal screen:

	     Download Manager Menu
        1) Enter password
        2) Set password
        3) Download and run program
        4) Execute downloaded program
        Enter Choice:

   8) Type 1, then enter the password "123" (See comments below for
      information about changing the password at compile time.)

   9) Type 3, then use the terminal program to send the file
      DLP_256kFlash.HEX. After the DLP_256kFlash.HEX loads, it should
      run and continuously send the characters 'X' to the dumb terminal
      screen.

  10) Now send the file RESTART.BIN using the dumb terminal program.
      This signals DLP to quit and let DLM_256kFlash run, so
      the menu should reappear.

***********************************************************/

#if (_USER)
	#error This sample is not compatible with RabbitSys - RabbitSys includes
   #error native support for remote firmware update.
#endif

// #class auto
#memmap xmem

//*** uncomment one only! ***
#define USE_SERIAL_PORT_B
// #define USE_SERIAL_PORT_C

#ifndef  COMPILE_PRIMARY_PROG
#error "Must compile as primary program."
#error "Put COMPILE_PRIMARY_PROG macro in Compiler | #defines options"
#error "Must Recompile BIOS also. Hit Ctrl-Y then recompile"
#endif

#ifdef  COMPILE_SECONDARY_PROG
#error "Cannot compile as secondary program."
#error "Remove COMPILE_SECONDARY_PROG macro in Compiler | #defines options"
#error "Must Recompile BIOS also. Hit Ctrl-Y then recompile"
#endif

#ifndef  INVERT_A18_ON_PRIMARY_FLASH
#error "Must declare intention to invert A18 on primary program."
#error "Add INVERT_A18_ON_PRIMARY_FLASH macro in Compiler | #defines options"
#error "Must Recompile BIOS also. Hit Ctrl-Y then recompile"
#endif

#ifndef  ROUTE_A18_TO_FLASH_A17
#error "Must invert A18 on secondary program."
#error "Put ROUTE_A18_TO_FLASH_A17 macro in Compiler | #defines options and retry"
#error "Must Recompile BIOS also. Hit Ctrl-Y then recompile"
#endif

#if  __SEPARATE_INST_DATA__
#error "This sample is not written for separate I&D space."
#endif

/* -------------------------------------------------------------------- */

//*** Maximum length of terminal messages and prompts
#define MAX_MESSAGE_LEN 40

#define SERIAL_BAUDRATE 57600ul

#ifdef USE_SERIAL_PORT_B
   #define BINBUFSIZE  4095
   #define BOUTBUFSIZE 127
#else
#ifdef USE_SERIAL_PORT_C
   #define CINBUFSIZE  4095
   #define COUTBUFSIZE 127
#endif
#endif

/**********************************************************************
**********************************************************************/
// The physical address of the second flash
#define DLP_PHYSICAL_ADDR 0x40000ul

#define MAXSECTORSIZE   4096

/**********************************************************************
 Password configuration
**********************************************************************/
#define USER_DLM_PASSWORD "123" 	 // default user password

#define DLM_MAX_PW_LEN   12       // max password length
#define DLM_MIN_PW_LEN    0  	 	 // min password length
#define DLM_PASSWORD_LVL  0  	 	 // 0 = runtime changeable

#define PSW_TIMEOUT   10000	  	 // time limit to enter password (ms)
//#define DLM_TIMEOUT   25000	    //  general time limit for serial functions (production)
#define DLM_TIMEOUT   250000L	    //  general time limit for serial functions (testing)
#define ERR_MESS_DELAY 2500       //  (ms) time to display an error message
											 //  before blanking it

#define USER_ID_BLOCK_PW_OFFSET 0 // Offset into the User Block of where
                                  //  password is stored. The other
                                  //  information kept in the block
                                  //  follows the password immediately.

/**
 *  Want to pass a command line?  If yes, define a length, otherwise leave
 *  this symbol undefined.  Offset is fixed, not based on actual storage.
 *  Better is store actual offset into "storedDLP" structure.
 *	 Undef USER_ID_BLOCK_CMDLINE_OFFSET to turn stuff off.
 */
#define  USER_ID_BLOCK_CMDLINE_OFFSET   60
#define  USER_ID_BLOCK_CMDLINE_SIZE     30


//*** Used for 1rst time initialization. If this is found in the User ID
//***  Block, then the password should be there too.
#define INITIALIZATION_FLAG  "Z-World/Rabbit Semiconductor"

#ifndef CR
 #define CR  '\r'
#endif
#ifndef LF
 #define LF  '\n'
#endif

/**********************************************************************
This is the signal to restart this program. The function
ProcessRestartCommand() in this program is called everytime GetChar
detects a byte with the high bit set. Program data is transmitted
in ASCII Hex format with no high bits set, so it cannot be confused
with a reset signal. Reset commands may be sent from the terminal
with keyboard combinations or by sending a binary file or by other
means in custom terminal program. The file RESTART.BIN is provided
to send the default restart command sequence provided here.

ProcessRestartCommand() needs to be used by the downloaded
program to monitor for this signal as well.

RESTARTSIGNAL should be a string of bytes at least one long.
Each byte should have the high bit set.

RESTARTSIGNAL_TIMEOUT is the number of seconds allowed between
beginning and end of RESTARTSIGNAL. These bytes may be sent by
keyboard combinations or a file or other means.
**********************************************************************/
#define RESTARTSIGNAL "\xaa\xbb\xcc\xbb\xaa"
#define RESTARTSIGNAL_TIMEOUT 30

int UserTask();
int InitComm();
int download_program();
int DLP_CRC_OK();
int RestartDLM();
int ProcessProgramByte(char);
int WriteLastSector();
int PauseComm();
int ResumeComm();
int CloseComm();
const char password[]=USER_DLM_PASSWORD;
char pword[DLM_MAX_PW_LEN];
int  clear_to_enter;

/* -------------------------------------------------------------------- */

const char *Menu[] =
{
	"\x1b[2J",						// blank the screen
	"Download Manager Menu",
	"1) Enter password",
	"2) Set password",
	"3) Download and run program",
	"4) Execute downloaded program",
	""
};

const char *ErrMessage[] =
{
	"",
	"Checksum error",
	"Invalid start of record error",
	"Unknown record type error",
	"Unknown state error",
	"Error writing last sector",
	"Error writing flash",
	"DLM overlap error",
	"CRC check error",
	"Must enter valid password first",
	"Timeout error",
	"User ID Block error",
	""
};

extern int _useOtherBlockFlag;

int Message(char row, char col, char *str,int blank);
int SetCursor(int row, int col);
int enter_password();
int set_password();
int ProcessDLP(char ch);
firsttime int GetString(char *s, int max, unsigned long tmout);
firsttime int PutString(char *s);
int PutChar(char ch);
int GetChar();
void ProcessRestartCommand(char ch);
void RunDLP();

//char  Chip_SW_Func [32];
char  Chip_SW_Func [90];
void  CopyChipSWtoRAM();
void  SwitchChip();
void 	_EndSwitchFlash(), _SwitchFlashChip();
#define SWITCH_CODE_SIZE	((int)((unsigned)_EndSwitchFlash - (unsigned)_SwitchFlashChip)+1)

//**** Special WriteFlash fucntion writing to upper 128K
root int WriteFlashH(unsigned long flashDst, void* rootSrc, int len);

//**** Error conditions
#define CHECKSUM_ERROR       1
#define INVALID_RECORD_START 2
#define UNKNOWN_RECORD_TYPE  3
#define UNKNOWN_STATE  		  4
#define WRITE_LAST_SECT_ERR  5
#define WRITE_FLASH_ERR      6
#define DLM_OVERLAP_ERR      7
#define CRC_ERROR            8
#define AUTHORIZATION_ERROR  9
#define TIMEOUT_ERROR        10
#define USER_IDBLK_IO_ERROR  11

//**** State machine states for processing HEX file
#define READ_RECORD_START  0
#define READ_RECORD_LEN    1
#define READ_OFFSET_LSB    2
#define READ_OFFSET_MSB    3
#define READ_RECORD_TYPE   4
#define READ_DATA_RECORD   5
#define READ_SEGMENT_LSB   6
#define READ_SEGMENT_MSB   7
#define READ_CHECKSUM      8
#define READ_PROGRAM_DATA  9
#define READ_LAST_CHECKSUM 10
#define END_DLP            11
#define END_DLP_ERROR      12

struct hexfileState
{
	unsigned int state;
	unsigned int offset;
	unsigned int segment;
	int CRC;
	char recordType;
	char recordLength;
	char checksum;
	unsigned long fileOffset;
	unsigned long Size;
	unsigned long Addr;
	unsigned long sectorOffset;
	char error;
	char nDataBytes;
};

struct hexfileState DLP;
struct _storedDLP
{
	unsigned CRC;
	unsigned long Addr;
	unsigned long Size;
};

// Initialize stored DLP info in flash to impossible
// values.
const struct _storedDLP storedDLP =
   {0xffffu,0xfffffffful,0xfffffffful};

///////////////////////////////////////////////////////////////////////

void main()
{
	long length;
	char ch;
	int  row,coferr,menuResp;
	int  retval,done,timedOut;
	char pssword[sizeof(INITIALIZATION_FLAG)];
	int vwd;

	if( SWITCH_CODE_SIZE > sizeof(Chip_SW_Func) ) {
		printf( "ERROR: SWITCHING CODE TOO LARGE!!!\n" );		/* DEBUG */
		serBputs( "ERROR: SWITCHING CODE TOO LARGE!!!\r\n" );	/* DEBUG */
		exit(2);
	}
#ifdef USER_ID_BLOCK_CMDLINE_OFFSET
	if( USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN+sizeof(storedDLP)+sizeof(INITIALIZATION_FLAG) > USER_ID_BLOCK_CMDLINE_OFFSET ) {
		printf( "ERROR: cmdline overwrites required userblock data!!\n" );
		serBputs( "ERROR: cmdline overwrites required userblock data!!\r\n" );
		exit(2);
	}
#endif

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	vwd  = VdGetFreeWd(255);     // get a virtual watchdog

	memset(&DLP,0,sizeof(DLP)); // initialize the data structure

	retval = readUserBlock( pssword,
					   USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN+sizeof(storedDLP),
                  sizeof(INITIALIZATION_FLAG)
					  );

	//*** If the initialization flag is not present or this is a debug
	//***  startup, put initialization flag in the User ID Block,
	//***  along with DLP info initial info, and user password.
	//***  (OPMODE == 0x88, using host debugger)
	if( strncmp(pssword, INITIALIZATION_FLAG, DLM_MAX_PW_LEN) || OPMODE==0x88 )
	{
#ifdef USER_ID_BLOCK_CMDLINE_OFFSET
		/* Do this first so can use pssword[] as temp storage! */
		strcpy( pssword, "*cmdline*" );
	   retval +=
	   writeUserBlock( USER_ID_BLOCK_CMDLINE_OFFSET,
		                pssword,
						    strlen(pssword)+1		/* Store NUL char also */
						   );
#endif

	   retval +=
	   writeUserBlock( USER_ID_BLOCK_PW_OFFSET,
		                USER_DLM_PASSWORD,
						    DLM_MAX_PW_LEN
						   );
	   retval +=
	   writeUserBlock( USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN,
		                &storedDLP,
						    sizeof(storedDLP)
						   );
	   retval +=
	   writeUserBlock( USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN+sizeof(storedDLP),
		                INITIALIZATION_FLAG,
						    sizeof(INITIALIZATION_FLAG)
						   );
	}

	if(retval)
	{
	   DLP.error = USER_IDBLK_IO_ERROR;
		DLP.state = END_DLP_ERROR;
	}

	clear_to_enter = 0;

	//*** Main loop ***
	while(!DLP.error && (DLP.state < END_DLP))
	{
		VdHitWd(vwd);    // hit vwd

		UserTask();

		costate{
         timedOut=0;

			//*** time out if user doesn't enter password quick enough
			waitfor(clear_to_enter || (!clear_to_enter && (timedOut = DelayMs(PSW_TIMEOUT))));
			if(timedOut)
			{
				DLP.error = TIMEOUT_ERROR;
			}
		}

		//*** main task ***
		costate
		{
			waitfor(InitComm());

			//*** display menu
			row = 0;
			while(strlen(Menu[row]))
			{
				waitfor(Message(row+1,1,Menu[row],0));
				row++;
			}

			//*** get menu choice
			menuResp = 0;
			while(menuResp < 0x31 || menuResp > 0x36)
			{
				UserTask();
				waitfor(Message(row+1,1,
					"Enter Choice:    ",0));
				waitfor(SetCursor(row+1,17));
				waitfor((menuResp = GetChar())>=0);
				waitfor(SetCursor(row+1,17));
				waitfor(PutChar(menuResp));
			}

			//*** process choice
			switch(menuResp)
			{
				case '1' :
					waitfor(enter_password());
					break;
				case '2' :
					if(clear_to_enter)
					{
						waitfor(set_password());
						break;
					}
					else
					{
						DLP.error = AUTHORIZATION_ERROR;
					}
					break;
				case '3' :
					if(clear_to_enter)
					{
						waitfor(download_program()>=END_DLP || DLP.error);
					}
					else
					{
						DLP.error = AUTHORIZATION_ERROR;
					}
					break;
				case '4' :
					goto RUNPROG;
				default : break;

			}	// end switch
		}	// end costatement
	}	// end while

RUNPROG:
	if(DLP.error)
	{
		while(!Message(9,1,ErrMessage[DLP.error],ERR_MESS_DELAY))
		{
			UserTask();
		}
	}

	if((retval=DLP_CRC_OK())==1)
	{
		while(!Message(9,1,"Valid DLP found, starting DLP",1500))
		{
			UserTask();
		}
		RunDLP();
	}
	else
	{
		if(retval)
		{
		   while(!Message(9,1,"Error Reading ID block",ERR_MESS_DELAY))
			{
				UserTask();
			}
		}
		else
		{
		   while(!Message(9,1,"CRC invalid on DLP",ERR_MESS_DELAY))
			{
				UserTask();
			}
		}
		RestartDLM();
	}
}

/**********************************************************************
int ProcessDLP(char ch);

Description:
This function is a state machine designed to process one byte at time
a HEX file generated by Dynamic C.

Input:
ch - the byte to be processed

Return value:
The current state of the state machine
**********************************************************************/
nodebug
int ProcessDLP(char ch)
{
	static char msNibble,msNread;
   struct _storedDLP	DLPinfo;
#GLOBAL_INIT
{
	//**** this code is run once before main is called only
	msNread=0;
}

	DLP.fileOffset++;

	//**** skip control chars
	if(ch < 0x20) return DLP.state;

	//**** get value of byte from ASCII Hex format
	//**** must read two chars to get byte except for ':'
	if(DLP.state!=READ_RECORD_START)
	{
		if(!msNread)
		{
			if(ch <= 0x39)	// if 0-9
			{
				msNibble = ((ch-0x30)<<4);
			}
			else   			// if a-f
			{
				msNibble = (((ch&0x0f)+9)<<4);
			}
			msNread = 1;

			//**** need to get LSN before processing byte
			return DLP.state;
		}

	  //**** read least significant nibble
		else
		{
			if(ch <= 0x39){
				ch = msNibble + (ch-0x30);
			}
			else
			{
				ch = msNibble + (ch&0x0F) + 9;
			}
			msNread = 0;  // so we start with MS nibble next time
		}
	}

	if(DLP.state!=READ_CHECKSUM &&
		DLP.state!=READ_LAST_CHECKSUM &&
		DLP.state!=READ_RECORD_START)
	{
		DLP.checksum += ch;
	}
	switch(DLP.state)
	{
		case READ_RECORD_START :
			if(ch != ':')
			{
				DLP.error = INVALID_RECORD_START;
				DLP.state = END_DLP_ERROR;
			}
			DLP.state = READ_RECORD_LEN;
			DLP.checksum = 0;
			break;
		case READ_RECORD_LEN :
			DLP.recordLength = ch;
			DLP.state = READ_OFFSET_MSB;
			break;
		case READ_OFFSET_LSB :
			DLP.offset += ch;
			DLP.state = READ_RECORD_TYPE;
			break;
		case READ_OFFSET_MSB :
			DLP.offset = (ch<<8);
			DLP.state = READ_OFFSET_LSB;
			break;
		case READ_RECORD_TYPE :
			switch(DLP.recordType = ch)
			{
				case 0 :
				   DLP.state = READ_PROGRAM_DATA;
					DLP.nDataBytes = DLP.recordLength;
					break;
				case 1  :
				   DLP.state = READ_LAST_CHECKSUM;
					break;
				case 2  :
				   DLP.state = READ_SEGMENT_MSB;
					break;
				default :
					DLP.error = UNKNOWN_RECORD_TYPE;
					DLP.state = END_DLP_ERROR;
					break;
			}
			break;
		case READ_SEGMENT_LSB :
			DLP.segment += ch;
			DLP.state = READ_CHECKSUM;
			break;
		case READ_SEGMENT_MSB :
			DLP.segment = (ch<<8);
			DLP.state = READ_SEGMENT_LSB;
			break;
		case READ_PROGRAM_DATA :
			if(!ProcessProgramByte(ch))
			{
				if( !(--DLP.nDataBytes) )
				{
					DLP.state = READ_CHECKSUM;
				}
			}
			else
			{
				DLP.error = WRITE_FLASH_ERR;
				DLP.state = END_DLP_ERROR;
			}
			break;
		case READ_CHECKSUM :
			if (ch != -DLP.checksum)
			{
				DLP.error = CHECKSUM_ERROR;
				DLP.state = END_DLP_ERROR;
			}
			DLP.state = READ_RECORD_START;
			break;
		case READ_LAST_CHECKSUM :
			if(ch != -DLP.checksum || -DLP.checksum != 0xff)
			{
				DLP.error = CHECKSUM_ERROR;
				DLP.state = END_DLP_ERROR;
			}
			if(!WriteLastSector(ch))
			{
				//*** store DLP info in UID block
				//*** store DLP info in flash constants
				DLPinfo.Addr = DLP.Addr;
				DLPinfo.Size = DLP.Size;
				DLPinfo.CRC =  DLP.CRC;
            if(writeUserBlock( USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN,
					    &DLPinfo,
                   sizeof(DLPinfo) )
               )
				{
					DLP.error = USER_IDBLK_IO_ERROR;
					DLP.state = END_DLP_ERROR;
				}
				else
				{
					DLP.state = END_DLP;
				}
			}
			else
			{
				DLP.state = END_DLP_ERROR;
				DLP.error = WRITE_LAST_SECT_ERR;
			}
			break;
		case END_DLP_ERROR :
			break;
		default :
			DLP.error = UNKNOWN_STATE;
			DLP.state = END_DLP_ERROR;
			break;
	}
	return DLP.state;
}

/**********************************************************************
int ProcessProgramByte(char ch);

Description:
This function processes bytes received from the DLM hex file
processing state machine and writes them to the 2nd flash. It
waits until a full sector has been received before writing to
the flash. Before it writes to the flash, it tells the host
to pause transmission so data is not lost.

Assumption:
The program will be received in one contiguous block starting on a
sector boundary.

Input:
ch - the byte to be processed

Return value:
0 unless a write to flash fails in which case the return value of the
failed call to WriteFlashH is returned.
**********************************************************************/
unsigned long physaddr;
char sectorBuff[MAXSECTORSIZE];
char scrBuff[MAXSECTORSIZE];

nodebug
int ProcessProgramByte(char ch)
{
	static char *sPtr;
	static int retval,done,first,s_size;

#GLOBAL_INIT
{
	//**** these are initialized once before main is called
	sPtr = sectorBuff;
	first = 1;
}
	retval = 0;

	s_size =  _FlashInfo.sectorSize > MAXSECTORSIZE ?
	    MAXSECTORSIZE : _FlashInfo.sectorSize;

	//**** keep a count program size
	DLP.Size++;

	//**** load byte to sector buffer
	*sPtr = ch;
	DLP.CRC = getcrc(&ch, 1, DLP.CRC);

	//**** If we're starting a sector, determine it's physical address.
	//****  The HEX format uses Intel real mode style addressing
	if(DLP.sectorOffset==0)
	{
 		physaddr = (unsigned long)DLP.offset +
 					 ((unsigned long)DLP.segment<<4L);
	}

	//**** if we have a sectorful, write it
	if(DLP.sectorOffset == s_size-1)
	{
		done = 0;
		while(!done && !DLP.error)
		{
			costate
			{
				//*** pause while sector is processed
				waitfor(PauseComm());
				waitfor(DelayMs(50));

				retval = WriteFlashH(physaddr+DLP_PHYSICAL_ADDR, sectorBuff, s_size);

				if(first)
				{
					//*** DLP.Addr is the start of the program
					first = 0;
					DLP.Addr = physaddr+DLP_PHYSICAL_ADDR;
					printf( "sector size = %u\n", (unsigned) s_size );
				}

				if(retval)
				{
					DLP.error = WRITE_FLASH_ERR;
					DLP.state = END_DLP_ERROR;
				}

				waitfor(ResumeComm());

				sPtr = sectorBuff;
				DLP.sectorOffset = 0;
				done = 1;
			}
		}
	}
	else
	{
		DLP.sectorOffset++;
		sPtr++;
	}
	return retval;
}

/**********************************************************************
Called after entire program received to see if a partial sector was
received last.
**********************************************************************/
nodebug
int WriteLastSector()
{
	int retval;

	if(DLP.sectorOffset==0)
	{
		// integeral number of sectors was received
		return 0;
	}

	retval = WriteFlashH(physaddr+DLP_PHYSICAL_ADDR,
							sectorBuff,
							(int)DLP.sectorOffset+1);
	if(retval)
	{
		DLP.error = WRITE_LAST_SECT_ERR;
		DLP.state = END_DLP_ERROR;
		return retval;
	}
	else
	{
		return 0;
	}
}

/**************************************************************************
	int SetCursor(int row, int col);
**************************************************************************/
nodebug
int SetCursor(int row, int col)
{
	//*** ASCII terminal set cursor command is esc[row;colH
	static char buf[9];
	static int retval;

	costate{
		retval = 0;
		buf[0] = '\x1b';
		buf[1] = '[';
		buf[2] = row/10 + 0x30;
		buf[3] = row - 10*(row/10) + 0x30;
		buf[4] = ';';
		buf[5] = col/10 + 0x30;
		buf[6] = col - 10*(col/10) + 0x30;
		buf[7] = 'H';
		buf[8] = 0;
		waitfor(PutString(buf));
		retval = 1;
	}
	return retval;
}

/**************************************************************************
	put  str at (row,col) and blank out after blank
	milliseconds if blank non-zero
**************************************************************************/
nodebug
int Message(char row,char col,char * buf, int blank)
{
	static char newbuf[MAX_MESSAGE_LEN];
	static int retval,i;

	costate{
		retval = 0;
		strncpy(newbuf,buf,MAX_MESSAGE_LEN);
		for(i=strlen(newbuf);i<	MAX_MESSAGE_LEN-1;i++)
		{
			newbuf[i]=' ';
		}
		newbuf[i]=0;
		waitfor(SetCursor(row,col));
		waitfor(PutString(newbuf));
		if(blank){
			waitfor(DelayMs(blank));
			for(i=0;i<MAX_MESSAGE_LEN-1;i++)
			{
				newbuf[i]=' ';
			}
			newbuf[i]=0;
			waitfor(SetCursor(row,col));
			waitfor(PutString(newbuf));
		}
		retval = 1;
	}
	return retval;
}

/**************************************************************************
int enter_password();
**************************************************************************/
nodebug
int enter_password()
{
   static char password[DLM_MAX_PW_LEN];
	static int retval,i;

	costate{
		retval = 0;
		waitfor(Message(9,1,"Enter password:    ",0));
		waitfor(SetCursor(9,18));
		waitfor(GetString(pword, DLM_MAX_PW_LEN, PSW_TIMEOUT));

      //*** compare with stored password
		if(!readUserBlock(password,USER_ID_BLOCK_PW_OFFSET,DLM_MAX_PW_LEN))
		{
   		if(!strcmp(password,pword)){
	   		clear_to_enter = 1;
		   	waitfor(Message(9,1," ",1));
   		   retval = 1;
   		}
	   	else{
		   	waitfor(Message(9,1,"Invalid Password!",ERR_MESS_DELAY));
   		   retval = 1;
   		}
		}
		else
		{
          waitfor(Message(9,1,"Error reading stored PW!",ERR_MESS_DELAY));
   		 retval = 1;
		}
	}
	return retval;
}

/**************************************************************************
int set_password();
**************************************************************************/
nodebug
int set_password()
{
	static int retval,i;
	static char tpswd[DLM_MAX_PW_LEN];

	costate{
		retval = 0;
		if(DLM_PASSWORD_LVL){
			waitfor(Message(9,1,"Password Changing Not Enabled",ERR_MESS_DELAY));
		}
		else{
			waitfor(Message(9,1,"Enter New Password: ",0));
			waitfor(SetCursor(9,22));
			waitfor(GetString(tpswd, DLM_MAX_PW_LEN,DLM_TIMEOUT));
			waitfor(Message(9,1,"Enter Again to Verify:   ",0));
			waitfor(SetCursor(9,24));
			waitfor(GetString(pword, DLM_MAX_PW_LEN,DLM_TIMEOUT));
			if(!strcmp(tpswd,pword) && strlen(tpswd)>=DLM_MIN_PW_LEN)
			{
				retval = writeUserBlock(USER_ID_BLOCK_PW_OFFSET,
				                       pword,DLM_MAX_PW_LEN);
				waitfor(Message(9,1,"                                  ",0));
				if(retval)
				{
					waitfor(Message(9,1,
						"Error Storing Password! ",ERR_MESS_DELAY));
					waitfor(SetCursor(8,1));
				   retval = 0;
				}
				else
				{
				   retval = 1;
				}
			}
			else{
				waitfor(Message(9,1,
					"Invalid Password! ",ERR_MESS_DELAY));
				waitfor(SetCursor(8,1));
			}
		}
	}
	return retval;
}

/*********************************************************************
int download_program()
This function is called until the DLP is completely downloaded
or an error happens.

Return Value: -  The last return value of ProcessDLP.
*********************************************************************/
nodebug
int download_program()
{
	static int crc,done,retval;
	static char chr;

	done = retval = 0;

	costate
	{
		waitfor(Message(9,1,"Send HEX file as raw ASCII or Binary",0));
		DLP.CRC = 0;
		while(!DLP.error && retval < END_DLP)
		{
			waitfor(((chr = GetChar())>=0) || DLP.error );
			if(DLP.error || DLP.state >= END_DLP)break;
			retval = ProcessDLP((char)chr);
			yield;
		}
		done = 1;
	}
	return retval;
}

/*********************************************************************
int GetString(char *s, int max, unsigned long tmout)

This function should be called continuously until it succeeds in
getting a terminated string or times out.

If DLM_TIMEOUT milliseconds elapse before it succeeds, it sets
DLP.error to TIMEOUT_ERROR and return 1.

Input: s - the string to fill
		 max - the maximum length
		 tmout - the tmout limit
Return Value: -  1 if finished or timed out, 0 otherwise
*********************************************************************/
nodebug
firsttime int GetString(CoData*CD,char *s, int max, unsigned long tmout)
{
   static int done, c;
   static char *p;
   static unsigned long chtm;

  	if (CD->firsttimeflag)
	{
	   p = s;
		CD->firsttimeflag = done = 0;
		CD->content.ul = MS_TIMER+tmout;
	}
	c=GetChar();
  	switch (c)
  	{
	   	case -1: break;
  	   	case  8: if (p > s) --p; break;
  		   case  0:
  	  		case LF: c = CR;
   	   case CR: done=1;*p=0;break;
      	default:
      		*p++ = c;
      		 break;
   }

   // null terminate if timed out, max len., or normal
	if(!done && MS_TIMER > CD->content.ul)
	{
		DLP.error = TIMEOUT_ERROR;
		DLP.state = END_DLP_ERROR;
		done = 1;
	}
	return (done);
}

/*********************************************************************
int PutString(char *s)

This function should be called continuously until it succeeds in
sending a NULL terminated string or times out.
If DLM_TIMEOUT milliseconds elapse before it succeeds, it sets
DLP.error to TIMEOUT_ERROR and return 1.

Input: s - the string to send
Return Value: -  1 if finished or timed out, 0 otherwise
*********************************************************************/
nodebug
firsttime int PutString(CoData*CD,char *s)
{
   static int done;
   static char *p;
   static unsigned long chtm;

  	if (CD->firsttimeflag)
	{
	   p = s;
		CD->firsttimeflag = done = 0;
		CD->content.ul = MS_TIMER+DLM_TIMEOUT;
	}
	else if (MS_TIMER > CD->content.ul)
	{
		DLP.error = TIMEOUT_ERROR;
		DLP.state = END_DLP_ERROR;
		done = 1;
	}
	else
	{
		if(*p)
		{
			PutChar(*p);
			p++;
		}
		else
		{
			done=1;
		}
	}
	return done;
}

/*********************************************************************
int ProcessRestartCommand()
This function restarts the DLM if it receives the correct sequence
of hig-bit-set characters within the allowed time. It is called
every time a high-bit-set character is received. If it times out,
it just reinitializes itself to the start state.
*********************************************************************/
nodebug
void ProcessRestartCommand(char ch)
{
	static unsigned long timelimit;
	static char restart[sizeof(RESTARTSIGNAL)];
	static int indx;
#GLOBAL_INIT{ indx = 0; memset(restart,0,sizeof(RESTARTSIGNAL)-1);}

	if(!indx)
	{
		timelimit = SEC_TIMER+RESTARTSIGNAL_TIMEOUT;
	}
	else
	{
		if((long)(SEC_TIMER - timelimit)>=0)
		{
			indx = 0;
			memset(restart,0,sizeof(RESTARTSIGNAL)-1);
			return;
		}
	}
	restart[indx] = ch;
	indx++;
	if(indx==sizeof(RESTARTSIGNAL))
	{
		memset(restart,0,sizeof(RESTARTSIGNAL)-1);
		indx = 0;
	}
   else if(!memcmp(restart,RESTARTSIGNAL,sizeof(RESTARTSIGNAL)-1))
	{
		RestartDLM();
	}
}

/*********************************************************************
int RestartDLM()
*********************************************************************/
nodebug
RestartDLM()
{
#asm
	ipset 3          ; turn off interrupts
	call 0x0000
#endasm
}

/*  JUMP CODE MUST BE IN "root" SO memcpy() CAN FIND IT! */
#asm nodebug root
_SwitchFlashChip::
	ipset 3							  // turn off interrupts
	ld   a,0x51
	ioi  ld (WDTTR),a
	ld   a,0x54
	ioi  ld (WDTTR),a

	ld   a,(MB0CRShadow)         // get shadow reg
   or   0x10                    // set invert A18 bit
	ioi  ld (MB0CR),a            // loadMMU reg

	ld   hl,0
	jp   (hl)                    // jump to DLP
_EndSwitchFlash::
#endasm

nodebug
void RunDLP()
{

	CloseComm();

	// Put flash switching function in RAM
	memcpy( Chip_SW_Func, _SwitchFlashChip, SWITCH_CODE_SIZE );

	// ...and call it
	SwitchChip();
}

nodebug
void SwitchChip()
{
#asm
		call	Chip_SW_Func
#endasm
}


/*********************************************************************
int DLP_CRC_OK()
*********************************************************************/
nodebug
int DLP_CRC_OK()
{
	int CRC;
	char prog[0x100], MB1CRnew;
	unsigned long Size,i;
   struct _storedDLP UBstoredDLP;


	//**** If download was successful, these were changed ****
	if(!readUserBlock(&UBstoredDLP,USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN,
	              sizeof(UBstoredDLP)))
	{
	   if((UBstoredDLP.Addr!=0xfffffffful) && (UBstoredDLP.Size!=0xfffffffful))
   	{
		   Size = UBstoredDLP.Size;
   		CRC = 0;

   		MB1CRnew = MB1CRShadow;
			MB1CRnew &= 0xf0;

			WrPortI(MB1CR,NULL,MB1CRnew);

	   	//*** CRC function can only handle 255 bytes
		   for(i=0;i<Size-0xff;i+=0xff)
   		{
	   		xmem2root(prog,UBstoredDLP.Addr+i,(int)0xff);
		   	CRC = getcrc(prog, 0xff, CRC);
   		}
	   	xmem2root(prog,UBstoredDLP.Addr+i,(int)0xff);
   		CRC = getcrc(prog, (char)(Size-i), CRC);

			WrPortI(MB1CR,NULL,MB1CRShadow);
	   	if(CRC==UBstoredDLP.CRC)
		   	return 1;
   		else
	   		return 0;
		}
	}
	return -1;
}



/*********************************************************************
int WriteFlashH()

This is a special flash writing function to write to the top
half of the 256K chip when A18 on the processor is connected to A17
on the chip. It never writes more than a sector and always writes
starting on a sector boundary.
*********************************************************************/
debug int
WriteFlashH(unsigned long flashDst, void* rootSrc, int len)
{
	static char  testbuff[MAXSECTORSIZE];		/* "static" -- too big to fit on stack! */
	auto int   rc;
	auto char  buff[60];
	auto int 	saveMB1CR;

	/*  First write.  */
	rc = WriteFlash( flashDst, rootSrc, len );
	if( rc != 0 ) {
		sprintf( buff, "\r\nWFH($%lx) --> %d\r\n", flashDst, rc );
#ifdef USE_SERIAL_PORT_B
		serBputs( buff );
#endif
#ifdef USE_SERIAL_PORT_C
		serCputs( buff );
#endif
		return -1;
	}

	/*  Then verify.  */
		// Map memory quardrant 2 to first flash (/CS0)
		saveMB1CR = MB1CRShadow;
		WrPortI( MB1CR, & MB1CRShadow, FLASH_WSTATES | 0x00 );

		xmem2root( testbuff, flashDst, len );

		//  Map memory quadrant 2 back to original location
		WrPortI( MB1CR, & MB1CRShadow, saveMB1CR );

	if( 0 != memcmp( rootSrc, testbuff, len ) ) {
		sprintf( buff, "WFH($%lx) bad memcmp()\r\n", flashDst );
#ifdef USE_SERIAL_PORT_B
		serBputs( buff );
#endif
#ifdef USE_SERIAL_PORT_C
		serCputs( buff );
#endif
		return -1;
	}

	return( rc );
}


/*********************************************************************
BEGIN USER DEFINABLE FUNCTIONS
*********************************************************************/
/*********************************************************************
PauseComm()

Signals to pause program transmission before
a sector of program data is written.

Return Value: - 1 finished, 0 otherwise
*********************************************************************/
nodebug
int PauseComm()
{
	// send XOFF
	return PutChar(19);
}

/*********************************************************************
ResumeComm()

Signals to resume program transmission after
a sector of program data is written.

Return Value: - 1 finished, 0 otherwise
*********************************************************************/
nodebug
int ResumeComm()
{
	// send XON
	return PutChar(17);
}

/*********************************************************************
PutChar(char ch)
This function should be called continuously until it succeeds in
sending a character or times out.
If DLM_TIMEOUT milliseconds elapse before it succeeds, it should
set DLP.error to TIMEOUT_ERROR and return 1.

Input: ch - the character to send
Return Value: - 0 if ch not sent, 1 if ch sent
*********************************************************************/
nodebug
int PutChar(char ch)
{
	static int retval,timedOut;

	costate
	{
		timedOut = retval = 0;
#ifdef USE_SERIAL_PORT_B
		waitfor( (retval=serBputc(ch) == 1) ||
#else
#ifdef USE_SERIAL_PORT_C
		waitfor( (retval=serCputc(ch) == 1) ||
#endif
#endif
					(timedOut=DelayMs(DLM_TIMEOUT))
					 );
	}
	if(timedOut)
	{
		DLP.error = TIMEOUT_ERROR;
		DLP.state = END_DLP_ERROR;
		retval = 1;
	}
	return retval;
}

/*********************************************************************
int GetChar(char ch)

This function should be called continuously until it succeeds in
getting a character or times out.
If DLM_TIMEOUT milliseconds elapse before it succeeds, it should
set DLP.error to TIMEOUT_ERROR and return 1.

It also needs to monitor for bytes received with the high
the bit set and call ProcessRestartCommand each time one is received.

Input: ch - the character to send
Return Value: -  -1 if character not received yet, else value
 received if one is received.
*********************************************************************/
nodebug
int GetChar()
{
	static int retval,timedOut;

	costate
	{
		timedOut = 0;
		retval = -1;

#ifdef USE_SERIAL_PORT_B
		waitfor( ((retval=serBgetc()) >= 0)||
#else
#ifdef USE_SERIAL_PORT_C
		waitfor( ((retval=serCgetc()) >= 0)||
#endif
#endif
					(timedOut=DelayMs(DLM_TIMEOUT))
					 );

		if((retval&0xff80)==0x0080)
		{
			ProcessRestartCommand(retval);
		}
	}
	if(timedOut)
	{
		DLP.error = TIMEOUT_ERROR;
		DLP.state = END_DLP_ERROR;
		retval = 1;
	}
	return retval;
}

/*********************************************************************
InitComm()

Open communication on the Download Manager channel

Return Value: -  1 sucessful, 0 otherwise
*********************************************************************/
nodebug
int InitComm()
{

#if ((_BOARD_TYPE_ >= BL2000) && (_BOARD_TYPE_ <= BL2040))
	brdInit();		//required for BL2000 series boards
#endif

#ifdef USE_SERIAL_PORT_B
	serBopen(SERIAL_BAUDRATE);
#else
#ifdef USE_SERIAL_PORT_C
	serCopen(SERIAL_BAUDRATE);
#endif
#endif

#if ((_BOARD_TYPE_ >= BL2000) && (_BOARD_TYPE_ <= BL2040))
	brdInit();		//required for BL2000 series boards

#ifdef USE_SERIAL_PORT_B
   serBwrFlush();
   serBrdFlush();
#else
#ifdef USE_SERIAL_PORT_C
   serCwrFlush();
   serCrdFlush();
#else
#error "Must #define a port"
#endif
#endif
   serMode(0);
#endif	// if BL20x0 series board

	// send XON
	return PutChar(17);
}

/*********************************************************************
CloseComm()

Close communication on the Download Manager channel.
This would be the place to send a hang-up command to a
Modem if that is desired.

Return value: 1 when finished
*********************************************************************/
nodebug
int CloseComm()
{

	/* wait for all bytes to be de-queued (sent). */
#ifdef USE_SERIAL_PORT_B
	while(serBwrUsed() != 0) ;
	serBclose();
#else
#ifdef USE_SERIAL_PORT_C
	while(serCwrUsed() != 0) ;
	serCclose();
#endif
#endif

	return 1;
}

/*********************************************************************
UserTask()

Called every pass through the main loop. Control tasks that must be
performed while the DLM runs should handled here.

Return Value: - undefined
*********************************************************************/
nodebug
int UserTask()
{

}

