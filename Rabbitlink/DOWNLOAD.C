#define DISABLE_FS_WARNING

/********************************************************

	download.c
	2001, Z-World

	Firmware source code for the EG2100 RabbitLink.
	To use, compile to the RabbitLink, detatch the
	programming cable from the PROG IN port, and
	reset the RabbitLink board.

	1.03 - first release version
	1.04 - added background polling
	1.05 - upgraded for 7.05 BIOS changes
			 (will still work with older versions)
	2.00 - complete rework for DevMate style loading
	2.05 - firmware has been updated to be compatible with
	       Dynamic C 7.30
	2.10 - update for DC 8.5 DHCP changes

 ********************************************************/

/* default IP values */
#define TCPCONFIG 0		// No predefined
#define USE_ETHERNET 0x01
#define MY_IP_ADDRESS	"10.10.1.100"
#define MY_NETMASK		"255.255.255.0"
#define MY_GATEWAY		"10.10.1.1"
#define MY_NAME			"RabbitLink"

/* try and use DHCP by default, if USE_DHCP is turned on */
#define USE_DHCP

/* enable the discovery protocol? This will listen for broadcast requests on the LAN */
#define DISCOVERY

/* enable the serial console? This brings in the filesystem, web server, etc */
#define SERIAL_CONSOLE

/* use serial port C (instead of the default port A) for the console */
//#define SERIAL_PORT_C

/* enable the watchdog timer */
#define USE_WATCHDOG

/* password for the control (loader) connection [default] */
#define TARGETPROC_LOADER_PASSPHRASE	""

/* DHCP config */
#define DHCP_NUM_SMTP	1
#define DHCP_MINRETRY	5
#define DHCP_CLASS_ID "Rabbit-TCPIP:Z-World:RabbitLink:2.0.0"
#define DHCP_CLIENT_ID_MAC

/* timeout to retry DHCP requests, in seconds, at boot time */
#define DHCP_RETRY_TIMEOUT		30

/* debug messages - uncomment to have that class of debug messages */
//#define TARGETPROC_LOADER_DEBUG	// debugging of the loading procedure
//#define DBG_BACKUP		// backup of tables debugging

/* DHCP debugging */
//#define BOOTP_DEBUG		// Debug DHCP stuff
//#define BOOTP_VERBOSE		// Print lots of detail

/* flash file system */
#define FS_FLASH
//#define FS_DEBUG
//#define FS_DEBUG_FLASH

#define CONSOLE_VERSION "2.10"

#define SSPEC_MAXSPEC 60
#define SSPEC_XMEMVARLEN 128

/* Set the BLINK function on the loader, to have it control the UserLED on the RLINK board */
#define TARGETPROC_LOADER_BLINK	Blink();

/*
 * Stuff for the webserver & console
 */
#define CONSOLE_BAUD				57600			// default console baud rate
#ifdef SERIAL_PORT_C
	#define CINBUFSIZE			1023
	#define COUTBUFSIZE			255
#else
	#define AINBUFSIZE			1023
	#define AOUTBUFSIZE			255
#endif
#define SERIAL_ECHO			1

#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 5
#ifdef USE_DHCP
	#define MAX_UDP_SOCKET_BUFFERS 3
#else
	#define MAX_UDP_SOCKET_BUFFERS 1
#endif
#define UDP_BUF_SIZE 512
#define DISABLE_DNS

#define HTTP_NO_FLASHSPEC

/* Size of the variable buffer */
#define VAR_BUFFER 4096

/* Size of the filesystem, in 4k blocks */
#define FS_NUM_BLOCKS	64

/* File numbers for backup files */
#define BACKUP_FILE1		128
#define BACKUP_FILE2		129

#memmap xmem
#use "dcrtcp.lib"
#use "md5.lib"
#ifdef SERIAL_CONSOLE
#use "FileSystem.lib"
#use "http.lib"
#use "smtp.lib"
#endif

/* TCP port the loader uses */
#define TARGETPROC_LOADER_PORT		4244

/* bring in the loader */
#define USE_TC_LOADER
#define TC_I_AM_DEVMATE
#use "tc_conf.lib"

/* UDP port the discovery protocol uses */
#define DISCOVERY_PORT	4242

#define CONSOLE_TIMEOUT	60

/* watchdog timeout time. time in seconds == (WATCHDOG_TIMEOUT - 1)/16 */
#define WATCHDOG_TIMEOUT	161
#ifdef USE_WATCHDOG
	#define HIT_WATCHDOG			VdHitWd(wd);
#else
	#define HIT_WATCHDOG
#endif

/* Help file index structure */
struct HelpIndex {
	char* cmd1;
	char* cmd2;
	char* cmd3;
	long addr;
};

/* Help files */

/* grab the help files */
#ximport "RabbitLink/help/help.txt" help_txt
#ximport "RabbitLink/help/help_delete.txt" help_delete_txt
#ximport "RabbitLink/help/help_echo.txt" help_echo_txt
#ximport "RabbitLink/help/help_get.txt" help_get_txt
#ximport "RabbitLink/help/help_help.txt" help_help_txt
#ximport "RabbitLink/help/help_list.txt" help_list_txt
#ximport "RabbitLink/help/help_mail.txt" help_mail_txt
#ximport "RabbitLink/help/help_put.txt" help_put_txt
#ximport "RabbitLink/help/help_reset.txt" help_reset_txt
#ximport "RabbitLink/help/help_set.txt" help_set_txt
#ximport "RabbitLink/help/help_set_mail.txt" help_set_mail_txt
#ximport "RabbitLink/help/help_set_mail_from.txt" help_set_mail_from_txt
#ximport "RabbitLink/help/help_set_mail_server.txt" help_set_mail_server_txt
#ximport "RabbitLink/help/help_show.txt" help_show_txt
#ximport "RabbitLink/help/help_createv.txt" help_createv_txt
#ximport "RabbitLink/help/help_getv.txt" help_getv_txt
#ximport "RabbitLink/help/help_putv.txt" help_putv_txt

const struct HelpIndex helpindex[] =
{
	{ NULL, NULL, NULL, help_txt },
	{ "delete", NULL, NULL, help_delete_txt },
	{ "echo", NULL, NULL, help_echo_txt },
	{ "get", NULL, NULL, help_get_txt },
	{ "help", NULL, NULL, help_help_txt },
	{ "list", NULL, NULL, help_list_txt },
	{ "mail", NULL, NULL, help_mail_txt },
	{ "put", NULL, NULL, help_put_txt },
	{ "reset", NULL, NULL, help_reset_txt },
	{ "set", NULL, NULL, help_set_txt },
	{ "set", "mail", NULL, help_set_mail_txt },
	{ "set", "mail", "from", help_set_mail_from_txt },
	{ "set", "mail", "server", help_set_mail_server_txt },
	{ "show", NULL, NULL, help_show_txt },
	{ "createv", NULL, NULL, help_createv_txt },
	{ "putv", NULL, NULL, help_putv_txt },
	{ "getv", NULL, NULL, help_getv_txt }
};

/* Blink the light! */
#asm
;
; bc_usrledon
;


bc_usrledon::
			ld		a, (GOCRShadow)
        	or      a,0x02
        	and     a,0xfe
ioi     	ld      (GOCR),a
			ld		(GOCRShadow), a
        	ret


;
; bc_usrledoff
;

bc_usrledoff::
			ld		a, (GOCRShadow)
        	or      a,0x03
ioi     	ld      (GOCR),a
			ld		(GOCRShadow), a
        	ret
#endasm

nodebug void Blink(void)
{
	static char state;
#GLOBAL_INIT {
	state = 0;
	bc_usrledoff();
}
	if(state) {
		bc_usrledoff();
		state = 0;
	} else {
		bc_usrledon();
		state = 1;
	}
}

/* sleep for 'len' milliseconds */
void sleep(long len)
{
	auto long time;

	time = MS_TIMER;
	time += len;

	while(MS_TIMER < time)
		continue;
}

#define MAIL_FROM_SIZE 51
#define MAIL_SERV_SIZE 51
typedef struct {
	char		top; // used to mark the top of the struct
	char		name[40];
	char		passwd[16];
	uint16	control_port;
	uint32	console_baud;
	uint32	ip_addr;
	uint32	sinmask;
	uint32 	gate_ip;
	char		echo;
	char		use_dhcp;
	char 		mail_from[MAIL_FROM_SIZE];
	char		mail_serv[MAIL_SERV_SIZE];
	uint16	varstrlen[SSPEC_MAXSPEC];
} DTPIdentification;

/***************
 * Global vars *
 ***************/

/* What is our current status? */
int GlobalStatus;

DTPIdentification main_id;
char NextBackupFile;
unsigned long BackupVersion;
int wd; /* watchdog timer */

/*******************
 * End global vars *
 *******************/

/* global prototypes */
void InitAllState(void);
void dtp_backup_id(void);
void dtp_hash(char *data, int length, char *digest);

/*********************
 * Discovery section *
 *********************/

#ifdef DISCOVERY

/*
 * Packet types
 */
typedef struct {
	uint16	protocol;
	uint16	requesttype;
	char		name[40];
	char		data[40];
} DTPDiscovery;

#define DTPS_ERROR		0xff
#define DTPS_RUNNING		0x00
#define DTPS_PROGRAMMED	0x01
#define DTPS_WAITING		0x02

typedef struct {
	uint16	protocol;
	uint16	status;
	char		name[40];
	long		ip;
	char		mac[6];
	uint16	control_port;
	char		data[30];
} DTPDiscoveryResponse;

/*
 * Data
 */
udp_Socket	discovery_sock;
sock_type *discovery_s;

/*
 * Init the discovery code (open the socket/etc)
 */
void dtp_broadcastinit(void)
{
	discovery_s = (sock_type *)&discovery_sock;

	udp_open(&discovery_sock, DISCOVERY_PORT, -1, 0, NULL);
}

