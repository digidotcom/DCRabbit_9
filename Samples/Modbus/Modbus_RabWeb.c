/* MBM_RABWEB.C 			Copyright January 2006, RabbitSemiconductor

This is a sample MODBUS master program using BL2600's.  For details on the
MODBUS functions used in the call-backs please see Modbus_Slave_BL26xx.lib.

This is a RabbitWeb program which requires a stand-alone MODBUS master device.
This device can be any Rabbit-based product.  The sample was built using an
RCM3700.  The slave devices are one or more BL2600s.  If more than one slave is
used the first one will need to ba a "gateway" device which can then send MODBUS
commands to downstream devices via RS485.  The remaining devices will be MODBUS
serial slaves.

This program is setup for two different types of devices, A & B, but for test
purposes the I/Os for both devices are setup for BL2600s.  So if a BL2600 and a
BL2100, or some other SBC, are to be used they would have to be different device
types.  For example A devices would be BL2600s and B devices BL2100s. So, you can
set the DEV_TYPE_B macro to the number of type B devices and also the macros below
it represent the hardware features for that device (BL2100s).

This sample does not use the tcp_config.lib for setting up network parameters.
All the network parameters are set locally in this program via the IFCONFIG_ETH0
macro.  The network parameter default settings are as follows:
	IP Address: 10.10.6.101  (IP Address of the device running this program)
   Gateway:		10.10.6.1    (by default this will be the 1st address in the net x.x.x.1)
   Netmask:		255.255.255.0

   **NOTE - If you do not have the RabbitWeb module you can use the Set_Ip.c
   			program to set a different IP address in the Userblock.  If there is
            a valid IP Address in the userblock this program will use that IP.
*/
//************************* MACROS *********************************************
//#define HTTP_VERBOSE
//#define HTTP_VERBOSE
//#define HTTP_DEBUG
// foreground colors for printf
#define	BLACK		"\x1b[30m"
#define	RED		"\x1b[31m"
#define	GREEN		"\x1b[32m"
#define	BLUE		"\x1b[34m"

// debug printout values
#define MODBUS_DEBUG_PRINT 0x0000	// set to 0 for no debug printout
												// 1 = DIO config
	                              	// 2 = DIO write
	                              	// 4 = HCO config
	                              	// 8 = HCO write
	                              	// 10 = DAC
	                              	// 20 = A/D
	                              	// 800 = MODBUS packet

#define TCPCONFIG 		0						// do not use tcp_config.lib
#define USE_ETHERNET		1
#define IFCONFIG_ETH0   IFS_DOWN				// 1st check userblock then bring up interface
#define MBAPSIZE	6
#define DEFAULT_IP		"10.10.6.101"     // default if ip is not in userblock
#define MBM_SLAVE_IP		"10.10.6.102"		// Modbus Gateway slave device IP Address

// The following three definitions are required for proper operation
// of the TCP/IP keepalive function.  If they are not defined here
// default values will be used.
#define	INACTIVE_PERIOD		5	//  period of inactivity, in seconds, before sending a
											//		keepalive, or 0 to turn off keepalives.
#define	KEEPALIVE_WAITTIME  	3	// number of seconds to wait between keepalives,
			  						  		//		after the 1st keepalive was sent
#define	KEEPALIVE_NUMRETRYS	3	// number of retrys

// TCP/IP Optimizing macros for more info see the TCP/IP Users Mnual vol. 1 chapter 4
#define HTTP_MAXBUFFER	(ETH_MTU-40)*2       // increased http buffer for optimal performance
#define TCP_BUF_SIZE		(HTTP_MAXBUFFER*6)   // increased socket buffer for optimal performance
#define HTTP_MAXSERVERS 		 2             // maximum number of http servers
#define MAX_TCP_SOCKET_BUFFERS 4

// RabbitWeb specific macros for more info see the RabbitWeb manual
#define USE_RABBITWEB			1              // bring in RabbitWeb
#define RWEB_ZHTML_MAXBLOCKS 	8              // default = 4
#define RWEB_POST_MAXBUFFER   HTTP_MAXBUFFER // default = 2048
#define RWEB_POST_MAXVARS		128				// default = 64, *20 = total root needed
#define SSPEC_MAXNAME  			32					// default = 20, max len of mime type

