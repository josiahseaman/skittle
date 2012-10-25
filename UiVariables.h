#ifndef JUI_VARS
#define JUI_VARS

#include <string>
#include <QWidget>

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
    int colorSetting;
    enum colorPalletes { CLASSIC, COLORBLINDSAFE, BETTERCBSAFE, DRUMS, BLUES, REDS };

    //	UiVariables();
    UiVariables(QTextEdit* text = NULL);
    ~UiVariables();

    void print(const char*);
    void print(std::string s);
    void printHtml(std::string);
    void print(const char* s, int num);
    void printNum(int num);
    //void print(int num1, int num2);
    int getColorSetting();

public slots:
    void changeWidth(int newWidth);
    void changeWidth();
    void changeScale(int newScale);
    void changeScale();
    void changeStart(int start);
    void changeZoom(int zoom);
    void changeOffset(int offset);
    void changeColorSetting(int set);
signals:
    void internalsUpdated();
    void colorsChanged(int);

private:
    int oldScale;
    int oldWidth;
    static int const maxSaneWidth = 4000;


};
#endif