/*
 * close the discovery socket - call init again to turn it back on
 */
void dtp_broadcastclose(void)
{
	sock_close(discovery_s);
}

/*
 * Run the discovery code - NON-Blockable!
 */
void dtp_broadcasthandler(void)
{
	static long	ip;
	static word	port;
	static DTPDiscovery			rx_packet;
	static DTPDiscoveryResponse	tx_packet;
	static int retval;
	static char *ptr;
	static longword remip;
	static word remport;
	static char tempipbuf[16];
	static int used;
	static long time;

	retval = udp_recvfrom(&discovery_sock, (char *)&rx_packet,
	                      sizeof(DTPDiscovery), &remip, &remport);
	if(retval >= 0) {
		//	/*printf("Discovery connection from: %s:%d\n",
		//		inet_ntoa(tempipbuf, remip), remport
		//		);*/

		if((ntohs(rx_packet.protocol) != 0x0100) && (ntohs(rx_packet.requesttype) != 0x0001)) {
			tx_packet.protocol = 0x0100;
			tx_packet.status = DTPS_ERROR;
		} else {
			retval = targetproc_loader_getstatus();
			switch(retval) {
			case TARGETPROC_LOADER_STATUS_RUNNING:
				tx_packet.status = DTPS_RUNNING;
				break;
			case TARGETPROC_LOADER_STATUS_PROGRAMMED:
				tx_packet.status = DTPS_PROGRAMMED;
				break;
			case TARGETPROC_LOADER_STATUS_WAITING:
				tx_packet.status = DTPS_WAITING;
				break;
			case TARGETPROC_LOADER_STATUS_ERROR:
			default:
				tx_packet.status = DTPS_ERROR;
				break;
			}
		}

		strcpy(tx_packet.name, main_id.name);
		tx_packet.ip = htonl(my_ip_addr);

		memset(tx_packet.mac, 0, 6);
		ptr = tx_packet.mac;
		pd_getaddress(0,tx_packet.mac);
		memset(tx_packet.data, 0, 30);

		/* send the control port */
		tx_packet.control_port = targetproc_loader_getport();

		if(udp_sendto(&discovery_sock, (char *)&tx_packet, sizeof(DTPDiscoveryResponse),
		              remip, DISCOVERY_PORT+1) != sizeof(DTPDiscoveryResponse)) {
			printf("error in write\n");
		}

#if 0
		printf("Sent: name='%s', ip='%s', mac='%02x:%02x:%02x:%02x:%02x:%02x', port='%d'\n",
			tx_packet.name,
			inet_ntoa(tempipbuf,ntohl(tx_packet.ip)),
			tx_packet.mac[0],tx_packet.mac[1],tx_packet.mac[2],
			tx_packet.mac[3],tx_packet.mac[4],tx_packet.mac[5],
			tx_packet.control_port);
#endif
	}
}

#endif /*DISCOVERY*/

/*************************
 * End discovery section *
 *************************/

/**************************
 * Serial console section *
 **************************/

#ifdef SERIAL_CONSOLE

/* macros for which serial port we are using */
#ifdef SERIAL_PORT_C
	#define SERopen			serCopen
	#define SERclose			serCclose
	#define SERwrFlush		serCwrFlush
	#define SERrdFlush		serCrdFlush
	#define SERpeek			serCpeek
	#define SERputs			serCputs
	#define SERputc			serCputc
	#define SERrdUsed			serCrdUsed
	#define SERwrUsed			serCwrUsed
	#define SERread			serCread
	#define SERwrite			serCwrite
#else
	#define SERopen			serAopen
	#define SERclose			serAclose
	#define SERwrFlush		serAwrFlush
	#define SERrdFlush		serArdFlush
	#define SERpeek			serApeek
	#define SERputs			serAputs
	#define SERputc			serAputc
	#define SERrdUsed			serArdUsed
	#define SERwrUsed			serAwrUsed
	#define SERread			serAread
	#define SERwrite			serAwrite
#endif

/*
 * Global defines for the serial console
 */
#define SER_INIT 					0
#define SER_READY					1
#define SER_GETREQ 				2
#define SER_PARSECMD 			3
#define SER_ECHO					4
#define SER_PUT					5
#define SER_PUT_READING			6
#define SER_PUT_B1				7
#define SER_PUT_B2				8
#define SER_GET					9
#define SER_GET_WRITING			10
#define SER_GET_B1				11
#define SER_GET_B2				12
#define SER_LIST					13
#define SER_LIST_FILES			14
#define SER_LIST_VARIABLES		15
#define SER_SET					16
#define SER_SET_DHCP				17
#define SER_SET_IP				18
#define SER_SET_NETMASK			19
#define SER_SET_HOSTNAME		20
#define SER_SET_GATEWAY			21
#define SER_SET_PORT				22
#define SER_SET_MAIL				23
#define SER_SET_MAIL_FROM		24
#define SER_SET_MAIL_SERVER	25
#define SER_MAIL					26
#define SER_MAIL_READING		27
#define SER_MAIL_SENDING		28
#define SER_SHOW					29
#define SER_PASSWORD				30
#define SER_PASSWORD_GET		31
#define SER_PASSWORD_AGAIN		32
#define SER_PASSWORD_FINISH	33
#define SER_RESET					34
#define SER_RESET_ANSWER		35
#define SER_RESET_CHECK			36
#define SER_RESET_ALL			37
#define SER_RESET_VARIABLES	38
#define SER_RESET_FILES			39
#define SER_HELP					40
#define SER_HELP_WRITING		41
#define SER_HELP_FINISH			42
//#define SER_HELP_MAIL			41
#define SER_DELETE				43
#define SER_CONS_BAUD			44
#define SER_CREATEV				45
#define SER_PUTV					46
#define SER_GETV					47

#define SER_MAIN_TIMEOUT		60
#define SER_PUT_TIMEOUT			10

#define CMD_SIZE 101
#define BUFFER_SIZE 1024
#define PASSWORD_SIZE	256

typedef struct {
	char* bufferend;
	FileNumber filenum;
	File f;

	char command[CMD_SIZE];
	char buffer[BUFFER_SIZE];
	char passbuf[PASSWORD_SIZE];
	char mailfrom[MAIL_FROM_SIZE];
	char mailserv[MAIL_SERV_SIZE];
	int buffernum;
	char* cmdptr;
	char* bufptr;
	int numparams;

	int spec;
	long num;
	long total;
	int getsawcr;

	char name[SSPEC_MAXNAME];

	char* mail_to;
	char* mail_subject;
	char* mail_body;

	int state;
	int nextstate;
	int laststate;
	int substate;
	int sawcr;
	int sawesc;
	int echo;

	long timeout;

	long varbuf;
	long varbuflen;
} SerialState;

/* The main state structure */
static SerialState sstate;


const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".cgi", "", NULL},                     // cgi
   { ".gif", "image/gif", NULL},
   { ".jpg", "image/jpeg", NULL},
   { ".jpeg", "image/jpeg", NULL},
   { ".txt", "text/plain", NULL}
};

/*
 * Init everything
 */
void dtp_consoleinit(void)
{
	sstate.state = SER_INIT;
	tcp_reserveport(80);
	http_init();
}

/*
 * Helper functions
 */

void DISABLE_NET(void)
{
	/* kill any open TCP connection */
	targetproc_loader_reset();

#ifdef DISCOVERY
	/* turn off the discovery code */
	dtp_broadcastclose();
#endif
}

void ENABLE_NET(void)
{
#ifdef DISCOVERY
	/* turn the discovery code back on */
	dtp_broadcastinit();
#endif
}

void UseStaticIPSettings(void)
{
	auto char buf[16];

#ifdef USE_DHCP
	ifconfig(IF_ETH0, IFS_DHCP, 0, IFS_END);
#endif

	ifconfig(IF_ETH0, IFS_IPADDR, main_id.ip_addr,
	         IFS_NETMASK, main_id.sinmask, IFS_ROUTER_SET, main_id.gate_ip,
	         IFS_UP,
	         IFS_END);
}

void ser_ok(SerialState* state) {
	SERputs("OK\r\n");
	state->state = SER_READY;
}

#define SER_ERR_TIMEOUT			1
#define SER_ERR_BADCOMMAND		2
#define SER_ERR_BADPARAMETER	3
#define SER_ERR_NAMETOOLONG	4
#define SER_ERR_DUPLICATE		5
#define SER_ERR_BADFILESIZE	6
#define SER_ERR_SAVINGFILE		7
#define SER_ERR_READINGFILE	8
#define SER_ERR_FILENOTFOUND	9
#define SER_ERR_MSGTOOLONG		10
#define SER_ERR_SMTPERROR		11
#define SER_ERR_BADPASSPHRASE	12
#define SER_ERR_CANCELRESET	13
#define SER_ERR_BADVARTYPE		14
#define SER_ERR_BADVARVALUE	15
#define SER_ERR_NOVARSPACE		16
#define SER_ERR_VARNOTFOUND	17
#define SER_ERR_STRINGTOOLONG	18
#define SER_ERR_NOTAFILE		19
#define SER_ERR_NOTAVAR			20
#define SER_ERR_USINGDHCP		21

