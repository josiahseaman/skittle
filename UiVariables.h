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
    QTextEdit* textArea;
    int colorSetting;
    enum colorPalletes { CLASSIC, COLORBLINDSAFE, BETTERCBSAFE, DARK, DRUMS, BLUES, REDS };

    static UiVariables* Instance();

    void newOffsetDial(GLWidget* gl);

    void print(const char*);
    void print(std::string s);
    void printHtml(std::string);
    void print(const char* s, int num);
    void printNum(int num);
    bool valueIsGoingToChange(QSpinBox* dial, int val);
    int getColorSetting();
    vector<QSpinBox *> getDialPointers();

public slots:
    void setAllVariables(int width, int scale, int zoom, int start, int size);
    int getWidth();
    void setWidth(int newWidth);
    int getScale();
    void setScale(int newScale);
    int getStart(GLWidget* gl);
    void setStart(GLWidget *saysWho, int start);
    int getZoom();
    void setZoom(int zoom);
    int getSize();
    void setSize(int size);

    QSpinBox* getOffsetDial(GLWidget* gl);
    void setOffsetDelta(GLWidget *gl, int deltaO);
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

    QSpinBox* widthDial;
    QSpinBox* scaleDial;
    QSpinBox* zoomDial;
    QSpinBox* startDial;
    QSpinBox* sizeDial;

};
#endif
