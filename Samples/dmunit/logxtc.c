/*
 * logxtc.c - demo DeviceMate with logging.
 *
 * If you run log100.c on the target processor, this demo will print
 * and log 100 messages from the target.  Messages alternate between
 * the filesystem (or XMEM) log and the stdio window.  You can press
 * enter at any time to review the current messages in the log.
 *
 * There are a lot of things you can configure...
 */

#define TC_I_AM_DEVMATE		// necessary for all DeviceMates

/*
 * Configure target communications
 */
//#define TARGETPROC_SERC
//#define XTC_DEBUG
//#define TARGETPROC_LOG_VERBOSE			// Print subsystem state changes
//#define XTC_VERBOSE						// Print XTC state transitions etc.
//#define XTC_PRINTPKT						// Dump packets transmitted or received
//#define XTC_PRINTPKT_COLOR_DARKBG		// Print using ANSI color escapes on a tube
//#define XTC_PRINTPKT_COLOR_LIGHTBG	//   ...with dark or light background.

/*
 * Configure the local log (log.lib, included by tc_conf.lib).
 */
#define LOG_USE_XMEM			// Either LOG_USE_FS2 or LOG_USE_XMEM

//#define LOG_VERBOSE						// Log debugging
#define  LOG_FS2_SIZE(strm)		1000	// Maximum file size (quota) for FS2 log
#define  LOG_XMEM_SIZE				500	// Memory quota for XMEM log
// Note: non-circular means that when the quota is exceeded, no more messages can
// be logged.  Otherwise (the default), the oldest message(s) are discarded to make
// room for new messages.
//#define  LOG_FS2_CIRCULAR(strm) 	0	// Define to make file non-circular
//#define  LOG_XMEM_CIRCULAR		 	0	// Define to make xmem buffer non-circular
#define FS2_USE_PROGRAM_FLASH 16			// Necessary if using program flash as filesystem
													// Make change to .\lib\bioslib\memconfig.lib e.g.
													//   #define XMEM_RESERVE_SIZE (16*1024L)
													// for this to work.

/*
 * Just used by this demo.
 */
//#define RESET_ALL_LOGS					// Define to reset all logs when started (only
													// applicable for FS2 log - xmem log is always
													// reset when demo is started).
//#define READ_BACKWARDS					// Define to print log most recent first, else
													// prints in order of addition.

/*
 * Choose LOG services.
 */
#define USE_TC_LOG
#use "tc_conf.lib"


int main()
{
	LogEntry loginfo;
	int status;
	char buf[200];


#ifdef LOG_USE_FS2
	#define DEST_IN_USE		LOG_DEST_FS2
	printf("Initializing file system.  Please wait...\n");
	status = fs_init(0,0);
	if (status) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}
	printf("...done.  Using LX#%d for logging\n", (int)LOG_FS2_DATALX(0));
#else
	#define DEST_IN_USE		LOG_DEST_XMEM
#endif

#ifdef RESET_ALL_LOGS
	log_open(LOG_DEST_ALL, 1);
#endif

	targetproc_init();

reprint:

#ifdef READ_BACKWARDS
	if (!log_seek(DEST_IN_USE, 1)) {
		printf("Scanning previous log entries, most recent first...\n");
		for (;;) {
			if (log_prev(DEST_IN_USE, &loginfo) < 0)
				break;
			printf("%s\n", log_format(&loginfo, buf, sizeof(buf), 1));
		}
		printf("End of messages.\n");
	}
#else
	if (!log_seek(DEST_IN_USE, 0)) {
		printf("Scanning previous log entries, oldest first...\n");
		for (;;) {
			if (log_next(DEST_IN_USE, &loginfo) < 0)
				break;
			printf("%s\n", log_format(&loginfo, buf, sizeof(buf), 1));
		}
		printf("End of messages.\n");
	}
#endif

#define LOG_TEST_STRING "~~~{ Started test run. }~~~"
	status = log_put(LOG_MAKEPRI(2,LOG_INFO), 0, LOG_TEST_STRING, strlen(LOG_TEST_STRING));
	if (status != 0)
		printf("Failed to add 1st message: %d\n", status);

	for (;;) {
		targetproc_tick();
		if (kbhit()) {
			gets(buf);	// ignore it
			goto reprint;
		}
	}

}