void ser_error(SerialState* state, int errnum) {
	auto char errstr[7];

	SERputs("ERROR ");
	sprintf(errstr, "%04d  ", errnum);
	SERputs(errstr);

	switch(errnum) {
	case SER_ERR_TIMEOUT:
		SERputs("Timed out.");
		break;

	case SER_ERR_BADCOMMAND:
		SERputs("Unknown command.");
		break;

	case SER_ERR_BADPARAMETER:
		SERputs("Bad or missing parameter.");
		break;

	case SER_ERR_NAMETOOLONG:
		SERputs("Filename too long.");
		break;

	case SER_ERR_DUPLICATE:
		SERputs("Duplicate object found.");
		break;

	case SER_ERR_BADFILESIZE:
		SERputs("Bad file size.");
		break;

	case SER_ERR_SAVINGFILE:
		SERputs("Error saving file.");
		break;

	case SER_ERR_READINGFILE:
		SERputs("Error reading file.");
		break;

	case SER_ERR_FILENOTFOUND:
		SERputs("File not found.");
		break;

	case SER_ERR_MSGTOOLONG:
		SERputs("Mail message too long.");
		break;

	case SER_ERR_SMTPERROR:
		SERputs("SMTP server error.");
		break;

	case SER_ERR_BADPASSPHRASE:
		SERputs("Passphrases do not match!");
		break;

	case SER_ERR_CANCELRESET:
		SERputs("Reset cancelled.");
		break;

	case SER_ERR_BADVARTYPE:
		SERputs("Bad variable type.");
		break;

	case SER_ERR_BADVARVALUE:
		SERputs("Bad variable value.");
		break;

	case SER_ERR_NOVARSPACE:
		SERputs("Out of variable space.");
		break;

	case SER_ERR_VARNOTFOUND:
		SERputs("Variable not found.");
		break;

	case SER_ERR_STRINGTOOLONG:
		SERputs("String too long.");
		break;

	case SER_ERR_NOTAFILE:
		SERputs("Not a file.");
		break;

	case SER_ERR_NOTAVAR:
		SERputs("Not a variable.");
		break;

#ifdef USE_DHCP
	case SER_ERR_USINGDHCP:
		SERputs("Using DHCP - disable that first. \"set dhcp off\"");
		break;
#endif

	default:
		SERputs("Unknown internal error!");
		break;
	}
	SERputs("\r\n");

	state->state = SER_READY;
}

int nullify(char* buffer) {
	auto int i;
	auto int looking;
	auto int quoted;
	auto int num;

	looking = 0;
	quoted = 0;
	num = 0;
	for (i = 0; buffer[i] != '\0'; i++) {
		if ((looking == 0) && (buffer[i] == ' ')) {
			buffer[i] = '\0';
		} else if ((looking == 0) && (buffer[i] != ' ')) {
			if (buffer[i] == '\"') {
				quoted = 1;
				buffer[i] = '\0';
			}
			looking = 1;
			num++;
		} else if ((looking == 1) && (quoted == 0) &&
		           ((buffer[i] == ' ') || (buffer[i] == '\"'))) {
		   if (buffer[i] == '\"') {
		   	// Quote in an unquoted param; start new param
		   	quoted = 1;
		   	looking = 1;
		   	num++;
		   } else {
		   	looking = 0;
		   }
			buffer[i] = '\0';
		} else if ((looking == 1) && (quoted == 1) && (buffer[i] == '\"')) {
			// Ending a quoted param
			looking = 0;
			quoted = 0;
			buffer[i] = '\0';
		}
		// Handle escaped characters
		if (buffer[i] == '\\') {
			strcpy(&buffer[i], &buffer[i+1]);
		}
	}
	return num;
}

char* get_param(char* buffer, int num) {
	auto int numnulls;
	auto int i;

	numnulls = 0;
	i = 0;
	// Skip past initial nulls
	while (buffer[i] == '\0') {
		i++;
	}
	// Skip to the right parameter
	while (numnulls < num) {
		if ((buffer[i] == '\0') && (buffer[i+1] != '\0')) {
			numnulls++;
		}
		i++;
	}
	return (&buffer[i]);
}

int match_names(char* name1, char* name2) {
	if ((name1 == NULL) && (name2 == NULL)) {
		return 1;
	} else if (strcmpi(name1, name2) == 0) {
		return 1;
	} else {
		return 0;
	}
}

long lookup_help(char* cmd1, char* cmd2, char* cmd3) {
	auto int i;
	for (i = 0; i < (sizeof(helpindex) / sizeof(struct HelpIndex)); i++) {
		if ((match_names(cmd1, helpindex[i].cmd1)) &&
		    (match_names(cmd2, helpindex[i].cmd2)) &&
		    (match_names(cmd3, helpindex[i].cmd3))) {
			return helpindex[i].addr;
		}
	}
	return -1;
}

/*
 * The main handler
 */
