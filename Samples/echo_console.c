/*
*/
/*****************************************************

     echo_console.c

     This program echo to stdio characters received from a serial port
     and will send eny characters typed on stdio to the serila port.
     It must be run with a serial utility such as Hyperterminal.

******************************************************/

#define PORT_B  0
#define PORT_C  1
#define PORT_D  2

#define PORT  PORT_C
#define  BAUDRATE 19200L

#if (PORT == PORT_B)
  #define BINBUFSIZE  127
  #define BOUTBUFSIZE 127
  #define serOpen   serBopen
  #define serGetc   serBgetc
  #define serPutc   serBputc
  #define serClose  serBclose
#elif (PORT == PORT_C)
  #define CINBUFSIZE  127
  #define COUTBUFSIZE 127
  #define serOpen   serCopen
  #define serGetc   serCgetc
  #define serPutc   serCputc
  #define serClose  serCclose
#elif (PORT == PORT_D)
  #define DINBUFSIZE  127
  #define DOUTBUFSIZE 127
  #define serOpen   serDopen
  #define serGetc   serDgetc
  #define serPutc   serDputc
  #define serClose  serDclose
#else
  #warns "Invalid serial port"
#endif


void main()
{
  int c;

//  brdInit();

  c = 0;
  serOpen(BAUDRATE);
  while (1)
  {
    if ((c = serGetc()) != -1)
    {
      putchar(c);
      if (c == '\r')
      	putchar ('\n');
    }
    if (kbhit ())
    {
      c = getchar ();
      serPutc (c);
      if (c == '\r')
      	serPutc ('\n');
    }
  }
  serClose();
}