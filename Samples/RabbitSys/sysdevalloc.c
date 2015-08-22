/*****************************************************************************
This program demonstrates the basic system call device allcation interface.
It generates a pulses out of PB3 (no specific period).
/*****************************************************************************/


main() {
	handle sbcr_hdl, pbddr_hdl, pbdr_hdl;

   pbddr_hdl = _sys_open(_SYS_OPEN_INTERFACE, PBDDR);
  	_sys_write(pbddr_hdl, 0x8);
   _sys_close(pbddr_hdl);
   pbdr_hdl = _sys_open(_SYS_OPEN_INTERFACE, PBDR);
   while(1) {
   	_sys_write(pbdr_hdl, 0x0);
   	_sys_write(pbdr_hdl, 0x8);
   }
}