void dtp_consolehandler(void) {
	auto char data;
	auto long len;
	auto int bytes;
	auto char* name;
	auto char* ptr;
	static SerialState* state;
	auto unsigned long	fileSize;
	auto char s[20];
	auto int sspec;
	auto long location;
	auto int tempchar;
	auto int tempint;
	auto long templong;
	auto float tempfloat;
	auto char* tailptr;
	auto int retval;
	auto word varkind;
	auto long ip;

	#GLOBAL_INIT {
		state = &sstate;
	}

	if (state->state != state->laststate) {
		if ((state->state != SER_PUT_B1) && (state->state != SER_PUT_B2)) {
			// state changed; reset timeout
			state->laststate = state->state;
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
		}
	}

	if (chk_timeout(state->timeout)) {
		if (state->state <= SER_GETREQ) {
			// never mind--we're waiting for a command or initializing
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
		} else {
			// we timed out in one state for too long
			if (state->filenum != 0) {
				fclose(&state->f);
				fdelete(state->filenum);
			}
			ser_error(state, SER_ERR_TIMEOUT);
		}
	}

	if ((state->state == SER_GETREQ) || (state->state == SER_READY)) {
		http_handler();
	}
	switch (state->state) {
	case SER_INIT:
		state->varbuf = xalloc(VAR_BUFFER);
		state->varbuflen = 0;

		state->echo = main_id.echo;
		state->sawcr = 0;

		SERopen(main_id.console_baud);
		SERwrFlush();
		SERrdFlush();
		SERputs("RabbitLink Console Version ");
		SERputs(CONSOLE_VERSION);
		SERputs(" Ready\r\n");
		state->state = SER_READY;
		break;

	case SER_READY:
		state->command[0] = '\0';
		state->cmdptr = state->command;
		state->numparams = 0;
		state->sawesc = 0;
		state->state = SER_GETREQ;
		state->filenum = 0;
		break;

	case SER_GETREQ:
		if (SERrdUsed() > 0) {
			SERread(&data, 1, 0);
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
		} else {
			break;
		}

		if (state->sawesc > 0) {
			// Skip past escaped characters (starting with 1B, such as cursor
			// movement keys)
			state->sawesc--;
			break;
		} else if ((data == '\r') || ((data == '\n') && (state->sawcr == 0))) {
			if (data == '\r') {
				state->sawcr = 1;
			}
			if (state->echo) {
				SERputs("\r\n");
			}
			if ((state->cmdptr - state->command) < CMD_SIZE) {
				*(state->cmdptr) = '\0';
			} else {
				ser_error(state, SER_ERR_BADCOMMAND);
				break;
			}

			// find the command
			state->state = SER_PARSECMD;
			break;
		} else if ((data == '\n') && (state->sawcr == 1)) {
			state->sawcr = 0;
			break;
		} else if (state->sawcr == 1) {
			state->sawcr = 0;
		}
		// Handle the character we just got
		if (data == 0x08) {
			// Backspace character
			if (state->cmdptr > state->command) {
				state->cmdptr--;
				if (state->echo) {
					//SERwrite(&data, 1);
					SERputs("\x08 \x08");
				}
			}
		} else if (data == 0x1B) {
			// Escape character--skip past the next two characters
			state->sawesc = 2;
		} else if (data == 0x15) {
			// CTRL-U -- linefeed and clear the command buffer
			if (state->echo) {
				SERputs("\r\n");
			}
			state->cmdptr = state->command;
			state->command[0] = '\0';
		} else if (data >= 0x20) {
			if ((state->cmdptr - state->command) < CMD_SIZE) {
				*(state->cmdptr) = data;
			}
			state->cmdptr++;
			if (state->echo) {
				SERwrite(&data, 1);
			}
		}
		break;

	case SER_PARSECMD:
		state->numparams = nullify(state->command);
		if (state->numparams == 0) {
			state->state = SER_READY;
		} else if (strcmpi(get_param(state->command, 0), "echo") == 0) {
			state->state = SER_ECHO;
		} else if (strcmpi(get_param(state->command, 0), "put") == 0) {
			state->state = SER_PUT;
		} else if (strcmpi(get_param(state->command, 0), "get") == 0) {
			state->state = SER_GET;
		} else if (strcmpi(get_param(state->command, 0), "list") == 0) {
			state->state = SER_LIST;
		} else if (strcmpi(get_param(state->command, 0), "set") == 0) {
			state->state = SER_SET;
		} else if (strcmpi(get_param(state->command, 0), "mail") == 0) {
			state->state = SER_MAIL;
		} else if (strcmpi(get_param(state->command, 0), "show") == 0) {
			state->state = SER_SHOW;
		} else if (strcmpi(get_param(state->command, 0), "reset") == 0) {
			state->state = SER_RESET;
		} else if (strcmpi(get_param(state->command, 0), "help") == 0) {
			state->state = SER_HELP;
		} else if (strcmpi(get_param(state->command, 0), "delete") == 0) {
			state->state = SER_DELETE;
		} else if (strcmpi(get_param(state->command, 0), "createv") == 0) {
			state->state = SER_CREATEV;
		} else if (strcmpi(get_param(state->command, 0), "getv") == 0) {
			state->state = SER_GETV;
		} else if (strcmpi(get_param(state->command, 0), "putv") == 0) {
			state->state = SER_PUTV;
		} else {
			ser_error(state, SER_ERR_BADCOMMAND);
		}
		break;

	case SER_ECHO:
		if ((state->numparams == 2) &&
		    (strcmpi(get_param(state->command, 1), "on") == 0)) {
			ser_ok(state);
			state->echo = 1;
			dtp_backup_id();
		} else if ((state->numparams == 2) &&
		           (strcmpi(get_param(state->command, 1), "off") == 0)) {
			ser_ok(state);
			state->echo = 0;
			dtp_backup_id();
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		state->state = SER_READY;
		break;

	case SER_PUT:
		if ((state->numparams == 2) || (state->numparams == 3)) {
			if (strlen(get_param(state->command, 1)) < SSPEC_MAXNAME) {
				strcpy(state->name, get_param(state->command, 1));
			} else {
				// Not enough room to put the page name
				ser_error(state, SER_ERR_NAMETOOLONG);
				break;
			}
			if (sspec_findname(state->name, SERVER_HTTP) != -1) {
				ser_error(state, SER_ERR_DUPLICATE);
				break;
			}
			state->bufferend = state->buffer;
			if (state->numparams == 2) {
				if ((state->filenum = fcreate_unused(&state->f)) == 0) {
					ser_error(state, SER_ERR_SAVINGFILE);
					break;
				}
				state->state = SER_PUT_READING;
			} else {
				state->num = strtol(get_param(state->command, 2), &ptr, 10);
				if ((*ptr == '\0') && (state->num < LONG_MAX) && (state->num > LONG_MIN)) {
					if ((state->filenum = fcreate_unused(&state->f)) == 0) {
						ser_error(state, SER_ERR_SAVINGFILE);
						break;
					}
					state->total = 0;
					state->timeout = set_timeout(SER_PUT_TIMEOUT);
					state->state = SER_PUT_B1;
				} else {
					ser_error(state, SER_ERR_BADFILESIZE);
				}
			}
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		break;

	case SER_PUT_READING:
		if (SERrdUsed() > 0) {
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
			SERread(&data, 1, 0);
		} else {
			break;
		}
		if ((data == 0x04) || (data == 0x1A)) {
			// Done getting the page (CTRL-D or CTRL-Z)
			if ((state->bufferend - state->buffer) > 0) {
				if (fwrite(&state->f, state->buffer, state->bufferend - state->buffer) < (state->bufferend - state->buffer)) {
					fclose(&state->f);
					fdelete(state->filenum);
					ser_error(state, SER_ERR_SAVINGFILE);
					break;
				}
			}
			fclose(&state->f);
			if (sspec_addfsfile(state->name, state->filenum, SERVER_HTTP) >= 0) {
				state->filenum = 0;
				dtp_backup_id();
				ser_ok(state);
				break;
			} else {
				fdelete(state->filenum);
				state->filenum = 0;
				ser_error(state, SER_ERR_SAVINGFILE);
				break;
			}
		} else if (data == 0x03) {
			// Bail out (CTRL-C)
			fclose(&state->f);
			fdelete(state->filenum);
			state->filenum = 0;
			ser_ok(state);
			break;
		} else if ((data == '\r') || ((data == '\n') && (state->sawcr == 0))) {
			*(state->bufferend) = data;
			state->bufferend++;
			if (data == '\r') {
				state->sawcr = 1;
			}
			if (state->echo) {
				SERputs("\r\n");
			}
		} else if ((data == '\n') && (state->sawcr == 1)) {
			*(state->bufferend) = data;
			state->bufferend++;
			state->sawcr = 0;
		} else {
			*(state->bufferend) = data;
			state->bufferend++;
			if (state->echo) {
				SERwrite(&data, 1);
			}
		}
		if ((state->bufferend - state->buffer) == BUFFER_SIZE) {
			if (fwrite(&state->f, state->buffer, BUFFER_SIZE) < BUFFER_SIZE) {
				fclose(&state->f);
				fdelete(state->filenum);
				ser_error(state, SER_ERR_SAVINGFILE);
				break;
			}
		}
		break;

	case SER_PUT_B1:
		if (SERrdUsed() > 0) {
			state->timeout = set_timeout(SER_PUT_TIMEOUT);
			if ((state->sawcr == 1) && (SERpeek() == '\n')) {
				SERread(&data, 1, 0);
			}
			state->state = SER_PUT_B2;
		}
		break;

	case SER_PUT_B2:
		if (SERrdUsed() > 0) {
			state->timeout = set_timeout(SER_PUT_TIMEOUT);
			len = state->bufferend - state->buffer;
			state->buffernum = SERread(state->bufferend, (((BUFFER_SIZE - (int)len) < 256)?(BUFFER_SIZE - (int)len):256), 0);
			if ((len + state->buffernum) == BUFFER_SIZE) {
				if (fwrite(&state->f, state->buffer, BUFFER_SIZE) < BUFFER_SIZE) {
					fclose(&state->f);
					fdelete(state->filenum);
					ser_error(state, SER_ERR_SAVINGFILE);
					break;
				}
				state->bufferend = state->buffer;
			} else {
				state->bufferend += state->buffernum;
			}
			state->total += state->buffernum;

			if (state->total >= state->num) {
				if (state->bufferend > state->buffer) {
					if (fwrite(&state->f, state->buffer, state->bufferend - state->buffer) != (state->bufferend - state->buffer)) {
						fclose(&state->f);
						fdelete(state->filenum);
						ser_error(state, SER_ERR_SAVINGFILE);
						break;
					}
				}
				fclose(&state->f);
				if (sspec_addfsfile(state->name, state->filenum, SERVER_HTTP) >= 0) {
					state->filenum = 0;
					dtp_backup_id();
					ser_ok(state);
					break;
				} else {
					fdelete(state->filenum);
					state->filenum = 0;
					ser_error(state, SER_ERR_SAVINGFILE);
					break;
				}
			}
		}
		break;

	case SER_GET:
		if ((state->numparams == 2) || (state->numparams == 3)) {
			if (strlen(get_param(state->command, 1)) < SSPEC_MAXNAME) {
				strcpy(state->name, get_param(state->command, 1));
			} else {
				// Not enough room to put the page name
				ser_error(state, SER_ERR_NAMETOOLONG);
				break;
			}
			if ((state->spec = sspec_findname(state->name, SERVER_HTTP)) == -1) {
				// Didn't find the file
				ser_error(state, SER_ERR_READINGFILE);
				break;
			}
			if (sspec_gettype(state->spec) != SSPEC_FILE) {
				// Not a file
				ser_error(state, SER_ERR_NOTAFILE);
				break;
			}
			state->num = 0;
			state->getsawcr = 0;
			if (state->numparams == 2) {
				state->state = SER_GET_WRITING;
			} else {
				state->num = strtol(get_param(state->command, 2), &ptr, 10);
				if ((*ptr == '\0') && (state->num < LONG_MAX) && (state->num > LONG_MIN)) {
					state->total = 0;
					sprintf(state->buffer, "%ld\r\n", sspec_getlength(state->spec));
					SERputs("LENGTH ");
					SERputs(state->buffer);
					state->state = SER_GET_B1;
				} else {
					ser_error(state, SER_ERR_BADFILESIZE);
				}
			}
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		break;

	case SER_GET_WRITING:
		if (SERwrUsed() == 0) {
			if (!state->echo) {
				bytes = sspec_readfile(state->spec, state->buffer, state->num, 200);
				if (bytes <= 0) {
					ser_error(state, SER_ERR_READINGFILE);
					break;
				}
				state->timeout = set_timeout(SER_MAIN_TIMEOUT);
				bytes = SERwrite(state->buffer, bytes);
				state->num += bytes;
				if (state->num >= sspec_getlength(state->spec)) {
					ser_ok(state);
				}
			} else {
				if (sspec_readfile(state->spec, &data, state->num, 1) == 1) {
					if ((data == '\r') || ((data == '\n') && (state->getsawcr == 0))) {
						SERputs("\r\n");
						if (data == '\r') {
							state->getsawcr = 1;
						}
					} else if ((data == '\n') && (state->getsawcr == 1)) {
						state->getsawcr = 0;
					} else {
						SERwrite(&data, 1);
					}
					state->num++;
				} else {
					ser_error(state, SER_ERR_READINGFILE);
					break;
				}
				if (state->num == sspec_getlength(state->spec)) {
					ser_ok(state);
				}
			}
		}
		break;

	case SER_GET_B1:
		if (SERwrUsed() == 0) {
			if ((state->sawcr == 1) && (SERpeek() == '\n')) {
				SERread(&data, 1, 0);
			}
			state->num = strtol(get_param(state->command, 2), &ptr, 10);
			if ((*ptr == '\0') && (state->num < LONG_MAX) && (state->num > LONG_MIN)) {
				state->total = 0;
				state->state = SER_GET_B2;
			} else {
				ser_error(state, SER_ERR_BADFILESIZE);
			}
		}
		break;

	case SER_GET_B2:
		if (SERwrUsed() == 0) {
			bytes = sspec_readfile(state->spec, state->buffer, state->total, 200);
			if (bytes <= 0) {
				ser_error(state, SER_ERR_READINGFILE);
				break;
			}
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
			if ((state->total + bytes) <= state->num) {
				SERwrite(state->buffer, bytes);
			} else if (state->total < state->num) {
				SERwrite(state->buffer, (int)(state->num - state->total));
			} else {
				ser_ok(state);
				break;
			}
			state->total += bytes;
			if (state->total >= sspec_getlength(state->spec)) {
				ser_ok(state);
			}
		}
		break;

	case SER_LIST:
		if (state->numparams != 2) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		if (strcmpi(get_param(state->command, 1), "files") == 0) {
			state->num = 0;
			state->state = SER_LIST_FILES;
		} else if (strcmpi(get_param(state->command, 1), "variables") == 0) {
			state->num = 0;
			state->state = SER_LIST_VARIABLES;
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		break;

	case SER_LIST_FILES:
		if (SERwrUsed() == 0) {
			if ((state->num = sspec_findnextfile((int)state->num, SERVER_HTTP)) != -1) {
				name = sspec_getname((int)state->num);
				if ((name != NULL) && (sspec_gettype((int)state->num) == SSPEC_FILE)) {
					fileSize = sspec_getlength((int)state->num);
					sprintf(s, "%8ld  ", fileSize);
					SERputs(s);
					SERputs(name);
					SERputs("\r\n");
					state->timeout = set_timeout(SER_MAIN_TIMEOUT);
				}
				state->num++;
			} else {
				ser_ok(state);
			}
		}
		break;

	case SER_LIST_VARIABLES:
		if (SERwrUsed() == 0) {
			if ((state->num = sspec_findnextfile((int)state->num, SERVER_HTTP)) != -1) {
				name = sspec_getname((int)state->num);
				if ((name != NULL) && (sspec_gettype((int)state->num) == SSPEC_VARIABLE)) {
					SERputs(name);
					varkind = sspec_getvarkind((int)state->num);
					switch (varkind) {
					case INT8:
						SERputs(" INT8");
						break;
					case INT16:
						SERputs(" INT16");
						break;
					case INT32:
						SERputs(" INT32");
						break;
					case FLOAT32:
						SERputs(" FLOAT32");
						break;
					case PTR16:
						SERputs(" STRING ");
						sprintf(s, "%d", main_id.varstrlen[(int)state->num] - 1);
						SERputs(s);
						break;
					}
					SERputs("\r\n");
					state->timeout = set_timeout(SER_MAIN_TIMEOUT);
				}
				state->num++;
			} else {
				ser_ok(state);
			}
		}
		break;

	case SER_SET:
		if (state->numparams <= 1) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		if (strcmpi(get_param(state->command, 1), "ip") == 0) {
			state->state = SER_SET_IP;
#ifdef USE_DHCP
		} else if (strcmpi(get_param(state->command, 1), "dhcp") == 0) {
			state->state = SER_SET_DHCP;
#endif
		} else if (strcmpi(get_param(state->command, 1), "netmask") == 0) {
			state->state = SER_SET_NETMASK;
		} else if (strcmpi(get_param(state->command, 1), "hostname") == 0) {
			state->state = SER_SET_HOSTNAME;
		} else if (strcmpi(get_param(state->command, 1), "gateway") == 0) {
			state->state = SER_SET_GATEWAY;
		} else if (strcmpi(get_param(state->command, 1), "port") == 0) {
			state->state = SER_SET_PORT;
		} else if (strcmpi(get_param(state->command, 1), "mail") == 0) {
			state->state = SER_SET_MAIL;
		} else if (strcmpi(get_param(state->command, 1), "passphrase") == 0) {
			state->state = SER_PASSWORD;
		} else if (strcmpi(get_param(state->command, 1), "consbaud") == 0) {
			state->state = SER_CONS_BAUD;
		} else {
			ser_error(state, SER_ERR_BADCOMMAND);
		}
		break;

#ifdef USE_DHCP
	case SER_SET_DHCP:
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		DISABLE_NET();
		if(0 == strncmpi("on",get_param(state->command,2),2)) {
			/* they turned dhcp on */
			main_id.use_dhcp = 1;
			SERputs("DHCP is ON!\r\n");
		} else if(0 == strncmpi("off",get_param(state->command,2),3)) {
			/* they turned dhcp off */
			main_id.use_dhcp = 0;
			SERputs("DHCP is OFF!\r\n");
		} else {
			/* unknown command */
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		/* re-acquire the DHCP address if necessary */
		ifconfig(IF_ETH0, IFS_DOWN, IFS_DHCP, main_id.use_dhcp, IFS_UP, IFS_END);
		if (!main_id.use_dhcp) {
			UseStaticIPSettings();
		}
		dtp_backup_id();
		ENABLE_NET();
		ser_ok(state);
		break;
#endif

	case SER_SET_IP:
#ifdef USE_DHCP
		if(main_id.use_dhcp) {
			ser_error(state, SER_ERR_USINGDHCP);
			break;
		}
#endif
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		DISABLE_NET();
		ip = inet_addr(get_param(state->command, 2));
		main_id.ip_addr = ip;
		ifconfig(IF_ETH0, IFS_DOWN, IFS_IPADDR, ip, IFS_UP, IFS_END);
		SERputs("IP address set to: ");
		ifconfig(IF_ETH0, IFG_IPADDR, &ip, IFS_END);
		SERputs(inet_ntoa(state->buffer,ip));
		SERputs("\r\n");
		dtp_backup_id();
		ENABLE_NET();
		ser_ok(state);
		break;

	case SER_SET_NETMASK:
#ifdef USE_DHCP
		if(main_id.use_dhcp) {
			ser_error(state, SER_ERR_USINGDHCP);
			break;
		}
#endif
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		DISABLE_NET();
		ip = inet_addr(get_param(state->command, 2));
		main_id.sinmask = ip;
		ifconfig(IF_ETH0, IFS_DOWN, IFS_NETMASK, ip, IFS_UP, IFS_END);
		SERputs("Netmask set to: ");
		ifconfig(IF_ETH0, IFG_NETMASK, &ip, IFS_END);
		SERputs(inet_ntoa(state->buffer,ip));
		SERputs("\r\n");
		dtp_backup_id();
		ENABLE_NET();
		ser_ok(state);
		break;

	case SER_SET_HOSTNAME:
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		DISABLE_NET();
		strcpy(main_id.name,get_param(state->command,2));
		sethostname(main_id.name);
		SERputs("Hostname set to: ");
		SERputs(main_id.name);
		SERputs("\r\n");
		dtp_backup_id();
		ENABLE_NET();
		ser_ok(state);
		break;

	case SER_SET_GATEWAY:
#ifdef USE_DHCP
		if(main_id.use_dhcp) {
			ser_error(state, SER_ERR_USINGDHCP);
			break;
		}
#endif
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		ip = inet_addr(get_param(state->command, 2));
		main_id.gate_ip = ip;
		ifconfig(IF_ETH0, IFS_DOWN, IFS_ROUTER_SET, ip, IFS_UP, IFS_END);
		SERputs("Gateway set to: ");
		ifconfig(IF_ETH0, IFG_ROUTER_DEFAULT, &ip, IFS_END);
		SERputs(inet_ntoa(state->buffer, ip));
		SERputs("\r\n");
		dtp_backup_id();
		ser_ok(state);
		break;

	case SER_SET_PORT:
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		DISABLE_NET();
		main_id.control_port = atoi(get_param(state->command,2));
		sprintf(state->buffer,"Control port set to : %d\r\n",main_id.control_port);
		SERputs(state->buffer);
		targetproc_loader_setport(main_id.control_port);
		dtp_backup_id();
		ENABLE_NET();
		ser_ok(state);
		break;

	case SER_SET_MAIL:
		if (state->numparams != 4) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		if (strcmpi(get_param(state->command, 2), "from") == 0) {
			state->state = SER_SET_MAIL_FROM;
		} else if (strcmpi(get_param(state->command, 2), "server") == 0) {
			state->state = SER_SET_MAIL_SERVER;
		} else {
			ser_error(state, SER_ERR_BADCOMMAND);
		}
		break;

	case SER_SET_MAIL_FROM:
		if (strlen(get_param(state->command, 3)) < MAIL_FROM_SIZE) {
			strcpy(main_id.mail_from, get_param(state->command, 3));
			dtp_backup_id();
			ser_ok(state);
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		break;

	case SER_SET_MAIL_SERVER:
		if (strlen(get_param(state->command, 3)) < MAIL_SERV_SIZE) {
			if (smtp_setserver(get_param(state->command, 3)) == SMTP_OK) {
				strcpy(main_id.mail_serv, get_param(state->command, 3));
				dtp_backup_id();
				ser_ok(state);
			} else {
				ser_error(state, SER_ERR_BADPARAMETER);
			}
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
		}
		break;

	case SER_MAIL:
		if (state->numparams != 2) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		state->bufptr = state->buffer;
		state->state = SER_MAIL_READING;
		break;

	case SER_MAIL_READING:
		if (SERrdUsed() > 0) {
			SERread(&data, 1, 0);
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
		} else {
			break;
		}
		if ((data == 0x04) || (data == 0x1A)) {
			if ((state->bufptr - state->buffer) >= (BUFFER_SIZE - 1)) {
				ser_error(state, SER_ERR_MSGTOOLONG);
				break;
			}
			*state->bufptr = '\0';
			if ((ptr = strchr(state->buffer, '\r')) != NULL) {
				*ptr = '\0';
				if (((ptr + 1 - state->buffer) < BUFFER_SIZE) && (*(ptr+1) == '\n')) {
					state->mail_body = ptr + 2;
				} else {
					state->mail_body = ptr + 1;
				}
			} else if ((ptr = strchr(state->buffer, '\n')) != NULL) {
				*ptr = '\0';
				state->mail_body = ptr + 1;
			} else {
				// No newline, so only subject, with no body
				state->mail_body = state->bufptr;
			}
			smtp_sendmail(get_param(state->command, 1), main_id.mail_from,
			              state->buffer, state->mail_body);
			state->state = SER_MAIL_SENDING;
			break;
		} else if (data == 0x03) {
			// Bail out (CTRL-C)
			ser_ok(state);
			break;
		}
		if ((data == '\r') || ((data == '\n') && (state->sawcr == 0))) {
			if (data == '\r') {
				state->sawcr = 1;
				// Put \r\n into the message
				if ((state->bufptr - state->buffer) < BUFFER_SIZE) {
					*state->bufptr = data;
				}
				state->bufptr++;
				data = '\n';
			}
			if (state->echo) {
				SERputs("\r\n");
			}
		} else if ((data == '\n') && (state->sawcr == 1)) {
			state->sawcr = 0;
			break;
		} else {
			if (state->sawcr == 1) {
				state->sawcr = 0;
			}
			if (state->echo) {
				SERwrite(&data, 1);
			}
		}
		if ((state->bufptr - state->buffer) < BUFFER_SIZE) {
			*state->bufptr = data;
		}
		state->bufptr++;
		break;

	case SER_MAIL_SENDING:
		if (smtp_mailtick() == SMTP_PENDING) {
			break;
		} else if (smtp_status() == SMTP_SUCCESS) {
			ser_ok(state);
		} else {
			ser_error(state, SER_ERR_SMTPERROR);
		}
		break;

	case SER_SHOW:
		SERputs("Current configuration:");
#ifdef USE_DHCP
		SERputs("\r\n\tUse DHCP:\t");
		if(main_id.use_dhcp) {
			SERputs("YES");
		} else {
			SERputs("NO");
		}
#endif
		SERputs("\r\n\tIP address:\t");
		ifconfig(IF_ETH0, IFG_IPADDR, &ip, IFS_END);
		SERputs(inet_ntoa(state->buffer,ip));
		SERputs("\r\n\tNetmask:\t");
		ifconfig(IF_ETH0, IFG_NETMASK, &ip, IFS_END);
		SERputs(inet_ntoa(state->buffer,ip));
		SERputs("\r\n\tGateway:\t");
		ifconfig(IF_ETH0, IFG_ROUTER_DEFAULT, &ip, IFS_END);
		SERputs(inet_ntoa(state->buffer,ip));
		SERputs("\r\n\tHostname:\t");
		SERputs(main_id.name);
		SERputs("\r\n\tControl port:\t");
		sprintf(state->buffer,"%d",targetproc_loader_getport());
		SERputs(state->buffer);
		SERputs("\r\n\tConsole baud:\t");
		sprintf(state->buffer,"%ld",main_id.console_baud);
		SERputs(state->buffer);
		SERputs("\r\n\tMail from:\t");
		SERputs(main_id.mail_from);
		SERputs("\r\n\tMail server:\t");
		SERputs(main_id.mail_serv);
		SERputs("\r\n");
		ser_ok(state);
		break;

	case SER_PASSWORD:
		if (state->numparams != 2) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		SERputs("Enter new passphrase: ");

		state->bufptr = state->buffer;
		state->state = SER_PASSWORD_GET;
		state->nextstate = SER_PASSWORD_AGAIN;
		break;

	case SER_PASSWORD_GET:
		if (SERrdUsed() > 0) {
			SERread(&data, 1, 0);
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
		} else {
			break;
		}
		if(state->sawcr == 1) {
			state->sawcr = 0;

			if ('\n' == data) {
				/* nothing - drop it */
				data = '\0';
			} else {
				/* character is ok */
			}
		}
		if('\0' == data) {
			/* empty char - drop it */
			break;
		} else if(('\r' == data) || ('\n' == data)) {
			/* end of line reached */
			if('\r' == data) {
				state->sawcr = 1;
			}
			if((state->bufptr - state->buffer) < BUFFER_SIZE) {
				*state->bufptr = '\0';
			}
			state->buffer[BUFFER_SIZE - 1] = '\0'; /* make sure we end in null */

			/* state->buffer has the password at this point */
			state->state = state->nextstate;
		} else {
			/* normal character - store it */
			if ((state->bufptr - state->buffer) < BUFFER_SIZE) {
				*state->bufptr = data;
			}
			state->bufptr++;
		}
		break;

	case SER_PASSWORD_AGAIN:
		memcpy(state->passbuf,state->buffer,PASSWORD_SIZE);
		state->passbuf[PASSWORD_SIZE - 1] = '\0';

		SERputs("\r\nEnter passphrase again: ");

		state->bufptr = state->buffer;
		state->state = SER_PASSWORD_GET;
		state->nextstate = SER_PASSWORD_FINISH;
		break;

	case SER_PASSWORD_FINISH:
		if(0 == strncmp(state->passbuf,state->buffer,strlen(state->passbuf))) {
			dtp_hash(state->passbuf, strlen(state->passbuf), main_id.passwd);
			targetproc_loader_setpassword(state->passbuf);
			dtp_backup_id();
			SERputs("\r\nPassphrase set.\r\n");
			ser_ok(state);
		} else {
//			SERputs("\r\nPassphrases do not match!\r\n");
			ser_error(state,SER_ERR_BADPASSPHRASE);
		}
		break;

	case SER_CONS_BAUD:
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		main_id.console_baud = atol(get_param(state->command,2));
		sprintf(state->buffer,"Console baud being set to : %ld\r\n",main_id.console_baud);
		SERputs(state->buffer);
		dtp_backup_id();
		SERclose();
		SERopen(main_id.console_baud);
		SERwrFlush();
		SERrdFlush();
		ser_ok(state);
		break;

	case SER_RESET:
		if (state->numparams != 2) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		if (strcmpi(get_param(state->command, 1), "all") == 0) {
			state->nextstate = SER_RESET_ALL;
			SERputs("Are you sure you want to reset to factory defaults? (y/n): ");
		} else if (strcmpi(get_param(state->command, 1), "variables") == 0) {
			state->nextstate = SER_RESET_VARIABLES;
			SERputs("Are you sure you want to reset the variables? (y/n): ");
		} else if (strcmpi(get_param(state->command, 1), "files") == 0) {
			state->nextstate = SER_RESET_FILES;
			SERputs("Are you sure you want to reset the files? (y/n): ");
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}

		state->bufptr = state->buffer;
		state->state = SER_RESET_ANSWER;
		break;

	case SER_RESET_ANSWER:
		if (SERrdUsed() > 0) {
			SERread(&data, 1, 0);
			state->timeout = set_timeout(SER_MAIN_TIMEOUT);
		} else {
			break;
		}
		if(state->sawcr == 1) {
			state->sawcr = 0;

			if ('\n' == data) {
				/* nothing - drop it */
				data = '\0';
			} else {
				/* character is ok */
			}
		}
		if('\0' == data) {
			/* empty char - drop it */
			break;
		} else if(('\r' == data) || ('\n' == data)) {
			/* end of line reached */
			if('\r' == data) {
				state->sawcr = 1;
			}
			if((state->bufptr - state->buffer) < BUFFER_SIZE) {
				*state->bufptr = '\0';
			}
			state->buffer[BUFFER_SIZE - 1] = '\0'; /* make sure we end in null */

			state->state = SER_RESET_CHECK;
		} else {
			/* normal character - store it */
			if ((state->bufptr - state->buffer) < BUFFER_SIZE) {
				SERputc(data);
				*state->bufptr = data;
			}
			state->bufptr++;
		}
		break;

	case SER_RESET_CHECK:
		SERputs("\r\n");
		if(1 != strlen(state->buffer)) {
			ser_error(state, SER_ERR_BADPARAMETER);		// ??
			break;
		}
		if(('y' != state->buffer[0]) && ('Y' != state->buffer[0])) {
			ser_error(state, SER_ERR_CANCELRESET);
			break;
		}
		SERputs("reset!\r\n");
		state->state = state->nextstate;
		break;

	case SER_RESET_ALL:
		/* reset it all! */
		InitAllState();
		sspec_init();
		fs_format(0, FS_NUM_BLOCKS, 1);

		fs_reserve_blocks(((sizeof(main_id) + sizeof(server_spec) + sizeof(server_auth)) /
		                  ((int)FS_BLOCK_SIZE - FS_HEADER) + 1) * 2);

		/* backup the reset data to flash */
		dtp_backup_id();

		/* reset serial console baud rate */
		SERclose();
		SERopen(main_id.console_baud);
		SERwrFlush();
		SERrdFlush();

		ser_ok(state);
		break;

	case SER_RESET_VARIABLES:
		sspec = 0;
		state->varbuflen = 0;
		while ((sspec = sspec_findnextfile(sspec, SERVER_HTTP)) != -1) {
			if (sspec_gettype(sspec) == SSPEC_VARIABLE) {
				sspec_remove(sspec);
			}
			sspec++;
		}
		dtp_backup_id();
		ser_ok(state);
		break;

	case SER_RESET_FILES:
		sspec = 0;
		state->varbuflen = 0;
		while ((sspec = sspec_findnextfile(sspec, SERVER_HTTP)) != -1) {
			if (sspec_gettype(sspec) == SSPEC_FILE) {
				templong = sspec_getfileloc(sspec);
				fdelete((FileNumber)templong);
				sspec_remove(sspec);
			}
			sspec++;
		}
		dtp_backup_id();
		ser_ok(state);
		break;

	case SER_HELP:
		if (state->numparams == 1) {
			SERputs("RabbitLink Console Version ");
			SERputs(CONSOLE_VERSION);
			SERputs("\r\n");
			location = lookup_help(NULL, NULL, NULL);
		} else if (state->numparams == 2) {
			location = lookup_help(get_param(state->command, 1),
			                       NULL, NULL);
		} else if (state->numparams == 3) {
			location = lookup_help(get_param(state->command, 1),
			                       get_param(state->command, 2),
			                       NULL);
		} else if (state->numparams == 4) {
			location = lookup_help(get_param(state->command, 1),
			                       get_param(state->command, 2),
			                       get_param(state->command, 3));
		}
		if (location != -1) {
			xmem2root(&state->total, location, 4);
			xmem2root(state->buffer, location+4, (int)state->total);
			state->buffer[(int)state->total] = '\0';
			state->num = 0;
		} else {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		state->state = SER_HELP_WRITING;
		break;

	case SER_HELP_WRITING:
		if (state->num < state->total) {
			state->num += SERputs(state->buffer + (int)state->num);
		} else {
			state->state = SER_HELP_FINISH;
		}
		break;

	case SER_HELP_FINISH:
		if (SERwrUsed() == 0) {
			ser_ok(state);
		}
		break;

	case SER_DELETE:
		if (state->numparams != 2) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		sspec = sspec_findname(get_param(state->command, 1), SERVER_HTTP);
		if (sspec >= 0) {
			if (sspec_getfiletype(sspec) == SSPEC_FSFILE) {
				location = sspec_getfileloc(sspec);
				if (location == -1) {
					ser_error(state, SER_ERR_FILENOTFOUND);
					break;
				}
				fdelete((FileNumber)location);
				sspec_remove(sspec);
			} else {
				// Not a flash file
				ser_error(state, SER_ERR_FILENOTFOUND);
				break;
			}
		} else {
			// File not found
			ser_error(state, SER_ERR_FILENOTFOUND);
			break;
		}
		dtp_backup_id();
		ser_ok(state);
		break;

	case SER_CREATEV:
		if ((state->numparams != 5) && (state->numparams != 6)) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		if (sspec_findname(get_param(state->command, 1), SERVER_HTTP) != -1) {
			ser_error(state, SER_ERR_DUPLICATE);
			break;
		}
		// Get the type of variable
		ptr = get_param(state->command, 2);
		if (strcmpi(ptr, "INT16") == 0) {
			// Check number of parameters
			if (state->numparams != 5) {
				ser_error(state, SER_ERR_BADPARAMETER);
				break;
			}
			// Check for space in our variable buffer
			if ((state->varbuflen + 2) > VAR_BUFFER) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			}
			templong = strtol(get_param(state->command, 4), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong > INT_MAX) || (templong < INT_MIN)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			} else {
				tempint = (int)templong;
			}
			root2xmem(state->varbuf + state->varbuflen, &tempint, 2);
			retval = sspec_addxmemvar(get_param(state->command, 1),
			                          state->varbuf + state->varbuflen, INT16,
			                          get_param(state->command, 3), SERVER_HTTP);
			if (retval == -1) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			} else {
				state->varbuflen += 2;
			}
		} else if (strcmpi(ptr, "INT8") == 0) {
			// Check number of parameters
			if (state->numparams != 5) {
				ser_error(state, SER_ERR_BADPARAMETER);
				break;
			}
			// Check for space in our variable buffer
			if ((state->varbuflen + 1) > VAR_BUFFER) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			}
			templong = strtol(get_param(state->command, 4), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong > CHAR_MAX) || (templong < 0)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			} else {
				tempchar = (char)templong;
			}
			root2xmem(state->varbuf + state->varbuflen, &tempchar, 1);
			retval = sspec_addxmemvar(get_param(state->command, 1),
			                          state->varbuf + state->varbuflen, INT8,
			                          get_param(state->command, 3), SERVER_HTTP);
			if (retval == -1) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			} else {
				state->varbuflen += 1;
			}
		} else if (strcmpi(ptr, "INT32") == 0) {
			// Check number of parameters
			if (state->numparams != 5) {
				ser_error(state, SER_ERR_BADPARAMETER);
				break;
			}
			// Check for space in our variable buffer
			if ((state->varbuflen + 4) > VAR_BUFFER) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			}
			templong = strtol(get_param(state->command, 4), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong >= LONG_MAX) || (templong <= LONG_MIN)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			root2xmem(state->varbuf + state->varbuflen, &templong, 4);
			retval = sspec_addxmemvar(get_param(state->command, 1),
			                          state->varbuf + state->varbuflen, INT32,
			                          get_param(state->command, 3), SERVER_HTTP);
			if (retval == -1) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			} else {
				state->varbuflen += 4;
			}
		} else if (strcmpi(ptr, "FLOAT32") == 0) {
			// Check number of parameters
			if (state->numparams != 5) {
				ser_error(state, SER_ERR_BADPARAMETER);
				break;
			}
			// Check for space in our variable buffer
			if ((state->varbuflen + 4) > VAR_BUFFER) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			}
			tempfloat = strtod(get_param(state->command, 4), &tailptr);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			root2xmem(state->varbuf + state->varbuflen, &tempfloat, 4);
			retval = sspec_addxmemvar(get_param(state->command, 1),
			                          state->varbuf + state->varbuflen, FLOAT32,
			                          get_param(state->command, 3), SERVER_HTTP);
			if (retval == -1) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			} else {
				state->varbuflen += 4;
			}
		} else if (strcmpi(ptr, "STRING") == 0) {
			// Check number of parameters
			if (state->numparams != 6) {
				ser_error(state, SER_ERR_BADPARAMETER);
				break;
			}
			templong = strtol(get_param(state->command, 5), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong >= (LONG_MAX - 1)) || (templong <= 0)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			templong += 1;		// Add 1 for the null terminator
			if ((state->varbuflen + templong) > VAR_BUFFER) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			}
			if (strlen(get_param(state->command, 4)) > templong) {
				ser_error(state, SER_ERR_STRINGTOOLONG);
				break;
			}
			root2xmem(state->varbuf + state->varbuflen,
			          get_param(state->command, 4),
			          strlen(get_param(state->command, 4)) + 1);
			retval = sspec_addxmemvar(get_param(state->command, 1),
			                          state->varbuf + state->varbuflen,
			                          PTR16,
			                          get_param(state->command, 3), SERVER_HTTP);
			if (retval == -1) {
				ser_error(state, SER_ERR_NOVARSPACE);
				break;
			}
			main_id.varstrlen[retval] = (uint16)templong;
			state->varbuflen += templong;
		} else {
			ser_error(state, SER_ERR_BADVARTYPE);
			break;
		}
		dtp_backup_id();
		ser_ok(state);
		break;

	case SER_GETV:
		if (state->numparams != 2) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		retval = sspec_findname(get_param(state->command, 1), SERVER_HTTP);
		if (retval == -1) {
			ser_error(state, SER_ERR_VARNOTFOUND);
			break;
		}
		if (sspec_gettype(retval) != SSPEC_VARIABLE) {
			ser_error(state, SER_ERR_NOTAVAR);
			break;
		}
		sspec_readvariable(retval, state->buffer);
		SERputs(state->buffer);
		SERputs("\r\n");
		ser_ok(state);
		break;

	case SER_PUTV:
		if (state->numparams != 3) {
			ser_error(state, SER_ERR_BADPARAMETER);
			break;
		}
		sspec = sspec_findname(get_param(state->command, 1), SERVER_HTTP);
		if (sspec == -1) {
			ser_error(state, SER_ERR_VARNOTFOUND);
			break;
		}
		if (sspec_gettype(sspec) != SSPEC_VARIABLE) {
			ser_error(state, SER_ERR_VARNOTFOUND);
			break;
		}
		varkind = sspec_getvarkind(sspec);
		if (varkind == INT8) {
			templong = strtol(get_param(state->command, 2), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong > CHAR_MAX) || (templong < 0)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			} else {
				tempchar = (char)templong;
			}
			root2xmem(sspec_getxvaraddr(sspec), &tempchar, 1);
		} else if (varkind == INT16) {
			templong = strtol(get_param(state->command, 2), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong > INT_MAX) || (templong < INT_MIN)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			} else {
				tempint = (int)templong;
			}
			root2xmem(sspec_getxvaraddr(sspec), &tempint, 2);
		} else if (varkind == INT32) {
			templong = strtol(get_param(state->command, 2), &tailptr, 10);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			if ((templong >= LONG_MAX) || (templong <= LONG_MIN)) {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			root2xmem(sspec_getxvaraddr(sspec), &templong, 4);
		} else if (varkind == FLOAT32) {
			tempfloat = strtod(get_param(state->command, 2), &tailptr);
			if (*tailptr != '\0') {
				ser_error(state, SER_ERR_BADVARVALUE);
				break;
			}
			root2xmem(sspec_getxvaraddr(sspec), &tempfloat, 4);
		} else if (varkind == PTR16) {
			//xmem2root(&templong, sspec_getxvaraddr(sspec) - 4, 4);
			if ((strlen(get_param(state->command, 2)) + 1) > main_id.varstrlen[sspec]) {
				ser_error(state, SER_ERR_STRINGTOOLONG);
				break;
			}
			root2xmem(sspec_getxvaraddr(sspec),
			          get_param(state->command, 2),
			          strlen(get_param(state->command, 2)) + 1);
		} else {
			ser_error(state, SER_ERR_BADVARTYPE);
			break;
		}
		ser_ok(state);
		break;

	default:
		/* ERROR! */
		state->state = SER_INIT;
		break;
	}
}

