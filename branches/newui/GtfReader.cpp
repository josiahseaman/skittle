#include <QtGui>
#include "GtfReader.h"
#include "NucleotideDisplay.h"
#include "BasicTypes.h"
#include "UiVariables.h"
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

GtfReader::GtfReader(UiVariables gui)
	:ui(gui)
{	
	//ui = gui;
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
	ss << ui.startDial->value();
	dialog.start->setText( QString( ss.str().c_str() ) );
	std::stringstream ss2;
	ss2 << ui.startDial->value() + ui.sizeDial->value();
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
			outFile.close();
		
			track_entry entry = track_entry(dialog.start->text().toInt(), dialog.end->text().toInt(), color_entry(), dialog.note->toPlainText().toStdString());
			emit BookmarkAdded(entry, outputFilename);	
		}
		else
		{
			ErrorBox msg("Could not read the file.");
			outFile.close();
		}
	}
}

void GtfReader::determineOutputFile(QString file)
{
	string filename = file.toStdString();
	outputFilename = filename;
	outputFilename.append("-skittle_notes.gtf");
	chrName = trimFilename(filename);
}

string GtfReader::trimFilename(string path)
{
	int startI = path.find_last_of('/');
	int endI = path.size();//path.find_last_of('.');//
	int sizeI = endI - startI;
	return path.substr(startI+1, sizeI-1); 
}

vector<track_entry>  GtfReader::readFile(QString filename)
{
	vector<track_entry> annotation_track;
	inputFilename = filename.toStdString();
	if( inputFilename.empty() || !initFile(inputFilename) )
	{
		return vector<track_entry>();
	}
	
	annotation_track.clear();
	
	srand(time(0));
	string line;
	while( getline(file, line) )
	{
		line.erase(line.size()-1);//erase last character, should be a newline character
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
	return annotation_track;
}
	
/*PRIVATE FUNCTIONS*/

/***********OUTPUT ANNOTATED SEQUENCE************** /
void GtfReader::snipAnnotatedSequence()
{
	const string* seq = glWidget->nuc->sequence;
	ofstream fout("clipped.fa");
	for(int i = 0; i < annotation_track.size(); i++)
	{
		fout << seq->substr(annotation_track[i].start, annotation_track[i].stop - annotation_track[i].start);
	}
	fout.close();
}/**/
	
color GtfReader::color_entry()
{
	volatile int r = (int)(((float)rand() / RAND_MAX)* 255);
	volatile int g = (int)(((float)rand() / RAND_MAX)* 255);
	volatile int b = (int)(((float)rand() / RAND_MAX)* 255);
	color c = color(r, g, b);
	return c;
}

bool GtfReader::eq(string& str1, const char* str2)
{
	return str1[0] == str2[0];//strcomp(str1.c_str(), str2) == 0;
}

string GtfReader::outputFile()
{
	return outputFilename;
}
