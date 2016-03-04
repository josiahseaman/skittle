#include <QApplication>
#include <QDebug>
#include <string>
#include <stdio.h>
//#include <windows.h>
#include "MainWindow.h"
using std::string;

/** GLOBAL VARIABLES **/
bool global_usingTextures = true;
//bool global_debugMode = false;


bool checkForSkittleTemp()
{
    //check for SkittleTemp.exe
    char buffer[1024];
    FILE* fd = _popen("dir /B SkittleTemp.exe", "r");
    if (fd == NULL) {
        return false;//error occured
    }
    bool found = false;
    while(NULL != fgets(buffer, sizeof(buffer), fd))
    {
        QString s = QString(buffer).trimmed();
        qDebug() << s;
        if( s.compare(s,QString("SkittleTemp.exe"), Qt::CaseInsensitive) == 0)
            found = true;
    }
    fclose (fd);
    return found;
}

/** In case the Launcher itself needs to be updated, it will download a new version
  of itself and name it SkittleTemp.exe.  It will then launch the SkittleToo and close.
  This program (SkittleToo) needs to check for the temporary file and rename it if it
  exists.*/
void checkForLauncherUpdate()
{
    //    bool found = checkForSkittleTemp();
    //    if(found)//rename SkittleTemp to Skittle
    if(checkForSkittleTemp())
    {
        //Sleep(5000);
        qDebug("Found Launcher update.");
        system("del Skittle.exe");
        system("rename SkittleTemp.exe Skittle.exe");
    }
    else
        qDebug("Launcher update not found.");
}



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

/** *******************
  This file is the main loop that starts the MainWindow class used in Skittle
  and every other Qt application.
  Argv[1] is passed to window.open() so that Skittle can be used by the OS
  to open FASTA files directly.  This connection is setup by making Skittle
  the associated program with .fa and .fasta file extensions.
  Additionally, main handles the update functionality linked to the Skittle
  Launcher.  Case 1: launcher passes the update argument to SkittleToo.exe and
  the update button is placed in the UI.  Case 2: The launcher places a new
  version of itself in SkittleTemp.exe and needs to be renamed by this program.
**********************/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    checkForLauncherUpdate();//case 2
    window.show();
    if(argc > 1)//case1
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
