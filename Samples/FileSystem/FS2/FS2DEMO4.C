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
/*************************************************************************
 FS2DEMO4.C Filesystem Mk II sample program.

 The purpose of this program is to show a dirty trick for working out
 the amount of stack used by a program.  Specifically, the filesystem
 uses the most amount of stack when initializing.  We want to see just
 how much it uses.

 When using FS2.LIB, the stack is set to 2048 bytes by default.

 It is important to know how much stack is used, since filesystem
 initialization consumes stack as a function of the maximum number of
 logical sectors in any extent.  Doubling the number of LSs uses
 approximately 100 more bytes of stack.  For 1024 LSs, about 1K of
 stack will be needed by fs_init().  Since this is the same as the
 default stack allocation, it is best to limit the number of LSs in
 any extent ("partition") to 512.  In the case of 512 LS, fs_init()
 will take approximately 2 seconds to run.

 Change the initial #define's as desired.
*************************************************************************/

#class auto
#memmap xmem

#define FS_BENCHMARK		// Define this to see initialisation timings

#define FS_MAX_FILES		12	// Maximum expected existing files on all LXs

// Amount of 1st flash to use (becomes 2nd or 3rd basic LX).
#define FS2_USE_PROGRAM_FLASH	16


#use "fs2.lib"

char buf[80];


int main()
{
	int rc;						// Return code from filesystem functions
	static char * init_sp;	// Top of stack
	static char * stack_base; // Bottom allowed byte of stack
	static byte sseg;
	unsigned stack_len;
	long seg_addr;
	char * p;
	int i;
	long t;

	#asm
	ld		hl,2			; Initial return address length (for return from main)
	add	hl,sp			; Get initial top of stack
	ld		(init_sp),hl
	ioi ld a,(STACKSEG)
	ld		(sseg),a
	#endasm

	seg_addr = (long)init_sp + ((long)sseg << 16);
	stack_len = Get_Stack_Size(seg_addr);
	printf("Stack length apears to be %u bytes\n", stack_len);
	stack_base = init_sp - stack_len + 1;

	/*
	 * Memset memory from stack base up to the current stack pointer, with
	 * a special 'magic value' which we can recognize.
	 */
	#asm
	ld		hl,0
	add	hl,sp
	ld		de,(stack_base) 
	or		a 				; clear carry.
	sbc	hl,de
	ld		c,l
	ld		b,h			; BC = count of bytes to zap
	dec	bc
	ld		hl,(stack_base)
	ld		de,(stack_base)
	inc	de
	ld		(hl),0x5A	; Magic byte

	// Simulte the LDIR opcode, which has troubles under separate I&D.
	//  Original code used it to smear the magic byte thru memory.
	//  Since we clear *stack* memory, must not C support function.
.loop:
	ldi					; Fill:  *DE++ = *HL++ , BC--
	ld 	a, c
	or 	b
	jp 	nz,.loop

	#endasm

	printf("Initializing file system.  Please wait...\n");
	t = MS_TIMER;
	rc = fs_init(0,0);
	if (rc) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}
	t = MS_TIMER - t;
	printf("Took %ld ms to initialize\n", t);

	// Here we make some use of the filesystem internals in order to print
	// some useful info.
	printf("Extent table:\n");
	printf("LXD# ps_size  num_ps  num_ls  ls_size  ls_use  dev_offs  ps/ls  ls/ps\n");
	printf("---- -------  ------  ------  -------  ------  --------  -----  -----\n");
	for (i = 1; i <= _fs.num_lx; i++)
		printf("%4d %7ld  %6u  %6u  %7u  %6u  %8ld  %5u  %5u\n",
			i, _fs.lx[i].ps_size, _fs.lx[i].num_ps, _fs.lx[i].num_ls,
			_fs.lx[i].ls_size, _fs.lx[i].d_size, _fs.lx[i].dev_offs,
			_fs.lx[i].ps_per_ls, _fs.lx[i].ls_per_ps);

	/*
	 * Check upwards from stack base until a non-0x5A byte is encountered.
	 * This is a rough indication of the stack high-water mark -- or is that
	 * low-water mark? :-)
	 */
	for (p = stack_base; *p == 0x5A; p++);
	printf("Stack used = %u bytes out of %u\n",
	  init_sp + 1 - p, init_sp + 1 - stack_base);
	

	return 0;
}
