If you are not a developer and simply want to use Skittle for research please download Skittle.zip from the sourceforge website which contains SkittleToo.exe and chrY-sample.fa.

Notes for Developers: Compiling Skittle

In order to compile Skittle you will need a development environment and a recent version of the Qt SDK 4.4 or higher.

I used gcc on Windows XP and Windows 7 using 32-bit and 64-bit systems.

Makefile.release and Makefile.debug were generated using qmake.exe though they have been modify to properly include the OpenGL/GLUT libraries.

If you modify Makefile.release you should know that qmake puts the OpenGL tags in the wrong order.  This is the correct order:

LIBS        =        -L"c:\Qt\lib" -lmingw32 -lqtmain -lQtOpenGL -lglut32 -lglu32 -lopengl32 -lQtGui -lgdi32 -lcomdlg32 -loleaut32 -limm32 -lwinmm -lwinspool -lmsimg32 -lQtCore -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32 -lws2_32

Specifically: -lQtOpenGL -lglut32 -lglu32 -lopengl32  need to be in that order.

I compiled and linked the entire Qt SDK statically so that I could have a single distributable executable and so that users didn't need to download the Qt libraries.

Finally, in order to break the dependce on mingwm10.dll use -fexceptions flag to disable C runtime exceptions since Skittle doesn't use them.

==Portability==
The Skittle executable has been tested on many Windows machines.  It has never been compiled for Linux or Mac, though the libraries themselves are all platform indepent.  The only platform specific component is the fact that it is written in C++.  Likely problem areas are in file input/ouput and in low level computation such as the logic for Local Alignment, which is highly optimized.  If anyone wants to undertake porting Skittle to other systems it would be very appreciated.