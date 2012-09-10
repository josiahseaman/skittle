#include <QApplication>

#include "MainWindow.h"

/** *******************
  This is the main loop that starts the MainWindow class used in Skittle
  and every other Qt application.
  Argv[1] is passed to window.open() so that Skittle can be used by the OS
  to open FASTA files directly.  This connection is setup by making Skittle
  the associated program with .fa and .fasta file extensions.
**********************/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    if(argc > 1)
    {
		window.open(QString(argv[1]));
	}
	else{
		window.open();
	}	
    return app.exec();
}
