//ViewManager.h
#ifndef VIEWMANAGER
#define VIEWMANAGER

#include <QtCore>
#include <QFrame>
#include "UiVariables.h"

class GLWidget;

class ViewManager : public QFrame
{
	Q_OBJECT

public:
	ViewManager(UiVariables gui);

	GLWidget* glWidget;
	GLWidget* glWidget2;
	
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
	QScrollBar* horizontalScrollBar;
	QScrollBar* verticalScrollBar;
	//	vector<GLWidget*> glwidgets;

	void connectWidget();
	void disconnectWidget();
	
};



#endif
