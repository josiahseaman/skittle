#ifndef JUI_VARS
#define JUI_VARS

#include <string>
#include <QWidget>
#include <map>

class QTextEdit;
class QSpinBox;
class GLWidget;

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
    QTextEdit* textArea;
    int colorSetting;
    enum colorPalletes { CLASSIC, COLORBLINDSAFE, BETTERCBSAFE, DARK, DRUMS, BLUES, REDS };

    static UiVariables* Instance();

    int newOffsetDial(GLWidget* gl);
    QSpinBox* getOffsetDial(GLWidget* gl);

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
    void diffOffset(GLWidget *gl, int deltaO);
    void changeColorSetting(int set);
signals:
    void internalsUpdated();
    void colorsChanged(int);

private:
    //	UiVariables();
    UiVariables(QTextEdit* text = NULL);
    UiVariables(UiVariables const&){}             // copy constructor is private
    UiVariables& operator = (UiVariables const&){return *Instance();}  // assignment operator is private

    ~UiVariables();
    static UiVariables* pointerInstance;
    map<GLWidget*, QSpinBox*> offsets;
    int oldScale;
    int oldWidth;
    static int const maxSaneWidth = 4000;


};
#endif
