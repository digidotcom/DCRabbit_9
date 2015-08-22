/********************************************************************
   SSL_RWEB_LED.C
   Z-World, 2004

	Description
	===========
   This program implements a WEB server and a secure Web server and shows
   how to password protect a secure page. This program requires both the
   SSL and RabbitWeb modules which are separate purchase.

	Instructions
	============
	1. Make changes in the configuration section below ani TCP_CONFIG.LIB
      to match your requirements.
	2. You must edit the lines:
       <A HREF="https://10.10.6.206/admin/index.zhtml">Email Settings</A>
      in  /SSLmain.zhtml
      AND
       <A HREF="http://10.10.6.206/index.zhtml">Back to main page</A>
      in  /pages/SSLsecure.zhtml to the same dotted IP Address you
      define in TCP_CONFIG.LIB.
	3. Compile and run this program.
	4. Open your browser to http://xxx.xxx.xxx.xxx where xxx. is the IP
      address defined TCP_CONFIG.LIB. You view LED status in the main
      page and change LED status in the secure page.

      You may have to refresh the main page for the LED status
      for status changes to show, depending on your browser settings.

********************************************************************/

/***********************************
 * Configuration Section           *
 ***********************************/
#define TCPCONFIG 1
#define USE_RABBITWEB 1

#define USERNAME "rabbit"
#define PASSWORD "carrot"

/*
 * Web server configuration
 *
 * Define the number of HTTP servers and socket buffers.
 * With tcp_reserveport(), fewer HTTP servers are needed.
 */
#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2
#define HTTP_SSL_SOCKETS 1
#define USE_HTTP_SSL

// Import the SSL certificate
#ximport "cert/mycert.dcc" SSL_CERTIFICATE

/********************************
 * End of configuration section *
********************************/

#use "dcrtcp.lib"
#use "http.lib"

#ximport "pages/SSLmain.zhtml"		   index_zhtml
#ximport "pages/SSLsecure.zhtml"	   	mailpage_zhtml

// The default for / must be first
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", index_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", index_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/admin/index.zhtml", mailpage_zhtml)
SSPEC_RESOURCETABLE_END

// Define web variables
int LED01, LED02;
#web LED01
#web LED02

#web_groups admin

main()
{
   auto int value, oldvalue;
   static userid;
   LED01 = LED02 = 0;
   oldvalue = 0;

	brdInit();				//initialize board for this demo
	sock_init();
	http_init();

   // Reserve the HTTPS port 80
	tcp_reserveport(80);
   // Reserve the HTTPS port 443
	tcp_reserveport(443);

   // Set up password protection
   sspec_addrule("/admin", "Admin", admin, admin, SERVER_HTTPS,
                 SERVER_AUTH_BASIC, NULL);

   userid = sauth_adduser(USERNAME, PASSWORD, SERVER_HTTPS);
   sauth_setusermask(userid, admin, NULL);

	while (1) {
		http_handler();
   	value=PFDRShadow&0x3F;   // on state for leds
      if(!LED01)
			value = value | 0x40;
      if(!LED02)
			value = value | 0x80;
      if(oldvalue!=value){
         // so we only write to the port when something changes
         oldvalue = value;
         WrPortI(PFDR, &PFDRShadow, value);
      }
	}
}