#ximport "\pages\type_a1.zhtml"	type_a1_zhtml
#ximport "\pages\type_a2.zhtml"	type_a2_zhtml
#ximport "\pages\type_b1.zhtml"	type_b1_zhtml
#ximport "\pages\type_b2.zhtml"	type_b2_zhtml
#ximport "\pages\functions.js"	functions_js
#ximport "\pages\tabs.css"		   tabs_css
#ximport "\pages\basic.css"		basic_css

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"
#define MODBUS_DEBUG debug
#use Modbus_Master.lib

/* the default for / must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml","text/html",zhtml_handler),
 	SSPEC_MIME(".js","application/x-javascript"),	// will need to change SSPEC_MAXNAME
   SSPEC_MIME(".css","text/css")
SSPEC_MIMETABLE_END

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/",type_a1_zhtml),
   SSPEC_RESOURCE_XMEMFILE("type_a1.zhtml",type_a1_zhtml),
   SSPEC_RESOURCE_XMEMFILE("type_a2.zhtml",type_a2_zhtml),
   SSPEC_RESOURCE_XMEMFILE("type_b1.zhtml",type_b1_zhtml),
   SSPEC_RESOURCE_XMEMFILE("type_b2.zhtml",type_b2_zhtml),
   SSPEC_RESOURCE_XMEMFILE("functions.js",functions_js),
  	SSPEC_RESOURCE_XMEMFILE("tabs.css",tabs_css),
  	SSPEC_RESOURCE_XMEMFILE("basic.css",basic_css)
SSPEC_RESOURCETABLE_END

// **************** GLOBAL STRUCTURES & VARIABLES ******************************

typedef struct {
	unsigned int cfg;    	// checkbox
   unsigned int val;       // checkbox
}ChBox;           // pair of checkboxes

typedef struct {
	char val[9];	// 8 char textbox
   int  unt;		// select menu
}TxtSt;

// These macros reflect the hardware features for Deice type A
#define  DEV_TYPE_A	2		// NUMBER OF TYPE A DEVICES
#define  DEVADIO		16		// Device A max Digital I/Os
#define  DEVAHCO		4		// Device A max High Current outputs
#define  DEVADAC		4     // Device A max Analog outputs
#define  DEVAADC		8     // Device A max Analog inputs

typedef struct {
   ChBox diox;
   int   dinx;
   ChBox hcox;
   TxtSt dac[DEVADAC];  	// text value; selectmenu unit options
   int adc_cfg[DEVAADC/2];	// number of configurable ADC's
   TxtSt adc[DEVAADC];		// text value; selectmenu unit options
}DevTypeA;

// These macros reflect the hardware features for Deice type B
#define  DEV_TYPE_B	2		// NUMBER OF TYPE B DEVICES
#define  DEVBDIO		16	   // Device B max Digital I/Os
#define  DEVBHCO		4		// Device B max High Current outputs
#define  DEVBDAC		4     // Device B max Analog outputs
#define  DEVBADC		8     // Device B max Analog inputs

struct {
   ChBox diox;
   int   dinx;
   ChBox hcox;
   TxtSt dac[DEVBDAC];  	// text value; selectmenu unit options
   int adc_cfg[DEVBADC/2];	// number of configurable ADC's
   TxtSt adc[DEVBADC];		// text value; selectmenu unit options
}DevTypeB;

#define 	MAXDEVICES  (DEV_TYPE_A+DEV_TYPE_B)	//total number of devices

struct {
   int  tabs;					// number of devices represented by tabs
   char status[128];			// used to display errors in the webpage
   char gw_ip[16];			// gateway's ip address
	int  update;	// hidden update variable to trigger updates when no changes in inputs
	struct {
   	char val[4];			// 4 char text box for device address
   }addr[MAXDEVICES];
	DevTypeA ta[DEV_TYPE_A]; 			// 2 type A devices
   DevTypeB tb[DEV_TYPE_B];  			// 2 type B devices
}dev;

typedef struct {
	int state;
   tcp_Socket sock;
	longword rip;
   word lport,rport;
   int rcvd,send,sent;
   char * packet;
}  _tcp_type;

#define TCP_INIT		0
#define TCP_ESTB   	1
#define TCP_SEND   	2
#define TCP_RECV   	3
#define TCP_CLSE   	4

#define TCP_BUSY				 0
#define TCP_CONN_ERROR		-1
#define TCP_SEND_ERROR		-2
#define TCP_RECV_ERROR		-3

// *********** (END) MODBUS TCP STRUCTS, GLOBALS, MACROS ************************

// **************** FUNCTION DECLARATIONS **************************************
void device_init();			// inititalze the device structures
void update_cb();				// tab callback function
void dev_dio_cfg_cb();  	// Digital I/O config checkbox callback function
void dev_dio_val_cb();  	// Digital I/O value checkbox callback function
void dev_hco_cfg_cb();		// High Current Output config checkbox callback function
void dev_hco_val_cb();		// High Current Output value checkbox callback function
void dev_dac_val_cb();
void dev_adc_cb();
int  isaddr_guard(char *);

// **************** REGISTERING ALL VARIABLES USED WITH RABBITWEB **************
#web dev.addr[@].val		((atoi($dev.addr[@].val) >= 1)?1:WEB_ERROR("*Must be greater than 0"))
#web dev.addr[@].val		((atoi($dev.addr[@].val) <= 250)?1:WEB_ERROR("*Must be less than 251"))
// modbus device A
#web dev.ta[@].diox.cfg
#web dev.ta[@].diox.val
#web dev.ta[@].dinx
#web dev.ta[@].hcox.cfg
#web dev.ta[@].hcox.val
#web dev.ta[@].dac[@].val
#web dev.ta[@].dac[@].unt	select("Raw"=0,"mVolts","Volts")
#web dev.ta[@].adc_cfg[@]	select("Single 0-20V"=0,"Single +-10","Diff. +-20V")
#web dev.ta[@].adc[@].val
#web dev.ta[@].adc[@].unt	select("Raw"=0,"mVolts","Volts")
// modbus devices B
#web dev.tb[@].diox.cfg
#web dev.tb[@].diox.val
#web dev.tb[@].dinx
#web dev.tb[@].hcox.cfg
#web dev.tb[@].hcox.val
#web dev.tb[@].dac[@].val
#web dev.tb[@].dac[@].unt	select("Raw"=0,"mVolts","Volts")
#web dev.tb[@].adc_cfg[@]	select("Single 0-20V"=0,"Single +-10","Diff. +-20V")
#web dev.tb[@].adc[@].val
#web dev.tb[@].adc[@].unt	select("Raw"=0,"Volts","mVolts")
// do tab last
#web dev.tabs	select("Gateway BL2600(A1)"=0,"Slave BL2600(A2)","Slave BL2600(B1)","Slave BL2600(B2)")
#web dev.status
#web dev.gw_ip          (isaddr_guard($dev.gw_ip)?1:WEB_ERROR("INVALID IP!"))
#web dev.update

// **************** REGISTERING RABBITWEB VARIABLES WITH CALLBACKS *************
#web_update dev.ta[@].diox.cfg,		dev.tb[@].diox.cfg		dev_dio_cfg_cb
#web_update dev.ta[@].diox.val,  	dev.tb[@].diox.val		dev_dio_val_cb
#web_update dev.ta[@].hcox.cfg,		dev.tb[@].hcox.cfg		dev_hco_cfg_cb
#web_update dev.ta[@].hcox.val,		dev.tb[@].hcox.val		dev_hco_val_cb

#web_update	dev.ta[@].dac[@].val,  	dev.ta[@].dac[@].unt,	\
				dev.tb[@].dac[@].val,	dev.tb[@].dac[@].unt		dev_dac_val_cb

#web_update	dev.ta[@].adc_cfg[@], 	dev.tb[@].adc_cfg[@]		dev_adc_cb

#web_update	dev.tabs,dev.gw_ip,dev.update,dev.addr[@].val	update_cb

int in_dev_dac_val[4];	// needed to prevent thrashing
int adc_config_flag[4];
int dac_config_flag[4];

// **************** START MAIN FUNCTION ****************************************
void main()
{
   long my_ip;
   char buff[16];
   readUserBlock(&my_ip,0,4);
   if( !isaddr(inet_ntoa(buff,my_ip))  || my_ip == -1L )
      my_ip = aton(DEFAULT_IP);

   sock_init();
   ifconfig(IF_ETH0,IFS_IPADDR,my_ip,
   			IFS_ROUTER_SET,(my_ip&aton("255.255.255.0"))+1,
            IFS_NETMASK,aton("255.255.255.0"),
				IFS_UP,IFS_END);
   while (ifpending(IF_DEFAULT) == IF_COMING_UP)
		tcp_tick(NULL);
   ip_print_ifs();
	http_init();
   tcp_reserveport(80);
   device_init();

   while (1) {
		http_handler();
	}
}

// **************** BEGIN SUBFUNCTIONS *****************************************
int	transaction;

/*** BeginHeader _initMBMpacket */
void _initMBMpacket ( int bytecount );
/*** EndHeader */

