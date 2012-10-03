#ifndef JUI_VARS
#define JUI_VARS

#include <string>
#include <QObject>

class QTextEdit;
class QSpinBox;

using namespace std;

class UiVariables : public QObject
{
    Q_OBJECT

public:
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;
    QSpinBox* offsetDial;
	QTextEdit* textArea;

//	UiVariables();
    UiVariables(QTextEdit* text = NULL);
    ~UiVariables();

    void print(const char*);
	void print(std::string s);
	void printHtml(std::string);
	void print(const char* s, int num);
	void printNum(int num);
	//void print(int num1, int num2); 

public slots:
    void changeWidth(int newWidth);
    void changeWidth();
    void changeScale(int newScale);
    void changeScale();
    void changeStart(int start);
    void changeZoom(int zoom);
    void changeOffset(int offset);
signals:
    void internalsUpdated();

private:
    int oldScale;
    int oldWidth;
    static int const maxSaneWidth = 4000;


};
#endif
