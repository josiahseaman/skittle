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

class ViewManager : public QMdiArea//QFrame//
{
	Q_OBJECT

public:
	GLWidget* glWidget;
	
	ViewManager(MainWindow* window, UiVariables gui);
	void createConnections();
	void connectLocalCopy(GLWidget* active, UiVariables local);
	void uiToGlwidgetConnections(GLWidget* active);
	
public slots:
	GLWidget* addNewView();
	void changeSelection(GLWidget*);
	void changeFile(QString);
	void addAnnotationDisplay(QString);
	void addBookmark();
	void handleWindowSync();
	
	
private:
	std::vector<MdiChildWindow*> views;
	MainWindow* mainWindow;
	GLWidget* activeWidget;
	UiVariables ui;	
	
	void broadcastLocalValues(UiVariables local);
	void broadcastPublicValues(UiVariables local);
	UiVariables copyUi();
	void connectVariables(GLWidget*, UiVariables);
	void disconnectVariables(GLWidget*, UiVariables);
	UiVariables vars(GLWidget* active);
	
};



#endif
