#ifndef JUI_VARS1234
#define JUI_VARS1234

#include <string>
#include <QObject>

class QTextEdit;
class QSpinBox;

class UiVariables// : public QObject
{
	//Q_OBJECT
		
public:
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;
    QSpinBox* offsetDial;
	QTextEdit* textArea;

	UiVariables();
	UiVariables(QTextEdit*);
	UiVariables(const UiVariables& copy);


	void print(const char*);
	void print(std::string s);
	void printHtml(std::string);
	void print(const char* s, int num);
	void printNum(int num);
	//void print(int num1, int num2); 

};
#endif