/*
MBAP Header ( MODBUS Application Protocol (MBAP) Header ) is 7 bytes:
	0,1 = Transaction Identifier
   			Used for transaction pairing, returned by target so host knows
            which transaction is being responded to.
   2,3 = Protocol Identifier = 0 for MODBUS
   4,5 = Length: number of following bytes - starting with Unit Identifier
   6   = Unit Identifier: remote slave identification
   			Used when the message is for another target.
            If value = 0xFF or wMSAddr then message is for this unit,
            if value = 0 then message is broadcast and is NOT supported,
            otherwise, message is for 'downstream" device
*/
// When uncommented, the following pair of "#pragma nowarn warns start" and
// "#pragma nowarn end" directives quiet a compiler warning about user code
// overriding the modbus_master.lib's _initMBMpacket() stub function.
//#pragma nowarn warns start
void _initMBMpacket ( int bytecount )
{
	#GLOBAL_INIT { transaction = 0; }
	_insertWord ( ++transaction );		// next transaction nbr
	_insertWord (0);							// MODBUS ID = 0
	_insertWord ( bytecount+1 );			// byte count - include Unit Identifier
	return;
}
//#pragma nowarn end


/* START FUNCTION DESCRIPTION ********************************************
                                                  <.LIB>

SYNTAX:			int client_handler( _tcp_type *client);

DESCRIPTION:	Establish a TCP connection and send (client->send) bytes from
					client->buff.  Then wait for client->rcvd bytes to be > 0.  Then
               return the number of bytes received in the client->buff.  Then if
               more packets need to be sent fill the client->buff, set
               client->send to lenght of packet and set client->state to  TCP_SEND,
               then call this function again.

PARAMETER:

RETURN VALUE:  (>0)	bytes received
					( 0)	TCP_BUSY
               (-1)	TCP_CONN_ERROR
               (-2)	TCP_SEND_ERROR
               (-3)	TCP_RECV_ERROR

END DESCRIPTION **********************************************************/

