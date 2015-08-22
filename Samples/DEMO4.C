/*****************************************************

     demo4.c
     Z-World, 2003

     Example of using Execution Tracing

     Execution tracing is configured in the Debug tab of the Project Options
     dialog box. For the sample program, check Enable Tracing and select Full
     from Trace Level radio buttons. The information recorded for lines traced
     will correspond to the Trace Window Information Fields selected.

     Tracing entries are generated in three ways:
      1) The _TRACE macro - Program state information is recorded for the single
         point at which it is executed regardless of tracing on/off state or
         whether the function in which it is placed is debug or nodebug.
      2) The _TRACEON macro - This does not cause any information to be recorded
         by itself like the _TRACE macro, but rather causes a change of state
         within the debug kernel so that program state information is recorded
         for program and library statements executed thereafter, until the
         _TRACEOFF macro is executed or until Inspect | Trace Off is hit.
      3) The Inspect | Trace On menu option - This is effectively the same as
         the _TRACEON macro, setting a tracing state which can be turned off
         with the Inspect | Trace Off menu option or the execution of the
         _TRACEOFF macro within the program. These menu options will typically
         be used at breakpoints.

     Tracing accumulates entries into a buffer, but the process is intrusive and
     can affect programs sensitive to timing. Execution is slower if the trace
     window is open while tracing and slower still if the watch window is also
     open. Tracing continues until the  program terminates or until a run-time
     error occurs. Whenever the trace window is be opened the buffer contents
     are transferred to the window.

     The user is encouraged to experiment with the configuration, the macros and
     the menu options within this and other programs to learn how execution flow
     can be traced within Dynamic C.
******************************************************/

nodebug
void foo1()    // entry/exit never traced for nodebug functions
{
	int i;

	i = 0;      // never traced - foo() is nodebug
	_TRACE      // always traced
	i = 1;      // never traced - foo() is nodebug
}

debug
void foo()     // entry into foo() traced only if tracing is on when entered
{
	int i;

	i = 0;      // traced if tracing is on coming into foo1()
   #asm debug
   push	af    // traced if tracing is on coming into foo1()
   _TRACEON    // all debug enabled statements and fn entry/exit will be traced
   pop	af    // always traced
   #endasm
	i = 1;      // traced unless Inspect|Stop Tracing is hit at prior breakpoint
	i = 2;      // traced unless Inspect|Stop Tracing is hit prior
}

debug
main ()
{
   int i;

   i = 0;      // traced only if restarting
   foo();      // tracing turned on in foo()
   i = 1;      // traced unless Inspect|Stop Tracing was hit in foo()
   _TRACEOFF   // only _TRACE will be traced until _TRACEON
   foo1();     // no tracing for nodebug foo1() except for _TRACE within
   i = 2;      // not traced unless Inspect|Start Tracing was hit on prior line
   i = 3;      // not traced unless Inspect|Start Tracing was hit on prior line
   i = 4;      // not traced unless Inspect|Start Tracing was hit on prior line
   _TRACE      // always traced
   i = 3;      // not traced unless Inspect|Start Tracing was hit prior
   foo();      // tracing turned on in foo()
   i = 4;      // traced unless Inspect|Stop Tracing is hit in foo()
   foo1();     // tracing on but no tracing for nodebug foo1() except for _TRACE
   i = 5;      // traced unless Inspect|Stop Tracing is hit prior
}              // current tracing state retained when restarted



