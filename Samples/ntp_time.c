/******************************************************************************
 *
 *    ntp_time.c
 *
 *  Test of SNTP client implementation
 *
 *  Author: Mircea Neacsu
 *  Date:   Jan 18, 2015
 *
*******************************************************************************/

#define TCPCONFIG 7
#define MAX_UDP_SOCKET_BUFFERS  2

#define BOOTP_VERBOSE
#define NTP_VERBOSE

#memmap xmem
#use "dcrtcp.lib"


#ifndef RTC_IS_UTC
/*
  Offset between local time and UTC (in hours) and name of time zone.
  For more explanations see RTCLOCK.LIB
*/
float timezone;
#define TIMEZONE timezone

char tzname[20];
#define TZNAME tzname
#endif

//Number of servers configured
#define NSERVERS          4

/*
  Hostnames (or dotted quad IP addresses) of each time server to query.
  You can "weight" some servers as being more reliable by mentioning the
  same server in more than one entry.
  NOTE: the following addresses were obtained from hosts that were
  denoted as "open access" (see list at http://ntp.org) to NTP.
*/
char* const ntp_servers[NSERVERS] =
{
   "pool.ntp.org"
  ,"ntp1.cmc.ec.gc.ca"
  ,"time.chu.nrc.ca"
  ,"timelord.uregina.ca"
};
#use "ntp.lib"



int main()
{
  int rc;
  long dst_start, dst_end, eur_start, eur_end;
  struct tm tmp;

  brdInit();
  sock_init();

  //wait for DHCP to bring the interface up
  while (ifpending (IF_ETH0) != IF_UP)
    tcp_tick (NULL);

  /* Use the nth_dow function (which in turn uses dow function) in NTP.LIB to
  determine the DST start and end times. Assumes RTC has at least the good year
  to get the correct start/end dates for DST. Worst case, just run the program
  twice. */
  mktm(&tmp, read_rtc());

  //Current US and Canada rules: DST starts on second Sunday in March at 02:00
  tmp.tm_mon = 3;
  tmp.tm_mday = nth_dow (tmp.tm_year+1900, 3, 0, 2);
  tmp.tm_hour = 2;
  tmp.tm_min = 0;
  tmp.tm_sec = 0;
  dst_start = mktime(&tmp);
  printf ("In North America DST start is %s\n", format_timestamp(dst_start));

  //DST ends on first Sunday of November at 02:00
  tmp.tm_mon = 11;
  tmp.tm_mday = nth_dow (tmp.tm_year+1900, 11, 0, 1);
  dst_end = mktime(&tmp);
  printf ("DST end is %s\n", format_timestamp(dst_end));

  //set timezone for Eastern time
  if (SEC_TIMER > dst_start && SEC_TIMER < dst_end)
  {
    timezone = -4;
    strcpy (tzname, "EDT");
  }
  else
  {
    timezone = -5;
    strcpy (tzname, "EST");
  }

  //Test for last week spec in nth_dow function

  //In Europe DST (summer time) starts on last Sunday in March at 01:00
  tmp.tm_mon = 3;
  tmp.tm_hour = 1;
  tmp.tm_mday = nth_dow (tmp.tm_year+1900, 3, 0, 5);
  eur_start = mktime(&tmp);
  printf ("In Europe summer time starts %s\n", format_timestamp(eur_start));

  //Summer time ends on last Sunday in October at 01:00
  tmp.tm_mon = 10;
  tmp.tm_mday = nth_dow (tmp.tm_year+1900, 10, 0, 5);
  eur_end = mktime(&tmp);
  printf ("...and ends %s\n", format_timestamp(eur_end));

  //Sync RTC with NTP servers
  ntp_set_time (NULL);

  return 0;
}