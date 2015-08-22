===========================================================
8/12/03 Information for using the Graphic Sample Programs
===========================================================

Graphic Sample Program Infor
----------------------------
The graphic sample programs are intended for Z-World's
development boards or controllers that have a graphic
LCD display module.

If you compile a graphic sample program on a product
that wasn't designed with a Graphic LCD, then you will
get compiler errors because target controller has not been
setup with #use'ing the graphic libraries.

If you're trying to use these sample programs with your
own custom Graphic LCD application, instructions for
creating a graphic LCD low-level driver is located in
the following drivers:

DC\LIB\Displays\Graphic\122x32\MSCG12232.LIB
	" OR "
DC\LIB\Displays\Graphic\320x240\SED1335F.LIB

Once you design your low-level driver you will need to #use
the graphic.lib and font libraries for your application.
Here's an example for the MSCG12232 LCD module that is used
on the OP6800 product:

// for 122x32 LCD and 1x7 keypad assembly option
#use "mscg12232.lib"    // Low-level driver......LCD specific
#use "graphic.lib"      // High-level functions..Required
#use "6x8l.lib"		// Font Lib..............Optional
#use "8x10l.lib"        // "  	" 	"
#use "12x16l.lib"       // "  	"	"
#use "courier12.lib"    // "  	" 	"

The above is located in default.h in the OP6800 ID area, however for
custom applications you can #use these libraries at the top of your
application program.

=============================================================================
EOF




