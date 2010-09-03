#ifndef GEN_READER
#define GEN_READER

#include <stdlib.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <QObject>

#include <QtGui>
#include <QRunnable>
#include <qtconcurrentrun.h>
#include <QApplication>
//#include "UiVariables.h"

using namespace std;

class GLWidget;
class UiVariables;

struct to_upper {
  int operator() ( int ch )
  {
    return std::toupper ( ch );
  }
};

class FastaReader : public QObject
{
	Q_OBJECT
	
public:
	
	FastaReader(GLWidget* gl, UiVariables* gui);
	~FastaReader();
	const string* seq();
	string trimFilename(string path);

public slots:
	void readFile(QString name);	

signals:
	void fileNameChanged(string name);
	void newFileRead(const string*);
	void progressValueChanged(int start);
	
private:
	GLWidget* glWidget;
	UiVariables* ui;
	char upperCase(char& c);
	bool initFile(string file);
	void loadingProgress();
	int readBlock(int &start);
	int readBlock2(int &start);
	int readBlock3(int &start);
	void storeChrName(string n);
	string logo();

	ifstream wordfile;
	string sequence;
	vector<char> buffer;
	QProgressDialog* progressBar;
	int bytesInFile;//file size, but more specific
	int blockSize;
};

class ProgressBar : public QRunnable//, public QObject
{
public:
	FastaReader* fasta;
	ProgressBar(FastaReader* f)
	{
		fasta = f;
	}
	void run()
	{
		// Create a progress dialog.
		QProgressDialog* dialog  = new QProgressDialog("Reading File...", 0, 0, 100);
		//dialog->setLabelText(QString("Reading using %1 thread(s)...").arg(QThread::idealThreadCount()));
		//QObject::connect(fasta, SIGNAL(progressValueChanged(int)), dialog, SLOT(setValue(int)));
	    
		dialog->exec();
		/*
		while( dialog->value() < 90)
		{
			;//wait for file to finish reading (we don't always get a 100% signal
		}*/
	}
};

#endif
