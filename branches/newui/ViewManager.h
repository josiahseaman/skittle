//ViewManager.h
#ifndef VIEWMANAGER
#define VIEWMANAGER

#include <QtCore>
#include <QFrame>
#include <vector>
#include "UiVariables.h"

class GLWidget;
class MainWindow;
class QHBoxLayout;

class ViewManager : public QFrame
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
	void setPageSize();
	void setHorizontalWidth(int);
	void addAnnotationDisplay(QString);
	void addBookmark();
	
	
	
private:
	std::vector<GLWidget*> views;
	MainWindow* mainWindow;
	GLWidget* activeWidget;
	UiVariables ui;
	QHBoxLayout* hLayout;
	QScrollBar* horizontalScrollBar;
	QScrollBar* verticalScrollBar;
	//	vector<GLWidget*> glwidgets;

	void connectWidget();
	void disconnectWidget();
	
};



#endif
