/*******************************************************************************
        dns2.c
        Z-World, 2001

        Demonstration of how to look up an IP address through a DNS
        server using the nonblocking API.  This function looks up
        multiple hostnames simultaneously.  It is organized around
        a state machine, with separate state information maintained
        for each request.
*******************************************************************************/
#class auto


/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1


// MY_DOMAIN specifies the domain to add to a hostname in the resolving
// process.  This domain is added automatically when there is no '.' in
// the hostname to be looked up.  If the hostname does not contain a '.',
// and the first lookup fails, then it is tried again without the domain.
// Vice versa, if the hostname does contain a '.', then the domain is not
// added to the hostname until the lookup has first failed without the
// domain.
#define MY_DOMAIN			"zworld.com"

// This lists the hostnames that will be looked up by this program.  This
// can be customized.
char* const hostnames[] =
	{
	  "www.zworld.com",
	  "www.rabbitsemiconductor.com",
	  "www.frobozz.xyzzy",					// This host does not exist, so it will
	  												// fail.
	  "www",										// This will have ".zworld.com" appended
	  												// to it.
	  "demo.zweng.com"						// This name will be looked up later than
	  												// the others, since only 4 concurrent
	  												// resolves are supported by default.
	};

#memmap xmem
#use "dcrtcp.lib"

// This structure holds the state information for the DNS requests.
typedef struct {
	int state;
	longword ipaddress;
	int handle;
} request_info;

// These are the states of the state machine.
#define DNS_START			0
#define DNS_CHECKING		1
#define DNS_FINISHED		2

void main(void)
{
	request_info dns_requests[sizeof(hostnames) / sizeof(char *)];
	int i;
	int retval;
	char buffer[20];
	int done;

	// Initialize the dns_requests structure
	for (i = 0; i < (sizeof(dns_requests) / sizeof(request_info)); i++) {
		dns_requests[i].state = DNS_START;
	}

	if (sock_init() != 0) {
		printf("sock_init() failed!\n");
	}

	// Loop until we are done with all DNS requests.
	done = 0;
	while (done == 0) {
		tcp_tick(NULL);

		// Drive the state machine
		for (i = 0; i < (sizeof(dns_requests) / sizeof(request_info)); i++) {

			switch (dns_requests[i].state) {
			case DNS_START:
				// Start the process of looking up the hostname.
				// resolve_name_start() returns a handle that we use in
				// subsequent calls to resolve_name_check().
				dns_requests[i].handle = resolve_name_start(hostnames[i]);
				if (dns_requests[i].handle >= 0) {
					printf("Starting the lookup for %s  ...\n", hostnames[i]);
					dns_requests[i].state = DNS_CHECKING;
				} else if (dns_requests[i].handle == RESOLVE_LONGHOSTNAME) {
					printf("The name %s is too large!\n", hostnames[i]);
					dns_requests[i].state = DNS_FINISHED;
				}
				break;

			case DNS_CHECKING:
				// Check if the given DNS request has finished
				retval = resolve_name_check(dns_requests[i].handle,
				                            &dns_requests[i].ipaddress);
				if (retval == RESOLVE_SUCCESS) {
					// The request finished successfully
					printf("%s is %s\n", hostnames[i],
					       inet_ntoa(buffer, dns_requests[i].ipaddress));
					dns_requests[i].state = DNS_FINISHED;
				} else if (retval == RESOLVE_FAILED) {
					// The given hostname does not exist
					printf("%s does not exist\n", hostnames[i]);
					dns_requests[i].state = DNS_FINISHED;
				} else if (retval == RESOLVE_TIMEDOUT) {
					// Our request has timed out, and can not be completed.
					printf("Lookup for %s timed out\n", hostnames[i]);
					dns_requests[i].state = DNS_FINISHED;
				} else if (retval == RESOLVE_AGAIN) {
					// Call resolve_name_check again next time through the state machine
				} else if (retval == RESOLVE_HANDLENOTVALID) {
					// Should never happen in this program
					printf("Invalid handle for %s (should never happen)\n",
					       hostnames[i]);
					dns_requests[i].state = DNS_FINISHED;
				} else {
					// Should never happen
					printf("Unknown return type for %s\n", hostnames[i]);
					dns_requests[i].state = DNS_FINISHED;
				}
				break;

			case DNS_FINISHED:
				// Do nothing
				break;
			}
		}

		// Check if all requests are finished
		done = 1;
		for (i = 0; i < (sizeof(dns_requests) / sizeof(request_info)); i++) {
			if (dns_requests[i].state != DNS_FINISHED) {
				done = 0;
			}
		}
	}

	printf("\nDone!\n");
}