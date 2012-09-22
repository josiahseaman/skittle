#include <QApplication>
#include <string>
using std::string;
#include "MainWindow.h"

/** *******************
  This is the main loop that starts the MainWindow class used in Skittle
  and every other Qt application.
  Argv[1] is passed to window.open() so that Skittle can be used by the OS
  to open FASTA files directly.  This connection is setup by making Skittle
  the associated program with .fa and .fasta file extensions.
**********************/

/** The launcher will check the server for an update, and if there is an update
  it will launch skittle with a flag 'update' in the last argument.  MainWindow
  then need to display a button in response. */
bool checkUpdateFlag(int& argc, char *argv[])
{
    for(int i = 1; i< argc; ++i)//skip first field because it is 'Skittle.exe'
    {
        string argument = string(argv[i]);
        if(argument.compare(string("update")) == 0)//0 if strings are equivalent
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    if(argc > 1)
    {
        if(checkUpdateFlag(argc, argv))//update flag should be the last argument
        {
            window.showUpdateButton();
            argc -= 1; //remove update from the argument count
        }
    }
    if(argc > 1)
        window.open(QString(argv[1]));//file to open should be the first argument argv[1]
    else{
        window.open();
        //        window.open(QString("C:\\Users\\Josiah\\Documents\\Genomes\\Human Genome\\chr18.fa"));
    }
    return app.exec();
}
