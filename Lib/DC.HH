/*** BeginHeader */
#ifndef __DCHH_LIB
#define __DCHH_LIB
/*** EndHeader */

/***************************************************************************\
	Function Prototypes for Dynamic C RunTime Library
		Copyright (c) 1999, Z-World.
\***************************************************************************/

/*** BeginHeader main */
#ifndef CoData
#define CoData FuncBlk
#endif

#nointerleave

#nouseix    	 // default do not use ix register as frame pointer

#define offsetof(s, f) ((unsigned int)((char*)(&((s*)0)->f) - ((char*)0)))

#define FALSE       0
#define TRUE        1

#define NULL    (void *) 0
#define EOF     -1

#define FLOAT           float

#define BADTAN          (FLOAT)1.560796327
#define EXPLARGE        (FLOAT)89.80081863
#define HUGE_VAL    		(FLOAT)3.00e38
#define INF             (FLOAT)3.00e38
#define IPIby180        (FLOAT)57.29577951
#define LNof10          (FLOAT)2.302585093
#define LOG2            (FLOAT)0.30102999567
#define LOGE            (FLOAT)0.43429448190
#define ONE             (FLOAT)1.0
#define ONE_HALF        (FLOAT)0.500000
#define OVERFLOW        49
#define PI              (FLOAT)3.14159265359
#define PIby180         (FLOAT)0.0174532925
#define PIbyTWO         (FLOAT)1.570796326795
#define POW10INF        (FLOAT)38.0
#define SQR10           (FLOAT)3.162277660168
#define TWObyPI         (FLOAT)0.63661977
#define ZERO            (FLOAT)0.0

#use "limits.h"

typedef struct  {
			char    *STACKP;
			char    *IX;
			char    *RETADR;
			char    RETXPC;
         char    RETSTACKSEG;
	}   jmp_buf[1];

struct free_list {
					 int  sz;
					 struct free_list *next;
				 };

extern char STORE_TABLE[256];	// Pointer Store Table

// remove obsolete keywords
#define fast
#define indirect
#define register

struct tm
{
	char tm_sec;             // seconds 0-59
	char tm_min;             // 0-59
	char tm_hour;            // 0-59
	char tm_mday;            // 1-31
	char tm_mon;             // 1-12
	char tm_year;            // 00-150 (1900-2050)
	char tm_wday;            // 0-6 0==sunday
};

typedef unsigned short size_t;

#define ASM_NOIX nodebug speed nouseix root
#define ASM_IX nodebug speed useix

// define target pseudo-overflow flag
#define ovf lo			// overflow
#define novf lz 		// no overflow
#define ddip 1      	// default disable interrupt priority

// disable/restore interrupts assembly sequence
#define diasmseq(priority)	push ip $ ipset priority
#define riasmseq           pop ip

/*** EndHeader */

/*** BeginHeader */
#endif
/*** EndHeader */