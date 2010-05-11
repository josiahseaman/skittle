#ifndef JUI_VARS
#define JUI_VARS

#include <string>

class QTextEdit;
class QSpinBox;

class UiVariables
{
public:
    QSpinBox* sizeDial;
    QSpinBox* widthDial;
    QSpinBox* startDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;
    QSpinBox* offsetDial;

	//UiVariables();
	UiVariables(QTextEdit*);
	//UiVariables(const UiVariables& copy);
	void print(const char*);
	//void print(std::string s);
	void printHtml(std::string);
	//void print(const char* s, int num);
	//void print(int num1, int num2); 
private: 
	QTextEdit* textArea;
};
#endif
