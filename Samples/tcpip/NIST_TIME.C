/*******************************************************************************
	Samples\TCPIP\NIST_time.c
	Z-World, 2001

	An example of accessing the current time in seconds via the Internet
	from a server at the National Institute for Standards and Technology
	(NIST).

	Run this program on an Ethernet-enabled board that is connected to
	the Internet to set the real-time clock to the correct time (within
	5 seconds).

	For more information about the time services available from NIST,
	visit their website at:

		http://www.boulder.nist.gov/timefreq/service/its.htm

	This program reads the current time from the NIST server as a
	one-line string in "Daytime Protocol" (RFC-867).  This format
	provides useful information like whether Daylight Saving Time is
	currently in effect and the health of the timer server. The
	string will appear something like this:

		52278 02-01-04 22:25:39 00 0 0 806.1 UTC(NIST) *

	Also available (on port 37) is "Time Protocol" (RFC-868),
	which simply provides a 32-bit value representing the number
	of seconds since January 1, 1970.

	Note that NIST actually recommends using the IP address, and
	not the domain names.  IP addresses of all current NIST time
	servers can be found at:

		http://www.boulder.nist.gov/timefreq/service/time-servers.html

*******************************************************************************/
#class auto

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

//uncomment one
//#define NIST_SERVER_IP "129.6.15.28" //time-a.nist.gov NIST, Gaithersburg, Maryland
#define NIST_SERVER_IP "129.6.15.29" //time-b.nist.gov NIST, Gaithersburg, Maryland
//#define NIST_SERVER_IP "132.163.4.101" //time-a.timefreq.bldrdoc.gov NIST, Boulder, Colorado
//#define NIST_SERVER_IP "132.163.4.102" //time-b.timefreq.bldrdoc.gov NIST, Boulder, Colorado
//#define NIST_SERVER_IP "132.163.4.103" //time-c.timefreq.bldrdoc.gov NIST, Boulder, Colorado
//#define NIST_SERVER_IP "128.138.140.44" //utcnist.colorado.edu University of Colorado, Boulder

#define NIST_PORT			13							//  Maryland, USA

#define TIMEZONE			-8						// -8 = Pacific Standard Time

/* NIST_TIMEOUT Specifies maximum number of seconds to wait for
 * response to arrive from NIST time server, before a time out
 * error is declared. Can be changed to any integer number of
 * seconds.*/
#define NIST_TIMEOUT		30

#memmap xmem
#use "dcrtcp.lib"

tcp_Socket s;

//////////////////////////////////////////////////////////////////////

void main()
{
	int status;
	char buffer[2048];
	longword ip;
	int i, dst, health;
	struct tm	t;
	unsigned long	longsec;


	// open connection to NIST server

	sock_init();
	// Wait for the interface to come up
	while (ifpending(IF_DEFAULT) == IF_COMING_UP) {
		tcp_tick(NULL);
	}

	ip=resolve(NIST_SERVER_IP);
	tcp_open(&s, 0, ip, NIST_PORT, NULL);
	sock_wait_established(&s, NIST_TIMEOUT, NULL, &status);
	sock_mode(&s, TCP_MODE_ASCII);


	// receive and process data -- the server will close the connection,
	// which will cause execution to continue at sock_err below.

	while (tcp_tick(&s)) {
		sock_wait_input(&s, NIST_TIMEOUT, NULL, &status);
		sock_gets(&s, buffer, 48);
	}

sock_err:
	if (status == -1) {
		printf("\nConnection timed out, exiting.\n");
		exit(1);
	}
	if (status != 1) {
		printf("\nUnknown sock_err (%d), exiting.\n", status);
		exit(1);
	}

	sock_close(&s);

	// Dynamic C doesn't have a sscanf function, so we do
	// it this way instead...
	t.tm_year = 100 + 10*(buffer[6]-'0') + (buffer[7]-'0');
	t.tm_mon  = 10*(buffer[9]-'0') + (buffer[10]-'0');
	t.tm_mday = 10*(buffer[12]-'0') + (buffer[13]-'0');
	t.tm_hour = 10*(buffer[15]-'0') + (buffer[16]-'0');
	t.tm_min  = 10*(buffer[18]-'0') + (buffer[19]-'0');
	t.tm_sec  = 10*(buffer[21]-'0') + (buffer[22]-'0');
	dst       = 10*(buffer[24]-'0') + (buffer[25]-'0');
	health    = buffer[27]-'0';

	// convert from tm_struct to seconds since Jan 1, 1980
	// (much easier to adjust for DST and timezone this way)

	longsec = mktime(&t);
	longsec += 3600ul * TIMEZONE;		// adjust for timezone
   dst = (dst >= 1 && dst <= 50 );
	if (dst)
		longsec += 3600ul;	// DST is in effect


	// convert back to tm struct for display to stdio
	mktm(&t, longsec);
	printf("Current time:  %02d:%02d:%02d  %02d/%02d/%04d\n",
				t.tm_hour, t.tm_min, t.tm_sec,
				t.tm_mon, t.tm_mday, 1900 + t.tm_year);
	if (dst)
		printf("Daylight Saving Time is in effect.\n");

	switch (health) {
		case 0:
			printf("Server is healthy.\n");
			break;
		case 1:
			printf("Server may be off by up to 5 seconds.\n");
			break;
		case 2:
			printf("Server is off by more than 5 seconds; not setting RTC.\n");
			break;
		default:
			printf("Server failure has occured; try another server (not setting RTC).\n");
			break;
	}

	// finally, set the RTC if the results seems good
	if (health < 2)
		write_rtc(longsec);

}