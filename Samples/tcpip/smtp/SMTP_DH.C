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
        smtp_dh.c

        A small program that uses the SMTP library
        to send an e-mail.  This makes use of the smtp_data_handler() function
        introduced in DC 8.0 to generate message data on-the-fly.
*******************************************************************************/

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

/*
 *   These macros need to be changed to the appropriate values or
 *   the smtp_sendmail(...) call in main() needs to be changed to
 *   reference your values.
 */

#define FROM     "myaddress@mydomain.com"
#define TO       "myaddress@mydomain.com"
#define SUBJECT  "Junk mail"

/*
 *   The SMTP_SERVER macro tells DCRTCP where your mail server is.  This
 *   mail server MUST be configured to relay mail for your controller.
 *
 *   This value can be the name or the IP address.
 */

#define SMTP_SERVER "mymailserver.mydomain.com"

/*
 *   The SMTP_DOMAIN should be the name of your controller.  i.e.
 *   "somecontroller.somewhere.com"  Many SMTP servers ignore this
 *   value, but some SMTP servers use this field.  If you have
 *   problems, turn on the SMTP_DEBUG macro and see were it is
 *   bombing out.  If it is in the HELO command consult the
 *   person in charge of the mail server for the appropriate value
 *   for SMTP_DOMAIN. If you do not define this macro it defaults
 *   to the value in MY_IP_ADDRESS.
 *
 */

//#define SMTP_DOMAIN "mycontroller.mydomain.com"

/*
 *   The USE_SMTP_AUTH macro enables SMTP Authentication, a method
 *   where the client authenticates with the server before sending
 *   a message.  Call smtp_setauth() before smtp_sendmail() to set
 *   the username and password to use for authentication.
 */

//#define USE_SMTP_AUTH

/*
 *   If the following macro is defined, then if SMTP authentication
 *   fails, the library will NOT attempt non-authenticated SMTP.
 */
//#define SMTP_AUTH_FAIL_IF_NO_AUTH

/*
 *   The SMTP_VERBOSE macro logs the communications between the mail
 *   server and your controller.  Uncomment this define to begin
 *   logging
 */

//#define SMTP_VERBOSE

/*
 *   The SMTP_DEBUG macro allows dynamic C debugging of the SMTP library
 */

//#define SMTP_DEBUG

/********************************
 * End of configuration section *
 ********************************/

#ifndef SMTP_SERVER
  #warn "Please define an SMTP server address (string)"
#endif

#class auto
#memmap xmem
#use dcrtcp.lib
#use smtp.lib


int mail_generator(char * buf, int len, longword offset,
                                int flags, void * dhnd_data)
{
	auto int * my_data;

	if (flags == SMTPDH_OUT) {
		if (offset > 500)
			return 0;	// EOF after 500 bytes

		my_data = (int *)dhnd_data;	// Access our opaque data

		// Note that a '.' at the beginning of a line is deleted by the mail server.
		// Also, email normally requires lines to be terminated with \r\n not just \n.

		snprintf(buf, len,
      	".. Line %d: The quick brown fox jumps over the lazy dog.  %lu\r\n",
         *my_data, offset);

		(*my_data)++;	// Increment line number (data is not opaque to us!).

		return(strlen(buf));
	}
	return -1;	// Indicate don't understand or care about this flag
}


int main()
{
	int line_num;	// This is our data handler opaque parameter.

	sock_init();
	// Wait for the interface to come up
	while (ifpending(IF_DEFAULT) == IF_COMING_UP) {
		tcp_tick(NULL);
	}

   #if _USER
   	smtp_setserver(SMTP_SERVER);
   #endif

#ifdef USE_SMTP_AUTH
	smtp_setauth ("myusername", "mypassword");
#endif

	smtp_sendmail(TO, FROM, SUBJECT, NULL);	// No fixed message
	smtp_data_handler(mail_generator, &line_num, 0);	// Set message generator function

	line_num = 1;	// Initialize for data handler benefit.

	while(smtp_mailtick()==SMTP_PENDING)
		continue;

	if(smtp_status()==SMTP_SUCCESS)
		printf("Message sent\n");
	else
		printf("Error sending message\n");
	return 0;
}