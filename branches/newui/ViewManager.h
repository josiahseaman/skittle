//ViewManager.h
#ifndef VIEWMANAGER
#define VIEWMANAGER

#include <QtCore>
#include <QMdiArea>
#include <vector>
#include "UiVariables.h"

class GLWidget;
class MainWindow;
class QHBoxLayout;

class ViewManager : public QMdiArea//QFrame//
{
	Q_OBJECT

public:
	GLWidget* glWidget;
	//GLWidget* glWidget2;
	
	ViewManager(MainWindow* window, UiVariables gui);
	void createConnections();
	void uiToGlwidgetConnections(GLWidget* active);
	
public slots:
	GLWidget* addNewView();
	void changeSelection(GLWidget*);
	void changeFile(QString);
	void addAnnotationDisplay(QString);
	void addBookmark();
	
	
	
private:
	std::vector<GLWidget*> views;
	MainWindow* mainWindow;
	GLWidget* activeWidget;
	UiVariables ui;	
};



#endif
