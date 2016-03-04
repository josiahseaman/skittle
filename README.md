Install Visual Studio 2010 on your data drive: http://download.cnet.com/Microsoft-Visual-Studio-2010-Professional/3000-2212_4-10618634.html   
	Note: Installing the trial is fine as all we need is the compiler. Where VS the GUI App may stop working after 30 days, the CLI compiler should continue to be valid

Download QT Open Source 4.8.1 (vs2010): https://download.qt.io/archive/qt/4.8/4.8.1/qt-win-opensource-4.8.1-vs2010.exe  
	Install on your data drive

Download Qt Creator Open Source 2.4.1: https://download.qt.io/archive/qtcreator/2.4/qt-creator-win-opensource-2.4.1.exe  
	Install on your data drive  
		Don't install MinGW items
		
Launch Qt Creator  
	Open Tools->Options->Build & Run->Qt Versions  
		Add new pointing to your Qt installation/bin/qmake.exe  
	Open Skittle project  
		Build  
		For now, just copy QtCore4.dll, QtGui4.dll and QtOpenGL4.dll (or their debug version if doing a debug build) from \path\to\Qt\bin to the build output directory