int client_handler( _tcp_type *client)
{  static long timeout;
	if( client->state != TCP_INIT  &&  tcp_tick(&client->sock) == 0 ) client->state = TCP_INIT;

	switch(client->state)
   {
   	case TCP_INIT:	//open a tcp connection to a modbus slave
         if(!tcp_extopen(&client->sock, IF_ANY, client->lport, client->rip, client->rport, NULL,0,0))
         	return TCP_CONN_ERROR;  	// failed to resolve remote ip (rip)
         tcp_set_nonagle(&client->sock);
         tcp_keepalive(&client->sock, INACTIVE_PERIOD); // enable keepalives.
         client->state = TCP_ESTB;
         timeout=MS_TIMER+2000L;
      break;

      case TCP_ESTB:	//check for an established connection
         if (sock_established(&client->sock)||sock_bytesready(&client->sock)>=0)
			{
         	client->state = TCP_SEND;
			}
         else if ((long)(MS_TIMER-timeout) > 0L)// if X msec and still waiting
			{
				return TCP_CONN_ERROR;
			}
      break;

      case TCP_SEND:   //send data to modbus slave
      	client->sent = sock_fastwrite ( &client->sock, client->packet, client->send );
        	if ( client->sent == -1 )
			{
				return TCP_SEND_ERROR;
			}
        	client->state = TCP_RECV;
         timeout=MS_TIMER+2000L;
      break;

      case TCP_RECV:	//check for data
       	client->rcvd = sock_fastread ( &client->sock, client->packet, 255 );
         if ( client->rcvd == -1 )
			{
				return TCP_RECV_ERROR;
			}
         if ( client->rcvd > 0 )
			{ 	client->state = TCP_SEND;
         	return client->rcvd;    // return the number of bytes received
         }
			else if ((long)(MS_TIMER-timeout) > 0L)// if X msec and still waiting
			{
				return TCP_CONN_ERROR;
			}
      break;

      default:
      	// waiting for close to finish....
   }
   return 0;
} // client_handler

