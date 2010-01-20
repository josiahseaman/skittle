#include <QtGui>
#include "GtfReader.h"
#include "NucleotideDisplay.h"
#include "BasicTypes.h"
#include "SkittleUi.h"
#include "ui_BookmarkDialog.h"

#include <QDebug>
#include <QThread>
#include <QString>
#include <qtconcurrentrun.h>
#include <QApplication>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include <stdlib.h>
#include <ctime>

using namespace QtConcurrent;
using namespace std;

GtfReader::GtfReader(Ui_SkittleGUI* gui)
{	
	ui = gui;
	inputFilename = string("blank.fa");
	outputFilename = string("user.gtf");
	bytesInFile = 0;
	blockSize = 1000000;
}

bool GtfReader::initFile(string fileName)
{
	file.clear();//reset the fail state to normal
	file.open (fileName.c_str(), ifstream::in | ifstream::binary);
	if(file.fail())
	{
		ErrorBox msg("Could not read the file.");
		return false;
	}
	int begin = file.tellg();
	file.seekg (0, ios::end);
	int end = file.tellg();
	bytesInFile = end - begin;
	file.seekg(0, ios::beg);//move pointer to the beginning of the file
	
	return true;
}

/**SLOTS**/
void GtfReader::addBookmark()//int start, int end)
{
	QDialog parent;
	Ui_BookmarkDialog dialog;
	dialog.setupUi(&parent);
	
	std::stringstream ss;
	ss << ui->startDial->value();
	dialog.start->setText( QString( ss.str().c_str() ) );
	std::stringstream ss2;
	ss2 << ui->startDial->value() + ui->sizeDial->value();
	dialog.end->setText( QString(ss2.str().c_str() ) );
	dialog.sequence->setText( QString(chrName.c_str()) );
	
	parent.show();
	int result = parent.exec();
	if(result == QDialog::Accepted)
	{
		ofstream outFile;
		outFile.open(outputFilename.c_str(), ios::app);
		if(!outFile.fail())
		{
			outFile << dialog.sequence->text().toStdString()<<"\t";
			outFile << dialog.source->text().toStdString()<<"\t";
			outFile << dialog.feature->text().toStdString()<<"\t";
			outFile << dialog.start->text().toStdString() <<"\t";
			outFile << dialog.end->text().toStdString() << "\t";
			outFile << dialog.score->text().toStdString() << "\t";
			outFile << (dialog.strand->isChecked() ? "+" : "-") << "\t";//QCheckBox
			outFile << dialog.frame->currentText().toStdString() << "\t";//QComboBox
			outFile << dialog.note->toPlainText().toStdString() << "\t";//QPlainTextEdit
			outFile << "\n";
		
		}
		else
		{
			ErrorBox msg("Could not read the file.");
		}
		
		outFile.close();
	}
}

void GtfReader::determineOutputFile(QString file)
{
	string filename = file.toStdString();
	
	outputFilename = filename;
	outputFilename.append("-skittle_notes.gtf");
}

void GtfReader::storeChrName(string seqName)
{
	chrName = seqName;
}

/*PRIVATE FUNCTIONS*/

void GtfReader::readFile(QString filename)
{
	inputFilename = filename.toStdString();
	if( inputFilename.empty() || !initFile(inputFilename) )
	{
		return;
	}
	
	annotation_track.clear();
	
	srand(time(0));
	string line;
	while( getline(file, line) )
	{
		stringstream lineStr( line );
		int start = 0;
		int stop = 0;
		//string repClass;
		
		lineStr.ignore(10000, '\t');//1
		lineStr.ignore(10000, '\t');//2
		lineStr.ignore(10000, '\t');//3
		/*
		lineStr.ignore(10000, '\t');//4
		lineStr.ignore(10000, '\t');//5
		lineStr.ignore(10000, '\t');//6*/
		//getline(lineStr, repClass, '\t');//repClass - type
		lineStr >> start >> stop;//genoStart	//genoEnd	
		color c = color_entry();//repClass);
		if(!lineStr.fail())
		{
			annotation_track.push_back( track_entry(start, stop, c, line) );
			int last_entry = annotation_track.size() -1;
			annotation_track[last_entry].index = last_entry;
		}
	}	
	file.close();	
	emit newGtfFileRead( tracks() );
	
	/***********OUTPUT ANNOTATED SEQUENCE************** /
	const string* seq = ui->glWidget->disp->sequence;
	ofstream fout("clipped.fa");
	for(int i = 0; i < annotation_track.size(); i++)
	{
		fout << seq->substr(annotation_track[i].start, annotation_track[i].stop - annotation_track[i].start);
	}
	fout.close();*/
}
	
color GtfReader::color_entry()//string repClass)
{
	volatile int r = (int)(((float)rand() / RAND_MAX)* 255);
	volatile int g = (int)(((float)rand() / RAND_MAX)* 255);
	volatile int b = (int)(((float)rand() / RAND_MAX)* 255);
	color c = color(r, g, b);
	//rand();
	/*
	short int rC=0;
	if( eq(repClass, "LINE") )//it is a LINE
		c = color(0,0,255);
	else if( eq(repClass, "LTR") )//it is a LTR	
		c = color(255,0,0);
	else if( eq(repClass, "SINE") )//it is a SINE		
		c = color(0,255,0);
	else
	{
		rC = (short int)repClass[0];
		if(rC > 90) rC-= 32; 
		rC -= 64;//drop it to 0-26
		//rC = rC;//invert
		rC *= 36;//drop it to 0-234 range
		c = color(rC, 0, 0);//spectrum(rC);
		//c = c * .4;
	}*/
	return c;
}

bool GtfReader::eq(string& str1, const char* str2)
{
	return str1[0] == str2[0];//strcomp(str1.c_str(), str2) == 0;
}

const vector<track_entry>& GtfReader::tracks()
{
	return annotation_track;
}
