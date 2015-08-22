/*******************************************************************************

	Samples\RabbitSys\usersyscall.c
	Rabbit Semiconductor, 2005

   Setup: Connect PB3 to PE4.

   This program demonstrates how to register and use a user mode syscall so that
   system level code and be run in the context of a user mode program.
   Register and using a user defined syscall requires only 2 API functions:

   _sys_register_usersyscall - Call this function and pass the address of the
                               function to run in system mode.  The user defined
                               syscall must have the following signature:
                               int user_syscall(int type, void* param);
                               where type is user defined and can be used to
                               determine what to do when the function is called,
                               and param is user defined data to be used in the
                               function.

	_sys_usersyscall          - Call this function to call the function
                               registered using _sys_register_usersyscall.

*******************************************************************************/


int interrupt_count;
nodebug root void ext_int0_isr(void);
void generate_interrupt();
nodebug int systemmode_test(int type, void* param);

enum {ENABLE_PORT_B, REGISTER_WRITE};
typedef struct
{
	int  port;
   char data;
} reg_write_t;

void main()
{
	int i;
	reg_write_t reg_write;

   // Register user defined syscall.
	_sys_register_usersyscall(systemmode_test);

	// Set PB3 to output
	WrPortI(PBDDR, &PBDDRShadow, PBDDRShadow | 0x08);
   // Set PE4 to input
   WrPortI(PEDDR, &PEDDRShadow, PEDDRShadow & ~0x10);
	// Set interrupt handler for external INT0
   SetVectExtern3000(0, ext_int0_isr);
   // Enable external INT0 on PE4, rising edge, priority 1
   WrPortI(I0CR, &I0CRShadow, 0x21);

	while(1)
   {
   	// Call user syscall to enable use of parallel port B through its user
      // enable register, generate interrupts and show the number of interrupts
      // handled.
	   _sys_usersyscall(ENABLE_PORT_B, NULL);
   	interrupt_count = 0;
		for(i = 0; i < 2000; i++)
      {
			generate_interrupt();
      }
      printf("Port B enabled: %d\n", interrupt_count);

		// Call user syscall to disable use of parallel port B through its user
      // enable register and attempt to generate interrupts.  Since the parallel
      // port B user enable register is not set, no interrupts will be generated
      // and the count will be zero at the end of this loop.
		// Note that disabling of PBUER is handled differently than above.  This
      // is purely for demonstration purposes to show the use of the parameter
      // that can be passed to the user defined syscall.
      reg_write.port = PBUER;
      reg_write.data = 0x00;
	   _sys_usersyscall(REGISTER_WRITE, (void*)&reg_write);
   	interrupt_count = 0;
		for(i = 0; i < 2000; i++)
      {
	   	generate_interrupt();
      }
      printf("Port B disabled: %d\n", interrupt_count);
   }
}

// This function implements a user definable syscall.  Great care must be taken
// in a user definable syscall since the code runs entirely in system mode.  In
// the following example, in the ENABLE_PORT_B case, the parallel port B user
// enable register is set to make parallel port B accessible in user mode.  Code
// running in user mode cannot alter the state of any of the user enable
// registers.  The REGISTER_WRITE case is a more generalized case where any
// internal I/O register can be written to and is particularly dangerous as an
// incorrect write to a system only register could bring RabbitSys down.

// NOTE: user defined syscalls must be specified nodebug.  It is possible to
// specify the function as debug, but as soon as the first rst 28 is
// encountered, the function will execute in user mode.  Returning from the
// function to the system after attempting to debug will result in a write
// protection violation.
nodebug int systemmode_test(int type, void* param)
{
	reg_write_t* reg_write;
   int port;
   char data;

	switch(type)
   {
   case ENABLE_PORT_B:
   	#asm
		ld		a,0x80
      ioi   ld (PBUER),a
      #endasm
   	break;
   case REGISTER_WRITE:
		reg_write = (reg_write_t*)param;
      port = reg_write->port;
      data = reg_write->data;
   	#asm
		ld		hl,(sp+@sp+data);
      ld		a,l
      ld		hl,(sp+@sp+port)
      ioi	ld (hl),a
      #endasm
      break;
   }
   return 0;
}

// External INT0 interrupt handler - increment the interrupt count and return.
#asm root nodebug
ext_int0_isr::
	push	hl
	ld		hl,(interrupt_count)
   inc	hl
   ld		(interrupt_count),hl
   pop	hl
	ipres
   ret
#endasm

// Generate an interrupt by toggling PB3
void generate_interrupt()
{
	WrPortI(PBDR, &PBDRShadow, PBDRShadow & ~0x08);
   WrPortI(PBDR, &PBDRShadow, PBDRShadow |  0x08);
}

