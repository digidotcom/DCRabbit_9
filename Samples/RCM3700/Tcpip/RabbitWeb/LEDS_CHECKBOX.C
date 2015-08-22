/********************************************************************
	LEDsCheckbox1.c
   Z-World 2004

	This program is used with RCM3700 series controllers with
	prototyping boards.

	Description
   This is a  bare-bones (almost) sample of using RabbitWeb to control
   digital i/o using check boxes.

	===========
	Instructions
	============
	1. Compile and run this program.
	2. Open browser to http://xxx.xxx.xxx.xxx where xxx.. is the IP address
      of the RCM37xx defined in TCP_CONFIG.LIB
	3. Use the checkboxes and Submit button to control two LEDs on the
      proto-board
********************************************************************/

// USe Ethernet and the static IP address as #defined in TCP_CONFIG.LIB
#define TCPCONFIG     1
#memmap xmem
#define USE_RABBITWEB 1
#use "dcrtcp.lib"
#use "http.lib"

#ximport "pages/ledsindex.zhtml" index_zhtml
#ximport "pages/rabbitlogo_C.gif" animate

SSPEC_MIMETABLE_START   // define file handlers
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif")
SSPEC_MIMETABLE_END

SSPEC_RESOURCETABLE_START   // define resources
	SSPEC_RESOURCE_XMEMFILE("/", index_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", index_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/animate.gif", animate)
SSPEC_RESOURCETABLE_END

// define web variables
int LED01, LED02;
#web LED01
#web LED02

main(){
   auto int value, oldvalue;

   brdInit();				//initialize board for this demo
   LED01 = LED02 = 0;
   oldvalue = 0;
   sock_init();
   http_init();
	tcp_reserveport(80);  // this helps speed access up

   while(1) {
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