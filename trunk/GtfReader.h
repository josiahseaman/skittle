#ifndef GTF_READER
#define GTF_READER

#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <QObject>
#include "BasicTypes.h"

using namespace std;

class QProgressDialog;
class Ui_SkittleGUI;

class GtfReader : public QObject
{
	Q_OBJECT
	
public:
	string inputFilename;
	string chrName;
	
	GtfReader(Ui_SkittleGUI* ui);
	const vector<track_entry>& tracks();
	

public slots:
	void readFile(QString name);	
	void addBookmark();//int start, int end);
	void determineOutputFile(QString file);
	void storeChrName(string);

signals:
	void newGtfFileRead(const vector<track_entry>&);
	void progressValueChanged(int start);
	
private:
	bool initFile(string fileName);
	color color_entry();//string repClass);
	bool eq(string& str1, const char* str2);

	Ui_SkittleGUI* ui;
	string outputFilename;
	ifstream file;
	vector<track_entry> annotation_track;
	QProgressDialog* progressBar;
	int bytesInFile;//file size, but more specific
	int blockSize;
};

#endif
