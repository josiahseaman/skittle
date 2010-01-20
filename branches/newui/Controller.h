#ifndef Controller_H
#define Controller_H

#include <QWidget>
#include "MainWindow.h"
#include "BorderLayout.h"

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

class GLWidget;
class Window;
class FastaReader;
class GtfReader;
using std::string;

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller(Window* win);
	UiVariables* ui;

public slots:
	void changeScale(int newScale);
	void doubleWidth();
	void halveWidth();


signals:
     
private:
	Window* window;
    GLWidget* glWidget;
    int scale;
};


#endif
