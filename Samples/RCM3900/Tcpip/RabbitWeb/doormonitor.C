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
/*******************************************************************************
   doormonitor.c

   This sample program is used with RCM3900 series controllers and RCM3300
   prototyping boards.

   An optional LCD/Keypad Module is required to run this program.  The brdInit()
   function in rcm39xx.lib automatically enables the Rabbit's external I/O bus
   for LCD/Keypad Module operations.

   Description
   ===========
   This sample program demonstrates using the enhanced RabbitWeb HTTP library
   and web interface to add/change passwords and names.  Entry and exit times
   are monitored while passwords are entered on the LCD/Keypad Module.

   See also the Pages/doormonitor.zhtml page that demonstrates the corresponding
   ZHTML scripting features.

   Instructions
   ============
   0. This program assumes that the controller's real-time clock time has been
      previously set.

   1. If necessary, make changes below in the configuration section to match
      your requirements.

   2. Compile and run this program.

   3. The LCD/Keypad Module's keypad is configured as:
         [ 1 ] [ 2 ] [ 3 ] [ 4 ]
            [ - ] [ + ] [ 0 ]

   4. On the LCD/Keypad Module, key in 1 2 3 4 0 and then press + to enter the
      password.

   5. Using your PC's web browser, access the web page running on the
      controller.

   6. On the web page, click the Update Monitor button.  You should see a new
      date and time logged for the Last Entry.

   7. On the web page, enter a new password and owner name then click the Submit
      Password button.

   8. On the LCD/Keypad module, key in the new password and then press - to
      enter it.

   9. On the web page, click the Update Monitor button.  You should see a new
      date and time logged for the Last Exit.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

/********************************
 * End of configuration section *
 ********************************/

/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1
#define USE_ZHTML 1

#memmap xmem

#use "dcrtcp.lib"
#use "http.lib"

/*
 * This page contains the ZHTML portion of the selection variable demonstration.
 */
#ximport "pages/doormonitor.zhtml" doormonitor_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
   SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
   SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END


/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
   SSPEC_RESOURCE_XMEMFILE("/", doormonitor_zhtml),
   SSPEC_RESOURCE_XMEMFILE("/index.zhtml", doormonitor_zhtml)
SSPEC_RESOURCETABLE_END

#define MAX_ENTRIES 20

/*
 * Structure variable to be registered.  Note that you can use watch
 * expressions or the evaluate expression feature during runtime to ensure that
 * the variables have properly changed values.
 */

struct {
   char ownerpw[8];
   char ownername[21];
   char entrypw[8];
   char entryname[21];
   char entrytime[64];
   char exittime[64];
} passwords[MAX_ENTRIES];

#web passwords[@].entrypw
#web passwords[@].entryname
#web passwords[@].entrytime
#web passwords[@].exittime

void updateentry(void)
{
   auto int entries;

   for (entries = 0; entries < MAX_ENTRIES; ++entries) {
      strcpy(passwords[entries].ownerpw, passwords[entries].entrypw);
      strcpy(passwords[entries].ownername, passwords[entries].entryname);
   }
}

#web_update passwords[@].entrypw updateentry
#web_update passwords[@].entryname updateentry

void gettime(char *intime)
{
   auto struct tm rtm;		// time struct
   auto unsigned long t0;	// used for date->seconds conversion

   //////////////////////////////////////////////////
   // read current date/time
   t0 = read_rtc();		// read time in seconds since 1980
   mktm(&rtm, t0);

   sprintf(intime, "%02d/%02d/%04d %02d:%02d:%02d",
           rtm.tm_mon, rtm.tm_mday, 1900+rtm.tm_year,
           rtm.tm_hour, rtm.tm_min, rtm.tm_sec);
}

int confirmentry(char *inputpw, char entrytype)
{
   auto int i, passkey;

   for (i = 0, passkey = 0; i < MAX_ENTRIES; ++i) {
      if (!strcmp(passwords[i].ownerpw, inputpw)) {
         passkey = 1;
         strcpy(inputpw, passwords[i].ownername);
         if (entrytype == '+') {
            gettime(passwords[i].entrytime);
         }
         if (entrytype == '-') {
            gettime(passwords[i].exittime);
         }
         break;
      }
   }
   return passkey;
}

fontInfo fi6x8;
windowFrame wholewindow;

void initLcdKeypad(void)
{
   // Start-up the keypad driver and initialize the graphic driver
   dispInit();

   // Use default key values along with a key release code
   keyConfig(0, '1', 0, 0, 0,  0, 0);
   keyConfig(1, '2', 0, 0, 0,  0, 0);
   keyConfig(2, '3', 0, 0, 0,  0, 0);
   keyConfig(3, '4', 0, 0, 0,  0, 0);
   keyConfig(4, '-', 0, 0, 0,  0, 0);
   keyConfig(5, '+', 0, 0, 0,  0, 0);
   keyConfig(6, '0', 0, 0, 0,  0, 0);

   // Initialize 6x8 font
   glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);
   TextWindowFrame(&wholewindow, &fi6x8, 0, 0, LCD_XS, LCD_YS);
   glBlankScreen();
   TextGotoXY(&wholewindow, 0, 0);
   TextPrintf(&wholewindow, "Password");
}

void main(void)
{
   // auto variables in main() permanently consume precious stack space
   static char buffer[20];
   static int i, j, wKey;

   // it's just good practice to initialize Rabbit's board-specific I/O
   brdInit();
   initLcdKeypad();

   // Initialize the #web-registered variables
   strcpy(passwords[0].entrypw, "12340");
   strcpy(passwords[0].entryname, "JGunchy");
   strcpy(passwords[0].entrytime, "no activity");
   strcpy(passwords[0].exittime, "no activity");

   for (i = 1; i < MAX_ENTRIES; ++i) {
      strcpy(passwords[i].entrypw, "Unused");
      strcpy(passwords[i].entryname, "Unused");
      strcpy(passwords[i].entrytime, "no activity");
      strcpy(passwords[i].exittime, "no activity");
   }

   updateentry();
   // initialize temporary variables
   j = 10;
   i = 0;

   // Initialize the TCP/IP stack, HTTP server
   sock_init();
   http_init();

   while (1) {
      // Drive the HTTP server
      http_handler();

      keyProcess();
      wKey = keyGet();

      costate {
         switch (wKey) {
         case 0:
            break;
         case '+':	//entry
         case '-':	//exit
            buffer[i] = '\0';
            TextGotoXY(&wholewindow, 0, 1);
            if (confirmentry(buffer, wKey)) {
               TextPrintf(&wholewindow, "Accepted");
               TextGotoXY(&wholewindow, 0, 3);
               TextPrintf(&wholewindow, "Thank you %s", buffer);
            } else {
               TextPrintf(&wholewindow, "Incorrect password");
            }
            i = 0;
            j = 10;
            waitfor(DelaySec(3));
            glBlankScreen();
            TextGotoXY(&wholewindow, 0, 0);
            TextPrintf(&wholewindow, "Password");
            break;
         default:
            if (i < 7) {
               buffer[i] = wKey;
               TextGotoXY(&wholewindow, j, 0);
               //TextPrintf(&wholewindow, "%c", wKey);
               TextPrintf(&wholewindow, "%c", '*');
               ++j;
               ++i;
            }
            break;
         }	// end switch
      }	// end costate
   }	// end while
}