#endif

/******************************
 * End serial console section *
 ******************************/

/**********************
 * Everything else... *
 **********************/

void dtp_hash(char *data, int length, char *digest)
{
	auto md5_state_t	md5_state;

	md5_init(&md5_state);
	md5_append(&md5_state, (md5_byte_t *)data, length);
	md5_finish(&md5_state, (md5_byte_t *)digest);
}

/*
 * This should be called to backup our identification + IP into flash
 */
#ifdef SERIAL_CONSOLE
void dtp_backup_id(void)
{
	auto int offset, len, retval;
	auto unsigned long temp;
	auto File f, *F;
	F = &f;

	main_id.top = 0xa5;

	/* backup: main_id */
#ifdef DBG_BACKUP
	printf("dtp_backup_id()...\n");
#endif
	if(fcreate(F, NextBackupFile)) {
		/* Error! File allready exists? */
		goto backup_error;
	}

	/* write out the new version number */
	BackupVersion++;
#ifdef DBG_BACKUP
	printf("\tBackupVersion == %ld\n",BackupVersion);
#endif
	if(4 != fwrite(F, (char *)&BackupVersion, 4)) {
		goto backup_error;
	}

	/* write out the length of the config table */
	temp = sizeof(main_id);
#ifdef DBG_BACKUP
	printf("\tsizeof(main_id) == %ld\n",temp);
#endif
	if(4 != fwrite(F, (char *)&temp, 4)) {
		goto backup_error;
	}

	/* write out the actual config table */
	offset = 0;
	len = sizeof(main_id);
	while(offset < len) {
		retval = fwrite(F, (char *)&(main_id.top) + offset, len - offset);
		if(retval < 1) {
			/* error - no room */
			goto backup_error;
		}
		offset += retval;
	}

	/* write out the length of the spec table */
	temp = SSPEC_MAXSPEC * sizeof(ServerSpec);
#ifdef DBG_BACKUP
	printf("\tsizeof(ServerSpec[]) == %ld\n",temp);
#endif
	if(4 != fwrite(F, (char *)&temp, 4)) {
		goto backup_error;
	}

	/* write the actual spec table */
	offset = 0;
	len = SSPEC_MAXSPEC * sizeof(ServerSpec);
	while(offset < len) {
		retval = fwrite(F, (char *)&(server_spec[0]) + offset, len - offset);
		if(retval < 1) {
			/* error - no room */
			goto backup_error;
		}
		offset += retval;
	}

	fclose(F);
	/* remove the old file, if it exists */
	if(BACKUP_FILE1 == NextBackupFile) {
		fdelete(BACKUP_FILE2);
		NextBackupFile = BACKUP_FILE2;
	} else {
		fdelete(BACKUP_FILE1);
		NextBackupFile = BACKUP_FILE1;
	}

	return;

backup_error:
	fclose(F);

	/* should we format the fs here? */
#ifdef DBG_BACKUP
	printf("ERROR: No room to backup config table in flash FS!\n");
#endif
	return;
}

