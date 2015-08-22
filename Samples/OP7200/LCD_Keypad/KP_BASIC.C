/***************************************************************************
	kp_basic.c
	
	Z-World, 2001
	Sample program to demonstrate the keypad functions. This program
	will display the following in the STDIO display window:
	
	1. Displays default ASCII keypad return values.
	2. Displays custom ASCII keypad return values.
	3. Demonstrates keypad repeat functionality. 

	Instructions:
	1. Compile and run this program.
	2. Press each key on the controller keypad at least once when prompted.
	3. When the "Key Repeat" prompt appears, press the key and hold it down
	   for a least 1-2 seconds to see the "Key Repeat" feature in action.

***************************************************************************/
#class auto		// default storage class for variables in function: on stack.
#memmap xmem  // Required to reduce root memory usage 

void main (	void	)		
{
	unsigned	wKey;		//	User Keypress
	int keyflag, keypad_active, done;
	
	brdInit();		// Required for all controllers.
	keyInit();		// Start-up the keypad driver
	keypadDef();	// Use the default keypad ASCII return values

	printf("\n\n");
	printf("LCD/Keypad Sample Program\n");
	printf("---------------------------\n");

	for(;;)
	{
	
		costate
		{								//	Process Keypad Press/Hold/Release
			keyProcess ();
			waitfor ( DelayMs(10) );
		}
			
		costate
		{
			//------------------------------------------------------------------------------------------
			//	Default Key demo
			//------------------------------------------------------------------------------------------
			
			printf("\n\nPress all keys on the keypad to see Default ASCII return values!\n\n\n");
			keypadDef();
			keyflag = 0;
			done = FALSE;
	
			while(!done)
			{
				waitfor ( wKey = keyGet() );	//	Wait for Keypress
				printf("KEY PRESSED = %c\n\r", wKey);
				switch(wKey)
				{			   
					case 'E': 	keyflag |= 0x01; 	break;
					case '+':	keyflag |= 0x02;	break;
					case '-':	keyflag |= 0x04;	break;
					case 'D':	keyflag |= 0x08;	break;
					case 'U':	keyflag |= 0x10;	break;
					case 'R':	keyflag |= 0x20;	break;
					case 'L':	keyflag |= 0x40;	break;
					case 'S':	keyflag |= 0x80;	break;
					case 'B':   keyflag |= 0x100;	break;
				}
				if(keyflag == 0x1FF)
				{
					done = TRUE;
				}
			}

			//------------------------------------------------------------------------------------------
			//	Custom Key demo
			//------------------------------------------------------------------------------------------						
			printf("\n\nReconfigured keypad for custom ASCII return values...Press all keys again!\n\n\n");
			keyConfig ( 0,'A',0, 0, 0,  0, 0 );
			keyConfig ( 1,'B',0, 0, 0,  0, 0 );
			keyConfig ( 2,'C',0, 0, 0,  0, 0 );
			keyConfig ( 3,'D',0, 0, 0,  0, 0 );
			keyConfig ( 4,'E',0, 0, 0,  0, 0 );
			keyConfig ( 5,'F',0, 0, 0,  0, 0 );
			keyConfig ( 6,'G',0, 0, 0,  0, 0 );
			keyConfig ( 7,'H',0, 0, 0,  0, 0 );	
			keyConfig ( 8,'I',0, 0, 0,  0, 0 );  	
					
			keyflag = 0;		
			done = FALSE;
			while(!done)
			{
				waitfor ( wKey = keyGet() );	//	Wait for Keypress
				printf("KEY PRESSED = %c\n\r", wKey);
				switch(wKey)
				{
				   case 'A': 	keyflag |= 0x01; 	break;
					case 'B':	keyflag |= 0x02;	break;
					case 'C':	keyflag |= 0x04;	break;
					case 'D':	keyflag |= 0x08;	break;
					case 'E':	keyflag |= 0x10;	break;
					case 'F':	keyflag |= 0x20;	break;
					case 'G':	keyflag |= 0x40;	break;
					case 'H':	keyflag |= 0x80;	break;
					case 'I':   keyflag |= 0x100;	break;
				}
				if(keyflag == 0x1FF)
				{
					done = TRUE;
				}
				
			}

			//------------------------------------------------------------------------------------------
			//	Key repeat demo
			//------------------------------------------------------------------------------------------
			
			printf("\n\nReconfigured keys back to the default with fast repeat enabled\n\n\r");			
			keypadDef();

			// Setup for FAST key repeat after holding down key for 150 ticks.
			//
			// Note: The keyConfig function "cCntHold" parameter along with the delay
			// between keyprocess determines the initial KEY hold time before it starts
			// to repeat. The delay being used to control the hold time is in the first
			// costatement in this program:
			//
			//  
			//		costate
			//		{								//	Process Keypad Press/Hold/Release
			//			keyProcess ();
			//			waitfor ( DelayMs(10) );
			//		}
			//
		
			// Left-Scroll and Right-Scroll
			keyConfig (  5,'L',0, 100, 1,  1, 1);
			keyConfig (  7,'R',0, 100, 1,  1, 1);
				
			// Scroll-down and Scroll-up
			keyConfig (  8,'D',0, 100, 1,  1, 1);
			keyConfig (  2,'U',0, 100, 1,  1, 1);

			// Page-up and Page-down
			keyConfig (  3,'-',0, 100, 1,  1, 1);
			keyConfig (  1,'+',0, 100, 1,  1, 1);
					
			// Space and Backspace
			keyConfig (  0,'B',0, 100, 1,  1, 1);
			keyConfig (  4,'S',0, 100, 1,  1, 1);

			// Enter key
			keyConfig (  6,'E',0, 100, 1,  1, 1);

			keyflag = 0;		
			done = FALSE;
			while(!done)
			{
				waitfor ( wKey = keyGet() );	//	Wait for Keypress
				printf("KEY PRESSED = %c\n\r", wKey);
				switch(wKey)
				{
					case 'E': 	keyflag |= 0x01; 	break;
					case '+':	keyflag |= 0x02;	break;
					case '-':	keyflag |= 0x04;	break;
					case 'D':	keyflag |= 0x08;	break;
					case 'U':	keyflag |= 0x10;	break;
					case 'R':	keyflag |= 0x20;	break;
					case 'L':	keyflag |= 0x40;	break;
					case 'S':	keyflag |= 0x80;	break;
					case 'B':   keyflag |= 0x100;	break;
				}
				if(keyflag == 0x1FF)
				{
					done = TRUE;
				}
			}
		}
	}
}
	
