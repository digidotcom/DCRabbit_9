#define DISABLE_FS_WARNING

/************************************************************

	clear_param.c
	Z-World, 2001

	Resets the parameters of the RabbitLink board (stored on
	the second flash) to default values.

	To use this program, compile it to the RabbitLink board,
	run it, then reload the RabbitLink with the DOWNLOAD.BIN
	firmware image.

 ************************************************************/

#define TCPCONFIG 0
#define USE_ETHERNET 1

#define CONTROL_PORT		4244
#define CONSOLE_BAUD		57600			// default console baud rate
#define MY_IP_ADDRESS	"10.10.1.100"
#define MY_NETMASK		"255.255.255.0"
#define MY_GATEWAY		"10.10.1.1"
#define MY_NAME			"RabbitLink"
#define SERIAL_ECHO		1

#define FS_FLASH
#define FS_NUM_BLOCKS	64

#define SSPEC_MAXSPEC 60

#define BACKUP_FILE1		128

#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 5
#define MAX_UDP_SOCKET_BUFFERS 1
#define UDP_BUF_SIZE 512
#define DISABLE_DNS

#memmap xmem
#use "dcrtcp.lib"
#use "md5.lib"
#use "FileSystem.lib"
#use "http.lib"
#use "smtp.lib"

#define MAIL_FROM_SIZE 51
#define MAIL_SERV_SIZE 51
typedef struct {
	char		top; // used to mark the top of the struct
	char		name[40];
	char		passwd[16];
	uint16	control_port;
	uint32	console_baud;
	uint32	_my_ip_addr;
	uint32	_sin_mask;
	uint32 	gate_ip;
	char		echo;
	char		use_dhcp;
	char 		mail_from[MAIL_FROM_SIZE];
	char		mail_serv[MAIL_SERV_SIZE];
	uint16	varstrlen[SSPEC_MAXSPEC];
} DTPIdentification;

DTPIdentification main_id;
char NextBackupFile;
unsigned long BackupVersion;

void InitAllState(void);
void dtp_backup_id(void);
void dtp_hash(char *data, int length, char *digest);

/******************************************************************************/

long lookup_gateway(void)
{
	auto int i;
	auto ATHandle ath;

	/* find the default gateway */
	for (i = 0; i < ARP_ROUTER_TABLE_SIZE; i++) {
		// Fixme: need to make documented interface for this data
		ath = ATH2INDEX(_arp_gate_data[i].ath);
		if (ath > ARP_TABLE_SIZE)
			continue;

		// found it
		return _arp_data[ath].ip;
	}
}

void dtp_hash(char *data, int length, char *digest)
{
	auto md5_state_t	md5_state;

	md5_init(&md5_state);
	md5_append(&md5_state, (md5_byte_t *)data, length);
	md5_finish(&md5_state, (md5_byte_t *)digest);
}

/******************************************************************************/

/*
 * This should be called to backup our identification + IP into flash
 */
void dtp_backup_id(void)
{
	auto int offset, len, retval;
	auto unsigned long temp;
	auto File f, *F;
	F = &f;

	main_id.top = 0xa5;
	main_id._my_ip_addr = my_ip_addr;
	main_id._sin_mask = sin_mask;
	main_id.gate_ip = lookup_gateway();

	/* backup: main_id */
	printf("dtp_backup_id()...\n");
	if(fcreate(F, NextBackupFile)) {
		/* Error! File allready exists? */
		goto backup_error;
	}

	/* write out the new version number */
	BackupVersion++;
	printf("\tBackupVersion == %ld\n",BackupVersion);
	if(4 != fwrite(F, (char *)&BackupVersion, 4)) {
		goto backup_error;
	}

	/* write out the length of the config table */
	temp = sizeof(main_id);
	printf("\tsizeof(main_id) == %ld\n",temp);
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
	printf("\tsizeof(ServerSpec[]) == %ld\n",temp);
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
	if(255 == NextBackupFile) {
		fdelete(254);
		NextBackupFile = 254;
	} else {
		fdelete(255);
		NextBackupFile = 255;
	}

	return;

backup_error:
	fclose(F);

	/* should we format the fs here? */
	printf("ERROR: No room to backup config table in flash FS!\n");
	return;
}

/******************************************************************************/

void InitAllState(void)
{
	/* init everything to basic values */
	BackupVersion = 1;
	NextBackupFile = BACKUP_FILE1;
	main_id.control_port = CONTROL_PORT;
	main_id.console_baud = CONSOLE_BAUD;
	strcpy(main_id.name,MY_NAME);
	dtp_hash("",0,main_id.passwd);
	main_id.echo = SERIAL_ECHO;
	strcpy(main_id.mail_from, "nobody@nowhere.com");
	strcpy(main_id.mail_serv, "10.10.6.1");
	main_id.use_dhcp = 1;
}

/******************************************************************************/

main()
{
	sock_init();

	/* reset it all! */
	InitAllState();
	sspec_init();
	fs_format(0, FS_NUM_BLOCKS, 1);

	/* backup the reset data to flash */
	dtp_backup_id();

	printf("Parameters reset.\n");

	/* wait in an endless loop (instead of running over and over in run mode) */
	while (1)
		/* do nothing */;

	/* pull this in to let it compile properly */
	sock_init();
	tcp_tick(NULL);
}

