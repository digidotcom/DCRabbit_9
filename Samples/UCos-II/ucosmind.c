/******************************************************************

      uCOSMind.c
      Z-World, 2002

  Simple master mind type game demonstrating the use of event
  flags and mutexes.

  The board is made up of 10 rows, each row has four columns to
  display color patterns, and one larger column on the right for
  displaying the number of correct colors, and the number of
  correct colors in correct locations.  The player enters a guess
  into the "Current Guess" row by using the arrow keys on the
  numeric keypad - arrow up and arrow down change the color of the
  current location, arrow right and arrow down change the current
  location, and enter submits the guess.  The "+" symbol below the
  "Current Guess" row shows which cell is active.

  Turn on "num lock" on your keyboard and then use the keypad to
  navigate the user interface.  Up-arrow and down-arrow are really
  8 and 2, for instance.  Hit ENTER to submit your guess.

  The first task is responsible for determining a pattern of
  colors, and then checking the guess of the user against the
  pattern.  If the user guesses one or more colors correctly,
  a "-" is displayed for each correct color.  If the user guesses
  one or more correct colors in the correct location, a "+" is
  displayed for each correct color in the correct position.
  The user gets 10 guesses before another pattern is chosen.

  The second task interacts with the user and gets a color pattern
  which is then checked by the first task.  Colors are entered by
  using the arrow keys on the numeric keypad - up and down change the
  color of the current location, right and left change the current
  location, and enter submits the guess.

See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.

******************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

// if this is set to 1, the current pattern is shown at the bottom
// of the grid
#define CHEAT_SHOW_PATTERN		0

#define OS_FLAG_EN			1					// enable event flags
#define OS_MUTEX_EN			1					// enable mutexes

// ucos library must be explicity used
#use "ucos2.lib"

// color definitions
#define COLRED			0
#define COLBLUE		1
#define COLYELLOW		2
#define COLGREEN		3
#define COLWHITE		4
#define COLCYAN		5
#define COLEND			6

// ansi color definitions
#define FGBLACK		"\033[30m"
#define FGRED  		"\033[31m"
#define FGGREEN      "\033[32m"
#define FGYELLOW     "\033[33m"
#define FGBLUE       "\033[34m"
#define FGMAGENTA    "\033[35m"
#define FGCYAN 		"\033[36m"
#define FGWHITE      "\033[37m"

#define BGBLACK 		"\033[40m"
#define BGRED       	"\033[41m"
#define BGGREEN      "\033[42m"
#define BGYELLOW     "\033[43m"
#define BGBLUE       "\033[44m"
#define BGMAGENTA    "\033[45m"
#define BGCYAN       "\033[46m"
#define BGWHITE      "\033[47m"

#define DEF_TEXT_COLOR 	(FGWHITE BGBLACK)

// event flag definitions
#define FLAG_GUESS_DONE					0x01
#define FLAG_COLORS_DONE				0x02
#define FLAG_POSITIONS_DONE			0x04
#define FLAG_PATTERN_DONE				0x08
#define FLAG_COLOR_MARKERS_DONE		0x10
#define FLAG_POSITION_MARKERS_DONE	0x20

// globals
OS_EVENT* GuessMutex;
OS_FLAG_GRP* ColorFlags;
BOOLEAN GuessDone;
char* const Colors[6] = {BGRED, BGBLUE, BGYELLOW, BGGREEN, BGWHITE, BGCYAN};
int CurrentPattern[4];
int PatternGuess[4];
int CorrectLocations;
int CorrectColors;

// function prototypes
void MasterTask(void* pdata);
void UserTask(void* pdata);
void DispStr(int x, int y, char* color, char *s);
void InitDisplay();

void main()
{
	INT8U err;
	OSInit();
	InitDisplay();
	// create tasks with 512 byte stacks.
	OSTaskCreate(MasterTask, NULL, 512, 6);
	OSTaskCreate(UserTask,   NULL, 512, 7);

	// create mutex used by tasks.  Priority inversion
	// priority is set to 5 (1 less that highest priority
	// task).
	GuessMutex = OSMutexCreate(5, &err);
	if(err != OS_NO_ERR)
		exit(err);

	// create event flags used by tasks, initially set to 0.
	ColorFlags = OSFlagCreate(0x00, &err);
	if(err != OS_NO_ERR)
		exit(err);
	OSStart();
}

void MasterTask(void* pdata)
{
	auto int guesses;
	auto INT8U err;
	auto int i, j;
	auto int r, b, y, g, w, c;

	GuessDone = FALSE;

	while(1)
	{
		guesses = 10;
		// choose a pattern of colors
		CurrentPattern[0] = (int)(rand() * COLEND);
		CurrentPattern[1] = (int)(rand() * COLEND);
		CurrentPattern[2] = (int)(rand() * COLEND);
		CurrentPattern[3] = (int)(rand() * COLEND);

#if CHEAT_SHOW_PATTERN == 1
		DispStr(6,  24, Colors[CurrentPattern[0]], " ");
		DispStr(10, 24, Colors[CurrentPattern[1]], " ");
		DispStr(14, 24, Colors[CurrentPattern[2]], " ");
		DispStr(18, 24, Colors[CurrentPattern[3]], " ");
#endif

		while(guesses--)
		{
			// Poll, waiting for user to finish with a guess.
			// This is not the most efficient way of accomplishing
			// this task, but is done only to demonstrate the use
			// of a mutex.
			while(1)
			{
				OSMutexPend(GuessMutex, 0, &err);
				if(err != OS_NO_ERR)
					exit(err);
				if(GuessDone)
				{
					GuessDone = FALSE;
					OSMutexPost(GuessMutex);
					break;
				}
				OSMutexPost(GuessMutex);
				OSTimeDly(10);
			}
			OSFlagPost(ColorFlags, FLAG_GUESS_DONE, OS_FLAG_SET, &err);
			if(err != OS_NO_ERR)
				exit(err);

			// compare users guess against current pattern
			// check number of correct colors
			r = b = y = g = w = c = 0;
			for(i = 0; i < 4; i++)
			{
				switch(PatternGuess[i])
				{
					case COLRED:
						r++;
						break;
					case COLBLUE:
						b++;
						break;
					case COLYELLOW:
						y++;
						break;
					case COLGREEN:
						g++;
						break;
					case COLWHITE:
						w++;
						break;
					case COLCYAN:
						c++;
						break;
				}
			}
			CorrectColors = 0;
			for(i = 0; i < 4; i++)
			{
				switch(CurrentPattern[i])
				{
					case COLRED:
						if(r)
						{
							CorrectColors++;
							r--;
						}
						break;
					case COLBLUE:
						if(b)
						{
							CorrectColors++;
							b--;
						}
						break;
					case COLYELLOW:
						if(y)
						{
							CorrectColors++;
							y--;
						}
						break;
					case COLGREEN:
						if(g)
						{
							CorrectColors++;
							g--;
						}
						break;
					case COLWHITE:
						if(w)
						{
							CorrectColors++;
							w--;
						}
						break;
					case COLCYAN:
						if(c)
						{
							CorrectColors++;
							c--;
						}
						break;
				}
			}
			OSFlagPost(ColorFlags, FLAG_COLORS_DONE, OS_FLAG_SET, &err);
			if(err != OS_NO_ERR)
				exit(err);

			// check number of correct locations
			CorrectLocations = 0;
			for(i = 0; i < 4; i++)
			{
				if(PatternGuess[i] == CurrentPattern[i])
					CorrectLocations++;
			}
			OSFlagPost(ColorFlags, FLAG_POSITIONS_DONE, OS_FLAG_SET, &err);
			if(err != OS_NO_ERR)
				exit(err);

			// put guessed pattern onto board
			DispStr(6,  4 + (2 * guesses), Colors[PatternGuess[0]], " ");
			DispStr(10, 4 + (2 * guesses), Colors[PatternGuess[1]], " ");
			DispStr(14, 4 + (2 * guesses), Colors[PatternGuess[2]], " ");
			DispStr(18, 4 + (2 * guesses), Colors[PatternGuess[3]], " ");
			OSFlagPost(ColorFlags, FLAG_PATTERN_DONE, OS_FLAG_SET, &err);
			if(err != OS_NO_ERR)
				exit(err);

			// put matching markers onto board
			// a "-" represents a correct color in an incorrect location
			// subtract out matches from the correctcolor count to avoid
			// double counting
			switch(CorrectColors - CorrectLocations)
			{
				case 1:
					DispStr(28, 4 + (2 * guesses), DEF_TEXT_COLOR, "-");
					break;
				case 2:
					DispStr(26, 4 + (2 * guesses), DEF_TEXT_COLOR, "- -");
					break;
				case 3:
					DispStr(24, 4 + (2 * guesses), DEF_TEXT_COLOR, "- - -");
					break;
				case 4:
					DispStr(22, 4 + (2 * guesses), DEF_TEXT_COLOR, "- - - -");
					break;
			}
			OSFlagPost(ColorFlags, FLAG_COLOR_MARKERS_DONE, OS_FLAG_SET, &err);
			if(err != OS_NO_ERR)
				exit(err);

			// a "+" means that there is a correct color in the correct
			// location, but its position does not necessarily correlate
			// to the position of the color.
			switch(CorrectLocations)
			{
				case 1:
					DispStr(22, 4 + (2 * guesses), FGRED BGBLACK, "+");
					break;
				case 2:
					DispStr(22, 4 + (2 * guesses), FGRED BGBLACK, "+ +");
					break;
				case 3:
					DispStr(22, 4 + (2 * guesses), FGRED BGBLACK, "+ + +");
					break;
				case 4:
					DispStr(22, 4 + (2 * guesses), FGRED BGBLACK, "+ + + +");
					break;
			}
			// if the patterns match, wait a couple of seconds before starting over
			if(CorrectLocations == 4)
			{
				// wait for 2 seconds before redrawing display and starting new game
				OSTimeDly(2 * OS_TICKS_PER_SEC);
			}
			OSFlagPost(ColorFlags, FLAG_POSITION_MARKERS_DONE, OS_FLAG_SET, &err);
			if(err != OS_NO_ERR)
				exit(err);

			// if the patterns match, break out of guessing loop
			if(CorrectLocations == 4)
				break;
		}
		if(guesses <= 0)
		{
			DispStr(6,  24, Colors[CurrentPattern[0]], " ");
			DispStr(10, 24, Colors[CurrentPattern[1]], " ");
			DispStr(14, 24, Colors[CurrentPattern[2]], " ");
			DispStr(18, 24, Colors[CurrentPattern[3]], " ");
			OSTimeDly(3 * OS_TICKS_PER_SEC);
		}
		InitDisplay();
	}
}

void UserTask(void* pdata)
{
	auto char key;
	auto int CurrentX;
	auto int CurrentY;
	auto int CurrentChoice;
	auto BOOLEAN done;
	auto int position;
	auto INT8U err;
	auto INT16U returnedflags;
	auto char num[2];

	while(1)
	{
		done = FALSE;
		CurrentX = 11;
		CurrentY = 27;
		CurrentChoice = 0;
		position = 0;
		returnedflags = 0;

		memset(PatternGuess, 0, sizeof(PatternGuess));
		DispStr(CurrentX,      CurrentY, Colors[CurrentChoice], " ");
		DispStr(CurrentX + 4,  CurrentY, BGBLACK, " ");
		DispStr(CurrentX + 8,  CurrentY, BGBLACK, " ");
		DispStr(CurrentX + 12, CurrentY, BGBLACK, " ");
		DispStr(CurrentX, CurrentY + 2, DEF_TEXT_COLOR, "+");

		// get a pattern from the user
		while(!done)
		{
			if(kbhit())
			{
		      key = getchar();
   		   switch(key)
      		{
					case 0x38:	// up
						CurrentChoice++;
						CurrentChoice %= COLEND;
						DispStr(CurrentX, CurrentY, Colors[CurrentChoice], " ");
						PatternGuess[position] = CurrentChoice;
						break;
					case 0x32:	// down
						CurrentChoice--;
						if(CurrentChoice < 0) CurrentChoice = COLEND - 1;
						CurrentChoice %= COLEND;
						DispStr(CurrentX, CurrentY, Colors[CurrentChoice], " ");
						PatternGuess[position] = CurrentChoice;
						break;
					case 0x34:  // left
						if(position > 0)
						{
							DispStr(CurrentX, CurrentY + 2, DEF_TEXT_COLOR, " ");
					   	position--;
							CurrentChoice = PatternGuess[position];
					   	CurrentX -= 4;
							DispStr(CurrentX, CurrentY, Colors[CurrentChoice], " ");
							DispStr(CurrentX, CurrentY + 2, DEF_TEXT_COLOR, "+");
						}
						break;
					case 0x36:  // right
						if(position < 3)
						{
							DispStr(CurrentX, CurrentY + 2, DEF_TEXT_COLOR, " ");
					   	position++;
							CurrentChoice = PatternGuess[position];
					   	CurrentX += 4;
							DispStr(CurrentX, CurrentY, Colors[CurrentChoice], " ");
					   	DispStr(CurrentX, CurrentY + 2, DEF_TEXT_COLOR, "+");
						}
						break;
					case 0x0d:
						done = TRUE;
						DispStr(CurrentX, CurrentY + 2, DEF_TEXT_COLOR, " ");
						break;
   		   }
   		}
   	}

   	// let the master task know which colors in what order were selected
		OSMutexPend(GuessMutex, 0, &err);
		if(err != OS_NO_ERR)
			exit(err);
		GuessDone = TRUE;
		OSMutexPost(GuessMutex);

   	// pend on flags from master task to let us know how many colors
   	// were selected in the correct location
		OSFlagPend(ColorFlags, FLAG_GUESS_DONE | FLAG_COLORS_DONE | FLAG_POSITIONS_DONE |
					  FLAG_PATTERN_DONE | FLAG_COLOR_MARKERS_DONE | FLAG_POSITION_MARKERS_DONE,
					  OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);

		// Once all flags have been set by master task, CorrectLocations and
		// CorrectColors are valid.  If CorrectColors and CorrectLocations are both
		// 4, then correct pattern has been guessed.
		if(CorrectColors != 4 && CorrectLocations != 4)
		{
			sprintf(num, "%d", CorrectColors);
			DispStr(29, 30, FGWHITE BGBLACK, num);
			sprintf(num, "%d", CorrectLocations);
		   DispStr(29, 31, FGWHITE BGBLACK, num);
		 }
	}
}

nodebug void InitDisplay()
{
	DispStr(0,  0,  DEF_TEXT_COLOR, "                                   ");
	DispStr(0,  1,  DEF_TEXT_COLOR, "           uC/OS-II Demo           ");
	DispStr(0,  2,  DEF_TEXT_COLOR, "          ---------------          ");
	DispStr(0,  3,  DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  4,  DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  5,  DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  6,  DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  7,  DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  8,  DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  9,  DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  10, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  11, DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  12, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  13, DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  14, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  15, DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  16, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  17, DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  18, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  19, DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  20, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  21, DEF_TEXT_COLOR, "    ---------------------------    ");
	DispStr(0,  22, DEF_TEXT_COLOR, "    |   |   |   |   |         |    ");
	DispStr(0,  23, DEF_TEXT_COLOR, "    ---------------------------	   ");
	DispStr(0,  24, DEF_TEXT_COLOR, "                                   ");
	DispStr(0,  25, DEF_TEXT_COLOR, "           Current Guess           ");
	DispStr(0,  26, DEF_TEXT_COLOR, "         -----------------         ");
	DispStr(0,  27, DEF_TEXT_COLOR, "         |   |   |   |   |         ");
	DispStr(0,  28, DEF_TEXT_COLOR, "         -----------------         ");
	DispStr(0,  29, DEF_TEXT_COLOR, "                                   ");
	DispStr(0,  30, DEF_TEXT_COLOR, "         Correct Colors   :        ");
	DispStr(0,  31, DEF_TEXT_COLOR, "         Correct Locations:        ");
	DispStr(0,  32, DEF_TEXT_COLOR, "                                   ");
	DispStr(0,  33, DEF_TEXT_COLOR, "                                   ");
}

nodebug void DispStr(int x, int y, char* color, char *s)
{
   x += 0x20;
   y += 0x20;
   printf("\x1B=%c%c%s%s", x, y, color, s);
}