/* START FUNCTION DESCRIPTION ********************************************
MBM_Send_ADU

SYNTAX:			int MBM_Send_Packet ( int ByteCount );

DESCRIPTION:	send the packet to the MODBUS target

PARAMETER1:		Address of ADU

PARAMETER2:		byte count of packet

RETURN VALUE:

END DESCRIPTION **********************************************************/

_tcp_type mbm;

int MBM_Send_ADU ( char * ADUaddress, int ByteCount )
{ 	auto int retval,i,j;
   #GLOBAL_INIT{	mbm.state = TCP_INIT; mbm.lport = 0; mbm.rport = 502; }
   mbm.rip = inet_addr ( dev.gw_ip );

   mbm.packet = ADUaddress;				// save packet address
#if MODBUS_DEBUG_PRINT & 0x800
	printf ( "\n\rTCP Tx:" );
	for ( i=0; i<ByteCount; i++ ) printf ( " %02X", mbm.packet[i] );
	printf ( "\n\r" );
#endif
#if MODBUS_DEBUG_PRINT & 0x400
	i = mbm.packet[8];
   i = (i<<8) + (int)mbm.packet[9];
	j = mbm.packet[10];
   j = (j<<8) + (int)mbm.packet[11];
	printf ( "TCP Tx: Addr=%d Function=%2.2X Reg=%4d P1=%4d\n\r",
   		mbm.packet[6], mbm.packet[7], i, j );
#endif

// Send the packet, wait for the response and remove "extra" stuff
   mbm.send = ByteCount;
   while (!(retval = client_handler(&mbm)));
   if(retval < 0)	// there was an error
   {
#if MODBUS_DEBUG_PRINT & 0x800
	printf ( "MBM_Send_ADU: Error client_handler returned %d",retval);
#endif
	return retval;
   }
   memcpy( ADUaddress, ADUaddress+MBAPSIZE, retval-MBAPSIZE );
#if MODBUS_DEBUG_PRINT & 0x800
	printf ( "TCP Rx:" );
	for ( i=0; i<retval-MBAPSIZE; i++ ) printf ( " %02X", ADUaddress[i] );
	printf ( "\n\r" );
#endif
	return MB_SUCCESS;
} // MBM_Send_Packet
//***************** END: MODBUS ************************************************

void device_init()
{
   int i,j,k;

	strcpy (dev.status,"Check addresses then Update");

   dev.tabs = dev.update = 0;
   strcpy(dev.gw_ip,MBM_SLAVE_IP);
   for(i=0;i<MAXDEVICES;i++)
	{ 	sprintf(dev.addr[i].val,"%d",i+1);
   	in_dev_dac_val[i] = 0;
   	adc_config_flag[i] = 0;
   	dac_config_flag[i] = 0;
	}

   for(i=0;i<DEV_TYPE_A;i++) {
      dev.ta[i].diox.cfg = 0;
      dev.ta[i].diox.val = 0;
      dev.ta[i].dinx = 0;
      dev.ta[i].hcox.cfg = 0;
      dev.ta[i].hcox.val = 0;
      for(j=0;j<DEVADAC;j++) {
         strcpy(dev.ta[i].dac[j].val,"0");
         dev.ta[i].dac[j].unt = 1;}
		for(j=0;j<DEVAADC/2;j++)
 			dev.ta[i].adc_cfg[j] = 0;
      for(j=0;j<DEVAADC;j++) {
       	strcpy(dev.ta[i].adc[j].val,"?");
			dev.ta[i].adc[j].unt = 1; }
   }

   for(i=0;i<DEV_TYPE_B;i++) {
      dev.tb[i].diox.cfg = 0;
      dev.tb[i].diox.val = 0;
      dev.tb[i].dinx = 0;
      dev.tb[i].hcox.cfg = 0;
      dev.tb[i].hcox.val = 0;
      for(j=0;j<DEVBDAC;j++) {
         strcpy(dev.tb[i].dac[j].val,"0");
         dev.tb[i].dac[j].unt = 1;}
		for(j=0;j<DEVBADC/2;j++)
 			dev.tb[i].adc_cfg[j] = 0;
      for(j=0;j<DEVBADC;j++) {
       	strcpy(dev.tb[i].adc[j].val,"?");
			dev.tb[i].adc[j].unt = 1; }
   }
} // device_init

