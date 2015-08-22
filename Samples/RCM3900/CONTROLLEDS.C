/*******************************************************************************

	controlleds.c
	Rabbit, 2007

	This sample program is used with RCM3900 series controllers and RCM3300
	prototyping boards.

	Description
	===========
	This sample program demonstrates controlling port outputs from STDIO by
	toggling LEDs on the prototyping board and the RCM39xx.

	Instructions
	============
	1. Compile and run this program.

	2. The program will prompt for a DS3, DS4, DS5, DS6 or User LED selection.
	   Make a selection from your PC's keyboard.

	3. After you have made the LED selection you'll be prompted to select an ON
	   or OFF state.  Logic "1" will light up the LED.

	4. The program repeats from step 2 until the User presses 'Q' or 'q' to quit.

*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

// the number of LEDs we control
#define NUM_LEDS 5

// the possible states of the LEDs we control
#define ON 1
#define OFF 0

// brief post-selection delay to display User's selection before acting on it
#define DELAY_MILLISECONDS 1000ul

// the number of display line buffers we need
#define NUM_DISPLAYLINES 3

// enumeration of the required display line buffers
enum DISPLAY_STRINGS {
	SELECT_DispStr,
	TEMP_DispStr,
	WORK_DispStr
};

// handy structure typedef for LEDs' name, channel and state information
typedef struct LEDs_info_t {
	char *name;
	char select;
	int channel;
	int state;
} LEDs_info_td;

// global LEDs information initialization structure array
const LEDs_info_td LEDs_info_init[NUM_LEDS] = {
	{ "DS3", '3', 3, OFF },
	{ "DS4", '4', 4, OFF },
	{ "DS5", '5', 5, OFF },
	{ "DS6", '6', 6, OFF },
	{ "USER", '7', 0, OFF }
};

// global LEDs information working structure array
LEDs_info_td LEDs_info[NUM_LEDS];

////////
// set the STDIO cursor location and display a string
////////
nodebug
void DispStr(int x, int y, char *s)
{
	x += 0x20;
	y += 0x20;
	printf("\x1B=%c%c%s", x, y,
	       "                                                                  ");
	printf("\x1B=%c%c%s", x, y, s);
}

////////
// handy N millisecond delay function
////////
nodebug
void msDelay(unsigned long delay)
{
	auto unsigned long time0;

	for (time0 = MS_TIMER; MS_TIMER - time0 < delay; );
}

void main(void)
{
	static char display[NUM_DISPLAYLINES][128];
	auto int control_LED, i, temp_int;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	// initialize all controlled LEDs
	for (i = 0; i < NUM_LEDS ; ++i) {
		LEDs_info[i] = LEDs_info_init[i];
		ledOut(LEDs_info[i].channel, LEDs_info[i].state);
	}

	// display User instructions and channel headings
	DispStr(8, 2, "<<< Controlled LEDs' status: >>>");

	// build then display the controlled LEDs names heading
	display[WORK_DispStr][0] = '\0';	// initialize for strcat function
	for (i = 0; i < NUM_LEDS; ++i) {
		sprintf(display[TEMP_DispStr], "%s\t", LEDs_info[i].name);
		strcat(display[WORK_DispStr], display[TEMP_DispStr]);
	}
	DispStr(8, 4, display[WORK_DispStr]);

	// build then display the controlled LEDs names heading separator
	display[WORK_DispStr][0] = '\0';	// initialize for strcat function
	for (i = 0; i < NUM_LEDS; ++i) {
		temp_int = strlen(LEDs_info[i].name);
		memset(display[TEMP_DispStr], '-', temp_int);
		display[TEMP_DispStr][temp_int] = '\t';
		display[TEMP_DispStr][temp_int + 1] = '\0';
		strcat(display[WORK_DispStr], display[TEMP_DispStr]);
	}
	DispStr(8, 5, display[WORK_DispStr]);

	// build the controlled LEDs selection string
	strcpy(display[SELECT_DispStr], "Select");	// init for strcat function
	for (i = 0; i < NUM_LEDS; ++i) {
		sprintf(display[TEMP_DispStr], "%s%c=%s",
		        0 == i ? " " : ", ", LEDs_info[i].select, LEDs_info[i].name);
		strcat(display[SELECT_DispStr], display[TEMP_DispStr]);
	}
	strcat(display[SELECT_DispStr], " LED to control.");

	DispStr(8, 10, "From PC keyboard:");
	DispStr(8, 21, "< Press 'Q' To Quit >");

	// loop until User presses the upper/lower case 'Q' key
	for (;;) {
		// update LEDs' outputs; build then display updated LEDs' status string
		display[WORK_DispStr][0] = '\0';	// initialize for strcat function
		for (i = 0; i < NUM_LEDS; ++i) {
			ledOut(LEDs_info[i].channel, LEDs_info[i].state);
			// format logic level for display
			sprintf(display[TEMP_DispStr], "%s\t",
			        ON == LEDs_info[i].state ? "ON " : "OFF");
			strcat(display[WORK_DispStr], display[TEMP_DispStr]);
		}
		DispStr(8, 6, display[WORK_DispStr]);

		// wait for User to make output channel selection or exit program
		DispStr(8, 12, display[SELECT_DispStr]);
		do {
			temp_int = toupper(getchar());
			// check if it's the upper/lower case 'Q' key
			if ('Q' == temp_int) {
				exit(0);
			}
			for (i = 0; i < NUM_LEDS; ++i) {
				if (temp_int == LEDs_info[i].select) {
					// found our selection, quit looking
					break;
				}
			}
		} while (NUM_LEDS == i);	// I.E.: until temp_int selection is found

		// save the selection, display the channel that the User has selected
		control_LED = i;
		sprintf(display[WORK_DispStr], "Selected %s LED to control.",
		        LEDs_info[control_LED].name);
		DispStr(8, 12, display[WORK_DispStr]);

		// wait for User to select logic level or exit program
		sprintf(display[WORK_DispStr], "Select %d=ON or %d=OFF.", ON, OFF);
		DispStr(8, 13, display[WORK_DispStr]);
		do {
			temp_int = toupper(getchar());
			// check if it's the q or Q key
			if ('Q' == temp_int) {
				exit(0);
			}
			temp_int -= '0';
		} while (ON != temp_int && OFF != temp_int);
		sprintf(display[WORK_DispStr], "Selected %s.",
		        ON == temp_int ? "ON" : "OFF");
		DispStr(8, 13, display[WORK_DispStr]);
		LEDs_info[control_LED].state = temp_int;

		// momentary delay
		msDelay(DELAY_MILLISECONDS);

		// Clear channel and logic level selection prompts
		DispStr(8, 12, "");
		DispStr(8, 13, "");
	}
}

