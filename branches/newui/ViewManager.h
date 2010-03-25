//ViewManager.h
#ifndef VIEWMANAGER
#define VIEWMANAGER

#include <QtCore>
#include "UiVariables.h"

class GLWidget;

class ViewManager : public QObject
{
	Q_OBJECT

public:
	ViewManager(UiVariables gui);

public slots:
	void changeSelection(GLWidget*);
	void changeFile(QString);
	void setPageSize();
	void setHorizontalWidth(int);
	void addAnnotationDisplay(QString);
	void addBookmark();
	
	
	
private:
	GLWidget* activeWidget;
	UiVariables ui;
	
	void connectWidget();
	void disconnectWidget();
	
};



#endif
