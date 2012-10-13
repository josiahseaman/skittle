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
	void changeSelection(MdiChildWindow*);
	void closeSubWindow(MdiChildWindow* closing);
	void changeFile(QString);
	void addAnnotationDisplay(QString);
	void addBookmark();
    void jumpToNextAnnotation();
    void jumpToPrevAnnotation();

    void handleWindowSync();
    void changeGlobalStart();
    void changeAllLocalStarts();
	void printNum(int num);
	void printNum2(int num);
    void updateCurrentDisplay();
	
	
private:
	std::vector<MdiChildWindow*> views;
	MainWindow* mainWindow;
    UiVariables* globalUi;
	
	void broadcastLocalValues(UiVariables local);
    void broadcastPublicValues(UiVariables *local);
    UiVariables* copyUi();
    void connectGlobalPushToLocal(GLWidget *, UiVariables*);//all windows are listening with syncCheckbox
    void connectLocalPushToGlobal(GLWidget *, UiVariables*);//only active window gets to push to globalUi
    void disconnectGlobalPushToLocal(GLWidget *, UiVariables*);
    void disconnectLocalPushToGlobal(GLWidget *, UiVariables*);
    UiVariables* vars(GLWidget* active);
	
};



#endif