union
{
	float fval;
	int ival[2];
}ADval;

void update_cb()
{
	static int mb_addr, DIOconfig[2], DIvalues[4], i, ADcfg, ADch, ADunits, ADvalue;
   TxtSt *padc;
   int *padc_cfg;
   float fADvalue;

	dev.status="";			// clear error message
   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
#if MODBUS_DEBUG_PRINT & 1
	printf( "%supdate_cb: dev.tabs = %d  dev.update = %d  address = %d\n\r%s",
           BLUE, dev.tabs, dev.update, mb_addr, BLACK );
#endif

   memset(dev.status,0,sizeof(dev.status)); 	// clear the status box
	i = MBM_ReadRegs ( mb_addr, DIOconfig, 1000, 2 ); // digOutConfig & digHoutConfig
   if ( (mbADU[0] & 0x80) | (i < 0))
	{
		sprintf(dev.status,"Error finding Modbus Address %d, Must reset Gateway!",mb_addr);
	}
   else
   {
	 MBM_ReadInRegs ( mb_addr, DIvalues, 0, 4 ); // read all inputs
	 for ( i=0; i<=3; i++ ) DIvalues[i] ^= 0xFF; // invert bits so 0v = on
	   switch ( dev.tabs )
	   {  // find the type of modbus device
	   	case 0:	// device type Gateway BL2600(A1)
	      case 1:	// device type Slave BL2600(A2)
	         dev.ta[dev.tabs].diox.cfg = DIOconfig[0];
	         dev.ta[dev.tabs].hcox.cfg = DIOconfig[1];
	         dev.ta[dev.tabs].diox.val = (DIvalues[1]<<8)|DIvalues[0];
	         dev.ta[dev.tabs].dinx = (DIvalues[3]<<8)|DIvalues[2];
	      break;
	      case 2:	// device type Slave BL2600(B1)
	      case 3:	// device type Slave BL2600(B2)
	         dev.tb[dev.tabs-DEV_TYPE_A].diox.cfg = DIOconfig[0];
	         dev.tb[dev.tabs-DEV_TYPE_A].hcox.cfg = DIOconfig[1];
	         dev.tb[dev.tabs-DEV_TYPE_A].diox.val = (DIvalues[1]<<8)|DIvalues[0];
	         dev.tb[dev.tabs-DEV_TYPE_A].dinx = (DIvalues[3]<<8)|DIvalues[2];
	      break;
	   }

	// update the A/D channels using
	   switch ( dev.tabs )
	   {  // find the type of modbus device
	   	case 0:	// device type Gateway BL2600(A1)
	      case 1:	// device type Slave BL2600(A2)
	         padc = dev.ta[dev.tabs].adc;
	         padc_cfg = dev.ta[dev.tabs].adc_cfg;
	      break;
	      case 2:	// device type Slave BL2600(B1)
	      case 3:	// device type Slave BL2600(B2)
	         padc = dev.tb[dev.tabs-DEV_TYPE_A].adc;
	         padc_cfg = dev.tb[dev.tabs-DEV_TYPE_A].adc_cfg;
	      break;
	   }

	if ( !adc_config_flag[dev.tabs] )
	{
		dev_adc_cb();
	}
	for ( ADch=0; ADch<=(dev.tabs<DEV_TYPE_A?DEVAADC:DEVBADC); ADch++ )
	   {	ADcfg = padc_cfg[ADch/2];
	   	if ( (ADch%2) && (ADcfg == 2) )
			{
				break;
			}
	   	ADunits = (padc+ADch)->unt;				// get units
		switch (ADunits)
	      {
	      case 0:	// raw
	 		MBM_ReadInRegs ( mb_addr, ADval.ival, 3003+(10*ADch), 1 );
	         itoa (ADval.ival[0], (padc+ADch)->val);
	      	break;
	      case 1:	// mV
	 		MBM_ReadInRegs ( mb_addr, ADval.ival, 3002+(10*ADch), 1 );
	         itoa (ADval.ival[0], (padc+ADch)->val);
	      	break;
	      case 2:	// V
	 		MBM_ReadInRegs ( mb_addr, ADval.ival, 3000+(10*ADch), 2 );
	 		sprintf ( (padc+ADch)->val, "%8.4f", ADval.fval );
	      	break;
	      }
	   }
    }
} // update_cb