/* load the tables out of the file, returning 0 on success, 1 on error */
int load_table(File *F)
{
	auto unsigned long config_len, spec_len;
	auto unsigned long offset;
	auto int retval;
	auto char buffer[32];

#ifdef DBG_BACKUP
	printf("Reading old config table...\n");
#endif
	/* read out the version number */
	if(4 != fread(F, (char *)&BackupVersion, 4)) {
		/* error */
		return 1;
	}
#ifdef DBG_BACKUP
	printf("\tBackupVersion == %ld\n",BackupVersion);
#endif

	/* read out the length of the main config region */
	if(4 != fread(F, (char *)&config_len, 4)) {
		/* error */
		return 1;
	}
#ifdef DBG_BACKUP
	printf("\tsizeof(main_id) == %ld\n",config_len);
#endif
	if(config_len != (unsigned long)sizeof(main_id)) {
#ifdef DBG_BACKUP
		printf("ERROR: Filesystem contains config struct of incorrect version!\n");
		printf("\tsizeof(main_id) == 0x%08lx\tconfig_len == 0x%08lx\n",(unsigned long)sizeof(main_id),config_len);
#endif
		return 1;
	}

	/* read out the main configuration */
	offset = 0;
	while(offset < config_len) {
		retval = fread(F, (char *)&(main_id.top) + (int)offset, (int)(config_len - offset));
		if(retval < 1) {
			/* not enough data in file! */
			return 1;
		}
		offset += retval;
	}

	/* read out the length of the spec table */
	if(4 != fread(F, (char *)&spec_len, 4)) {
		/* error */
		return 1;
	}
#ifdef DBG_BACKUP
	printf("\tsizeof(ServerSpec[]) == %ld\n",spec_len);
#endif
	if(spec_len != (SSPEC_MAXSPEC * sizeof(ServerSpec))) {
#ifdef DBG_BACKUP
		printf("ERROR: Server spec table is of incorrect size!\n");
#endif
		return 1;
	}

	/* read the spec table */
	offset = 0;
	while(offset < spec_len) {
		retval = fread(F, (char *)&(server_spec[0]) + (int)offset, (int)(spec_len - offset));
		if(retval < 1) {
			/* not enough data in file! */
			return 1;
		}
		offset += retval;
	}

	/* all done! fill in the loaded values to their real locations */
	smtp_setserver(main_id.mail_serv);
	targetproc_loader_setport(main_id.control_port);
	targetproc_loader_md5setpassword(main_id.passwd);
	return 0;
}

