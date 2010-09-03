#ifndef GTF_READER
#define GTF_READER

#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <QObject>
#include "BasicTypes.h"
#include "UiVariables.h"

using namespace std;

class QProgressDialog;

class GtfReader : public QObject
{
	Q_OBJECT
	
public:
	string inputFilename;
	string chrName;
	
	GtfReader(UiVariables ui);
	vector<track_entry> readFile(QString name);
	string outputFile();
	string trimFilename(string path);

public slots:
	void addBookmark();
	void determineOutputFile(QString file);

signals:
	void newGtfFileRead(const vector<track_entry>&);
	void progressValueChanged(int start);
	void BookmarkAdded(track_entry, string);
	
private:
	bool initFile(string fileName);
	color color_entry();
	bool eq(string& str1, const char* str2);

	UiVariables ui;
	string outputFilename;
	ifstream file;
	QProgressDialog* progressBar;
	int bytesInFile;//file size, but more specific
	int blockSize;
};

#endif
