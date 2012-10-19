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

public slots:
    bool readFile(QString name);
    void cancel();

signals:
    void fileNameChanged(string name);
    void newFileRead(const string*);

private:
    GLWidget* glWidget;
    UiVariables* ui;
    char upperCase(char& c);
    void storeChrName(string n);
    void setupProgressBar();
    string logo();

    ifstream wordfile;
    string sequence;
    QProgressDialog* progressBar;
    int bytesInFile;//file size, but more specific

    bool cancelled;
};

#endif
