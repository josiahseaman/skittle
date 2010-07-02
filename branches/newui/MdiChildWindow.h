#ifndef MDI_CHILD_SKITTLE
#define MDI_CHILD_SKITTLE

#include <QMdiArea>
#include <QFrame>
#include <vector>
#include "UiVariables.h"

class GLWidget;
class MainWindow;
class QHBoxLayout;
class QTabWidget;
class QFrame;
class QScrollArea;

using std::vector;

class MdiChildWindow : public QFrame
{
	Q_OBJECT

public:

	QScrollBar* horizontalScrollBar;
	QScrollBar* verticalScrollBar;
	QFrame* subFrame;
	GLWidget* glWidget;
	UiVariables ui;
	
	MdiChildWindow(UiVariables gui, QSpinBox* pStart, QTabWidget* settings);
	void closeEvent(QCloseEvent *event);
	//createConnections();
	void connectWidget();
	void hideSettingsTabs();
	void showSettingsTabs();
	void mousePressEvent(QMouseEvent *event);
	
public slots:
	void changeLocalStartFromPublicStart(int val);
	void changeLocalStartFromOffset(int val);
	void setHorizontalWidth(int val);
	void setOffsetStep(int val);
	void setPageSize();
	void hideSettingsTab(QScrollArea*);
	void showSettingsTab(QScrollArea*);

signals:
	void subWindowClosing(MdiChildWindow*);
	void IveBeenClicked(MdiChildWindow*);

private:
	QSpinBox* publicStart;
	QTabWidget* settingsDock;
	vector<QScrollArea*> settingsTabs;
	
	void createSettingsTabs();
};	
#endif
