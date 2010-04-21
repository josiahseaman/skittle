#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "UiVariables.h"
//#include "MainPanel.h"

class AbstractGraph;
class FastaReader;
class GtfReader;
class GLWidget;

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
class QScrollArea;
class QScrollBar;
class QFrame;
class QLineEdit;
class QTabWidget;
class QCheckBox;

class MainWindow : public QMainWindow
{
friend class ViewManager;

		Q_OBJECT
public:
	QFrame* glHandler;
    FastaReader* fastaReader;
    GtfReader* trackReader;
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
	void addDisplayDivider();
private:
	void createActions();
	void createMenus();
	void createDocks();
	void createContextMenus();
	void createToolbars();
	void createStatusBar();
	void createUiConnections();
	void createFileConnections();
	UiVariables getDisplayVariables();
	bool loadfile(const QString &filename);
	void readSettings();
	void writeSettings();
	
	ViewManager* viewManager;
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
	QSpinBox* similarityDial;
	QLineEdit* seqEdit;
	QDockWidget *filterDock;
    QTabWidget *tabWidget;
    QFrame *highlighterTab;
	
	QSpinBox *widthDial;
	QPushButton *doubleDisplayWidth;
	QPushButton *halveDisplayWidth;
	QSpinBox *scale;
	QSpinBox* zoom;
	QSpinBox *startOffset;
	QSpinBox *displayLength;
	QSpinBox *wordLength;
	QCheckBox* syncCheckBox;
	
	
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
	QAction *addViewAction;
	QAction *openAction;
	QAction *importAction;
	QAction *exitAction;
	
	QLabel *statusTip;
	//QProgressBar *processStatus;
	//QLabel *processState;
	int oldScale;
	
};

#endif

