/*******************************************************************************

	Samples\RabbitSys\usermodeupload.c
	Rabbit Semiconductor, 2005

	This program demonstrates how to upload a new user program to RabbitSys using
   the syscall interface.  RabbitSys supports program upload through its own
   HTTP and FTP servers, and also provides this method for user programs that
   run in Fast RAM.  The user program upload interface is simple:

   _sys_uploadstart    - Call this function first to let RabbitSys know a new
                         user program is available.
   _sys_uploaddata     - Call this function repeatedly until the entire .upl
                         file has been loaded
   _sys_uploadend      - Call this function once the .upl file is completely
                         loaded
   _sys_uploadstartupl - Call this function after _sys_uploadend verifies that
                         the user program was written to flash correctly to
                         start the newly loaded program.

*******************************************************************************/

// To simplify the demonstration of uploading user code to RabbitSys, the user
// code is ximported directly into this sample program.  This .upl file was
// created in Dynamic C by disabling all debugging support in the user program
// and compiling to a .bin file (Original source in samples\RCM3360\flashled.c).
#ximport "flashled.upl" upl_file

void shutdown(void)
{
	printf("System shutdown!\n");
   printf("====================================================\n");
   printf("Safe state handling should be done prior to starting \n");
   printf("the user program update.\n");
}

void main()
{
	auto long bytes_left;
   auto long upl_offset;
   auto char upl_buf[512];
   _sys_event_data_t edata;


	// Register a shutdown event with RabbitSys.  RabbitSys will call the
   // specified user function (shutdown) when the user program calls
   // _sys_uploadstart.
	_sys_add_event(_SYS_EVENT_SHUTDOWN, shutdown, NULL, &edata);

	// Tell RabbitSys we're about to upload a new user program.  Since this
   // program upload is being initiated by a user program, RabbitSys will not
   // halt the user program.  It is up to the user mode program to make sure
   // that it is in a safe state prior to updating itself.
	_sys_uploadstart();

	// Get the number of bytes that need to be transferred to RabbitSys.
   bytes_left = xgetlong(upl_file);

   // Move the offset beyond the length that is stored at the beginning of the
   // ximported user mode program.
   upl_offset = upl_file + sizeof(long);

   while(bytes_left > 512)
   {
		// Move 512 bytes at a time from xmem to a buffer on the stack
		xmem2root(upl_buf, upl_offset, 512);

      // Transfer 512 bytes from stack buffer to RabbitSys.  RabbitSys will
      // write the user mode program to the flash as it is received.
      _sys_uploaddata(upl_buf, 512);

      upl_offset += 512;
      bytes_left -= 512;
   }
   if(bytes_left > 0)
   {
   	// Handle any remaining bytes
		xmem2root(upl_buf, upl_offset, (int)bytes_left);
      _sys_uploaddata(upl_buf, (int)bytes_left);
   }

   // After all of the data is transferred, _sys_uploadend is called to
   // determine whether the transferred file was valid.
	if(_sys_uploadend(1) == 0)
   {
      // MD5 checksums matched, user program was successfully written to flash.
      // Start the new user program which will overwrite this program.
      _sys_upload_startupl();
   }
   else
   {
		printf("The user program did not update correctly.\n");
   }
}

