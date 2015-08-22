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
 /***********************************************************
  *
  *   Syntax:    HTTP.C
  *   Created:    08/31/2005 9:52AM
  *   Author:     keven morlang
  *   Requested:
  *   Comments:   A sample program to demonstrate the RabbitSys
  * 					HTTP internal server.
  *
  ************************************************************/
#use "DCRTCP.LIB"

/*--------------------------------------------------
 This structure is used to pass data back to the
 internal HTTP server of RabbitSys.
 --------------------------------------------------*/
rs_SSI_CGI_State st;

// this is an output buffer for our web related text.
char mybuffer[256];

/*************************************************************************
SYNTAX:			rs_SSI_CGI_State *testproc (int param)

DESCRIPTION:	Generate Web Content (HTML) based on the parameter
passed to the function.

RETURNS:			RabbitSys SSI/CGI function result structure address

END DESCRIPTION **********************************************************/
rs_SSI_CGI_State *testproc (int param)
{
	// We always return this string no matter what the value
	// of param.
	st.buffer = mybuffer;
   sprintf(st.buffer,"<BR>Text from an SSI procedure.\n<BR>Param was %d\n",
   	param);
	if ( param==42 )	//create a link to the RabbitSys pages
	{
		strcpy(mybuffer,"<P><A HREF=/RABBITSYS>RabbitSys Home</A>\n</BODY>\n</HTML>");
		st.retval = RS_SSI_SENDDONE;
	}
	else if (param==15)
		st.retval = RS_SSI_SENDDONE;
	else
	{
		// We have been called with (param != 15), so
		// increment param, tell the server to send
		// our data and call us again.
		st.newParameter = param+1;
		st.retval = RS_SSI_SEND;
	}
	return &st;
}

/***********************************************************
Syntax:			rs_SSI_CGI_State *pureproc (int param);

Description:	generate an entire web page using the SSI
style of function calls.

Parameter:		state value.

Return Value:	RabbitSys SSI/CGI function result structure address
************************************************************/
rs_SSI_CGI_State *pureproc (int param)
{
	st.buffer = mybuffer;
	st.retval = RS_SSI_SEND;		//our default return code.
	st.newParameter = param+1;
	if (!param)
	{
		// The RabbitSys HTTP server will generate a header for us. It
		// is placed in our buffer.
		_sys_httpGenHeader(st.buffer, sizeof(mybuffer), 200, "text/html");
	}
	else if (param==1)
	{
		// generate part of the web page...
		sprintf(st.buffer,"<HTML><HEAD Title=Function Only></HEAD><BODY>");
	}
	else if (param==2)
	{
		// generate more of the web page...
   	sprintf(st.buffer,"Test from pureproc()",	param);
   }
	else if ( param==3 )
	{
		// generate the final bit of the web page.  Set the return state
		// to "send this, I am finished."
		strcpy(mybuffer,"<P><A HREF=/RABBITSYS>RabbitSys Home</A>\n</BODY>\n</HTML>");
		st.retval = RS_SSI_SENDDONE;
	}
	return &st;
}//pureproc()

/* ***********************************************************************
Syntax:			rs_SSI_CGI_State *rscgi_func (sysHttpState *h)

DESCRIPTION:	handle CGI states for our form.

PARAMETER:		pointer to RabbitSys' HTTP state control structure.

RETURN:

END DESCRIPTION **********************************************************/
rs_SSI_CGI_State *rscgi_func (sysHttpState *h)
{
	int i;
	char *p;

   st.buffer = mybuffer;
	switch(h->action)
   {
   case RS_CGI_PROLOG:
		_sys_httpGenHeader(mybuffer, RS_HTTP_MAXBUFFER,200, NULL);
      strcat(mybuffer, "<HTML><BODY>");
		st.retval = RS_CGI_SEND;
		printf("%s\n",mybuffer);
   	break;

   case RS_CGI_START:
   	// This is the beginning of a tag-value pair.  The start
   	// condition occurs once for each field on your form.
   	strcpy(mybuffer, "<P>");
   	printf(mybuffer);
		st.retval = RS_CGI_SEND;
		break;

	case RS_CGI_DATA:
		// This state handles one field for each iteration.
		// The field name will be in h->tag, and h->buffer
   	// will be the field value. The code below shows how
   	// to parse out the field value.
		i = sprintf(mybuffer,"Data field name:%s<BR>\nForm data: ",h->tag);
		// copy out the field value.
		strncpy(&mybuffer[i], h->buffer, (int)h->length);
      mybuffer[i + (int)h->length] = 0;
      // clear off eol characters
      p = strstr(mybuffer, "\r\n");
      if(p)
      	*p = 0;
		st.retval = RS_CGI_SEND;
		printf("%s\n",mybuffer);
      break;

	case RS_CGI_END:
		// When a field from your form has been processed this
		// state occurs.
		strcpy(mybuffer,"</P>\n");
		st.retval = RS_CGI_SEND;
		printf(mybuffer);
      break;

	case RS_CGI_EOF:
		// This state indicates the end of form processing (no
		// more fields).
      strcpy(mybuffer,"</BODY></HTML>");
      st.retval = RS_CGI_SEND_DONE;
		printf("%s\n",mybuffer);
      break;

   default:
		st.retval = RS_CGI_OK;
		break;
	}
	return &st;
}


#ximport "samples/rabbitsys/static_ssi.html"    index_html
#ximport "samples/tcpip/http/pages/rabbit1.gif" rabbit1_gif
#ximport "samples/rabbitsys/RabbitSysCGI.html"  rscgi_html

RS_HTTPRESOURCETABLE_START
RS_HTTPRESOURCE_XMEMFILE_HTML("/", index_html),
RS_HTTPRESOURCE_XMEMFILE_GIF("/rabbit1.gif", rabbit1_gif),
RS_HTTPRESOURCE_FUNCTION("testfunc", testproc),
RS_HTTPRESOURCE_FUNCTION("/purefunc", pureproc),
RS_HTTPRESOURCE_XMEMFILE_HTML("/rscgi", rscgi_html),
RS_HTTPRESOURCE_CGI("/rabbitsys_cgi.cgi", rscgi_func),
RS_HTTPRESOURCETABLE_END

void main (void)
{
	char cmdStr[40];

	RS_REGISTERTABLE();
   inet_ntoa(cmdStr,gethostid());
   printf("IP:%s\n",cmdStr);
   while (1)
		_sys_tick(1);
}