// Digital I/O Cofiguration Callback
void dev_dio_cfg_cb()
{
   int maxio,mb_addr,mb_data;

   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
   switch ( dev.tabs )
   {  // find the type of modbus device
   	case 0:	// device type Gateway BL2600(A1)
      case 1:	// device type Slave BL2600(A2)
         maxio = DEVADIO;
         mb_data = dev.ta[dev.tabs].diox.cfg;
      break;
      case 2:	// device type Slave BL2600(B1)
      case 3:	// device type Slave BL2600(B2)
         maxio = DEVBDIO;
         mb_data = dev.tb[dev.tabs-DEV_TYPE_A].diox.cfg;
      break;
   }
#if MODBUS_DEBUG_PRINT & 1
   printf("%sdev_dio_cfg_cb: address = %d\n",BLUE,mb_addr);
   printf("dev_dio_cfg_cb: cfg[%d..0] = 0x%04X %s\n",maxio-1,mb_data,BLACK);
#endif
	MBM_WriteReg(mb_addr,1000,mb_data);
} // dev_dio_cfg_cb

// Digital I/O Value callback
void dev_dio_val_cb()
{
   int maxio,mb_addr,mb_data;

   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
   switch ( dev.tabs )
   {  // find the type of modbus device
      case 0:	// device type Gateway BL2600(A1)
      case 1:	// device type Slave BL2600(A2)
         maxio = DEVADIO;
         mb_data = dev.ta[dev.tabs].diox.val;
      break;
      case 2:	// device type Slave BL2600(B1)
      case 3:	// device type Slave BL2600(B2)
         maxio = DEVBDIO;
         mb_data = dev.tb[dev.tabs-DEV_TYPE_A].diox.val;
      break;
   }
#if MODBUS_DEBUG_PRINT & 2
   printf("%sdev_dio_val_cb: address = %d\n",BLUE,mb_addr);
   printf("dev_dio_val_cb: val[%d..0] = 0x%04X %s\n",maxio-1,mb_data,BLACK);
#endif
	MBM_WriteReg(mb_addr,0,mb_data);
	MBM_WriteReg(mb_addr,1,mb_data>>8);
} // dev_dio_val_cb

// High Current Config callback
void dev_hco_cfg_cb()
{
   int maxio,mb_addr,mb_data;

   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
   switch ( dev.tabs )
   {  // find the type of modbus device
   	case 0:	// device type Gateway BL2600(A1)
      case 1:	// device type Slave BL2600(A2)
         maxio = DEVAHCO;
         mb_data = dev.ta[dev.tabs].hcox.cfg;
      break;
      case 2:	// device type Slave BL2600(B1)
      case 3:	// device type Slave BL2600(B2)
         maxio = DEVBHCO;
         mb_data = dev.tb[dev.tabs-DEV_TYPE_A].hcox.cfg;
      break;
   }
#if MODBUS_DEBUG_PRINT & 4
   printf("%sdev_hco_cfg_cb: address = %d\n",BLUE,mb_addr);
   printf("dev_hco_cfg_cb: cfg[%d..0] = 0x%04X %s\n",maxio-1,mb_data,BLACK);
#endif
   MBM_WriteReg(mb_addr,1001,mb_data);
} // dev_hco_cfg_cb

