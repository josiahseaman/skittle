#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "UiVariables.h"
//#include "MainPanel.h"

class AbstractGraph;
class QAction;
class QLabel;
class QActionGroup;
class QDockWidget;
class QTextEdit;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QPushButton;
class QProgressBar;
class FastaReader;
class GtfReader;
class GLWidget;
class QScrollArea;
class QScrollBar;
class QFrame;

class MainWindow : public QMainWindow
{
		Q_OBJECT
public:
    GLWidget* glWidget;
    FastaReader* fastaReader;
    GtfReader* trackReader;
	QScrollBar* horizontalScrollBar;
	QScrollBar* verticalScrollBar;
	MainWindow();


signals:
	void newFileOpen(QString name);
	void newGtfFileOpen(QString name);
	
protected:
	void closeEvent(QCloseEvent *event);
	
public slots:
	void open();
	void open(QString fileName);
	void updateProgress(int);
	void updateState(QString);
	void updateStatus(QString);
	void changeWindowName(std::string name);
	void openGtf();
	void changeScale(int newScale);
	void doubleWidth();
	void halveWidth();
	void addDisplayActions(AbstractGraph*);
	
private:
	void createActions();
	void createMenus();
	void createDocks();
	void createContextMenus();
	void createToolbars();
	void createStatusBar();
	void createConnections();
	UiVariables getDisplayVariables();
	bool loadfile(const QString &filename);
	void readSettings();
	void writeSettings();
	
	QFrame* scrollArea;//QScrollArea
	QMenu *annotationMenu;
	QMenu *fileMenu;
	QMenu *presetMenu;
	QMenu *searchMenu;
	QMenu *toolMenu;
	QMenu *viewMenu;
	QMenu *toolBarMenu;
	
	QActionGroup *toolActionGroup;
	
	QToolBar *annotationToolBar;
	QToolBar *presetToolBar;
	QToolBar *toolToolBar;
	QToolBar *settingToolBar;
	
	QDockWidget *infoDock;
	QTextEdit* textArea;
	QDockWidget *filterDock;
	
	QSpinBox *widthDial;
	QPushButton *doubleDisplayWidth;
	QPushButton *halveDisplayWidth;
	QSpinBox *scale;
	QSpinBox* zoom;
	QSpinBox *startOffset;
	QSpinBox *displayLength;
	QSpinBox *wordLength;
	
	
	QAction *moveAction;
	QAction *resizeAction;
	QAction *zoomAction;
	QAction *selectAction;
	QAction *findAction;
	QAction *addAnnotationAction;
	QAction *nextAnnotationAction;
	QAction *prevAnnotationAction;
	QAction *browseCommunityAction;
	QAction *delAnnotationAction;
	QAction *findSequenceAction;
	QAction *findNextAction;
	QAction *findPrevAction;
	QAction *hilightResultsAction;
	QAction *openAction;
	QAction *importAction;
	QAction *exitAction;
	
	QLabel *statusTip;
	//QProgressBar *processStatus;
	//QLabel *processState;
	int oldScale;
	
};

#endif

