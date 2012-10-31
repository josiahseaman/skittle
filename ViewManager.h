//ViewManager.h
#ifndef VIEWMANAGER
#define VIEWMANAGER

#include <QtCore>
#include <QMdiArea>
#include <vector>
#include "MdiChildWindow.h"
#include "UiVariables.h"

class GLWidget;
class MainWindow;
class QHBoxLayout;

class ViewManager : public QMdiArea
{
    Q_OBJECT

public:
    GLWidget* activeWidget;

    ViewManager(MainWindow* window, UiVariables* gui);
    void createConnections();
    void uiToGlwidgetConnections(GLWidget* active);

signals:

public slots:
    GLWidget* addNewView(bool suppressOpen = false);
    void changeSelection(QMdiSubWindow * container);
    void changeSelection(MdiChildWindow* );
    void closeSubWindow(MdiChildWindow* closing);
    void changeFile(QString);
    void addAnnotationDisplay(QString);
    void jumpToNextAnnotation();
    void jumpToPrevAnnotation();
    void updateCurrentDisplay();

private:
    std::vector<MdiChildWindow*> views;
    MainWindow* mainWindow;
    UiVariables* ui;

    bool newOffsetDial(GLWidget *gl);
    UiVariables* vars(GLWidget* active);

};



#endif
