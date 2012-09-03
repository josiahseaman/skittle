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
	
	ViewManager(MainWindow* window, UiVariables gui);
	void createConnections();
	void connectLocalCopy(GLWidget* active, UiVariables local);
	void uiToGlwidgetConnections(GLWidget* active);

signals:
	
public slots:
	GLWidget* addNewView(bool suppressOpen = false);
	void changeSelection(MdiChildWindow*);
	void closeSubWindow(MdiChildWindow* closing);
	void changeFile(QString);
	void addAnnotationDisplay(QString);
	void addBookmark();
	void handleWindowSync();	
	void changePublicStart(int val);
	void printNum(int num);
	void printNum2(int num);
	void updateCurrentDisplay();
    void scaleChangedSmart(int);
	
	
private:
	std::vector<MdiChildWindow*> views;
	MainWindow* mainWindow;
	UiVariables ui;	
	
	void broadcastLocalValues(UiVariables local);
	void broadcastPublicValues(UiVariables local);
	UiVariables copyUi();
//	void connectOffset(GLWidget* active, UiVariables local);
//	void disconnectOffset(GLWidget* active, UiVariables local);
	void connectVariables(GLWidget*, UiVariables);
	void disconnectVariables(GLWidget*, UiVariables);
	UiVariables vars(GLWidget* active);
	
};



#endif