void load_tables(void)
{
	auto FileNumber fname1, fname2;
	auto File *F1, *F2;
	auto File f1, f2;
	auto unsigned long v1, v2;

	F1 = &f1;
	F2 = &f2;
	fname1 = BACKUP_FILE1;
	fname2 = BACKUP_FILE2;

	if(fopen_rd(F1, fname1)) {
		/* no file 1 */
		if(fopen_rd(F2, fname2)) {
			/* we have neither file! Error! format the fs! */
			fs_format(0, FS_NUM_BLOCKS, 1);

		} else {
			if(load_table(F2)) {
				goto format_fs;
			}
			NextBackupFile = BACKUP_FILE1;
			fclose(F2);

		}
	} else {
		if(fopen_rd(F2, fname2)) {
			/* have file 1, but no file 2 */
			if(load_table(F1)) {
				goto format_fs;
			}
			NextBackupFile = BACKUP_FILE2;
			fclose(F1);

		} else {
			/* both exist! check the version numbers */
			if(4 != fread(F1, (char *)&v1, 4)) {
				/* error in read! try the other one */
				if(load_table(F2)) {
					/* error in send file as well - format everything! */
					goto format_fs;
				}
				fclose(F1);
				fclose(F2);
				fdelete(fname1); /* kill the first file - it was incorrect */
				NextBackupFile = BACKUP_FILE1;
				return;
			}
			if(4 != fread(F2, (char *)&v2, 4)) {
				/* error in read! try the other one */
				if(load_table(F1)) {
					/* error in send file as well - format everything! */
					goto format_fs;
				}
				fclose(F1);
				fclose(F2);
				fdelete(fname2); /* kill the first file - it was incorrect */
				NextBackupFile = BACKUP_FILE2;
				return;
			}

			/* got two version numbers - compare them */
			if(v1 < v2) {
				/* v1 is older - keep it */
				fseek(F1,0,SEEK_SET);
				if(load_table(F1)) {
					goto format_fs;
				}
				fclose(F1);
				NextBackupFile = BACKUP_FILE2;

				/* delete the incorrect one */
				fclose(F2);
				fdelete(fname2);
			} else {
				/* v2 is older - keep it */
				fseek(F2,0,SEEK_SET);
				if(load_table(F2)) {
					goto format_fs;
				}
				fclose(F2);

				NextBackupFile = BACKUP_FILE1;

				/* delete the incorrect one */
				fclose(F1);
				fdelete(fname1);
			}
		}
	}
	/* all done */
	return;

format_fs:
	fclose(F1); /* make sure the files are closed */
	fclose(F2);
	fs_format(0, FS_NUM_BLOCKS, 1);
}
#endif /*SERIAL_CONSOLE*/