// High Current Value callback
void dev_hco_val_cb()
{
   int maxio,mb_addr,mb_data;

   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
   switch ( dev.tabs )
   {  // find the type of modbus device
      case 0:	// device type Gateway BL2600(A1)
      case 1:	// device type Slave BL2600(A2)
         maxio = DEVAHCO;
         mb_data = dev.ta[dev.tabs].hcox.val;
      break;
      case 2:	// device type Slave BL2600(B1)
      case 3:	// device type Slave BL2600(B2)
         maxio = DEVBHCO;
         mb_data = dev.tb[dev.tabs-DEV_TYPE_A].hcox.val;
      break;
   }
#if MODBUS_DEBUG_PRINT & 8
   printf("%sdev_hco_val_cb: address = %d\n",BLUE,mb_addr);
   printf("dev_hco_val_cb: val[%d..0] = 0x%04X %s\n",maxio-1,mb_data,BLACK);
#endif
   MBM_WriteReg(mb_addr,2,mb_data);
} // dev_hco_val_cb

void dev_dac_val_cb()
{
union
{	float	fval;
   int ival[2];
} fvolts;

	int k, dac_count, mb_addr, voltage, units;
   TxtSt *pdac;

#if MODBUS_DEBUG_PRINT & 0x10
	printf ( "%sdev_dac_val_cb\n\r%s", BLUE, BLACK );
#endif
	in_dev_dac_val[dev.tabs] = 1;

   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
   switch ( dev.tabs )
   {  // find the type of modbus device
   	case 0:	// device type Gateway BL2600(A1)
      case 1:	// device type Slave BL2600(A2)
         dac_count = DEVADAC;
         pdac = dev.ta[dev.tabs].dac;
      break;
      case 2:	// device type Slave BL2600(B1)
      case 3:	// device type Slave BL2600(B2)
         dac_count = DEVBDAC;
         pdac = dev.tb[dev.tabs-DEV_TYPE_A].dac;
      break;
   }

   for ( k=0; k<dac_count; k++ )
   {	units =(pdac+k)->unt;
   	if ( units != 2 )
      {	voltage = atoi ((pdac+k)->val);
			MBM_WriteReg ( mb_addr, 2003-units+(10*k), voltage );
      }
   	else
      {	fvolts.fval = atof ((pdac+k)->val);
			MBM_WriteReg ( mb_addr, 2000+(10*k), fvolts.ival[0] );
			MBM_WriteReg ( mb_addr, 2001+(10*k), fvolts.ival[1] );
      }
   }
	in_dev_dac_val[dev.tabs] = 0;
} // dev_dac_val_cb

void dev_adc_cb()	// configure the A/D channels: anaInConfig
{	int mb_addr, chpr;

	adc_config_flag[dev.tabs] = 1;								// show adcs configured
   mb_addr = atoi(dev.addr[dev.tabs].val);	// get the modbus address
#if MODBUS_DEBUG_PRINT & 0x20
	printf ( "%sdev_adc_cb\n\r%s", BLUE, BLACK );
#endif

   switch ( dev.tabs )
   {  // find the type of modbus device
   	case 0:	// device type Gateway BL2600(A1)
      case 1:	// device type Slave BL2600(A2)
			for ( chpr=0; chpr<=3; chpr++ ) 		// for each channel pair - see anaInConfig
			{	MBM_WriteReg ( mb_addr, 3008+(10*chpr), dev.ta[dev.tabs].adc_cfg[chpr] );
         }
      break;
      case 2:	// device type Slave BL2600(B1)
      case 3:	// device type Slave BL2600(B2)
			for ( chpr=0; chpr<=3; chpr++ ) 		// for each channel pair
			{	MBM_WriteReg ( mb_addr, 3008+(10*chpr), dev.tb[dev.tabs-DEV_TYPE_A].adc_cfg[chpr] );
         }
      break;
   }
} // dev_adc_cb

int isaddr_guard(char *new)
{
   if(isaddr(new))
   	return 1;
	else
   	return 0;
}