void InitAllState(void)
{
	/* init everything to basic values */
	BackupVersion = 1;
	NextBackupFile = BACKUP_FILE1;
	main_id.control_port = TARGETPROC_LOADER_PORT;
	targetproc_loader_setport(main_id.control_port);
	main_id.console_baud = CONSOLE_BAUD;
	strcpy(main_id.name,MY_NAME);
	sethostname(main_id.name);
	dtp_hash(TARGETPROC_LOADER_PASSPHRASE,strlen(TARGETPROC_LOADER_PASSPHRASE),main_id.passwd);
	targetproc_loader_setpassword(TARGETPROC_LOADER_PASSPHRASE);
#ifdef SERIAL_CONSOLE
	main_id.echo = SERIAL_ECHO;
	strcpy(main_id.mail_from, "nobody@nowhere.com");
	strcpy(main_id.mail_serv, "10.10.6.1");
	smtp_setserver(main_id.mail_serv);
#endif
#ifdef USE_DHCP
	main_id.use_dhcp = 1;
#else
	main_id.use_dhcp = 0;
#endif
}

main()
{
#ifdef USE_WATCHDOG
	/* init the watchdog timer */
	wd = VdGetFreeWd(WATCHDOG_TIMEOUT);
#endif

	/* make all state data sane */
	InitAllState();

	/* prepare the LED */
	bc_usrledon();

#ifdef SERIAL_CONSOLE
	/* load from flash main_id and my_ip_addr here */
	/* (opposite of dtp_backup_id() ) */

	/* do we have a filesystem in flash? if so, try to mount it */
	HIT_WATCHDOG;
	if(fs_init(0, FS_NUM_BLOCKS)) {
		/* problem initializing the filesystem - format it? */
	}

	HIT_WATCHDOG;
	sock_init();

	/* check for duplicate files for our ID stuff, and load the tables */
	HIT_WATCHDOG;
	load_tables();
	HIT_WATCHDOG;
	fs_reserve_blocks(((sizeof(main_id) + sizeof(server_spec) + sizeof(server_auth)) /
	                  ((int)FS_BLOCK_SIZE - FS_HEADER) + 1) * 2);
#endif

#ifdef USE_DHCP
	/* restore a static configuration? */
	if(!main_id.use_dhcp) {
		UseStaticIPSettings();
	}
	else {
		ifconfig(IF_ETH0, IFS_DHCP, 1, IFS_END);
	}
#else
	UseStaticIPSettings();
#endif

	/* init the main daemons */
	HIT_WATCHDOG;
#ifdef DISCOVERY
	dtp_broadcastinit();
#endif
#ifdef SERIAL_CONSOLE
	dtp_consoleinit();
#endif
	targetproc_init();

	/* run everything in a big loop */
	for(;;) {
#ifdef DISCOVERY
		HIT_WATCHDOG;
		dtp_broadcasthandler();
#endif
#ifdef SERIAL_CONSOLE
		HIT_WATCHDOG;
		dtp_consolehandler();
#endif
		HIT_WATCHDOG;
		targetproc_tick();
		HIT_WATCHDOG;
		tcp_tick(NULL);
	}
}