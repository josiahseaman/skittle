#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "UiVariables.h"

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

class MainWindow : public QMainWindow
{
		Q_OBJECT
public:
    GLWidget* glWidget;
    FastaReader* fastaReader;
    GtfReader* trackReader;
	MainWindow();


signals:
	void newFileOpen(QString name);
	void newGtfFileOpen(QString name);
	
//protected:
	//void closeEvent(QCloseEvent *event);
	
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
	
private:
	void createActions();
	void createMenus();
	void createDocks();
	void createContextMenus();
	void createToolbars();
	void createStatusBar();
	void MainWindow::createConnections();
	bool loadfile(const QString &filename);
	UiVariables getDisplayVariables();
	
	QScrollArea* scrollArea;
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
	QAction *presetCylinderAction;
	QAction *presetNucleotideAction;
	QAction *presetLocalAlignmentAction;
	QAction *presetFrequencyMapAction;
	QAction *presetBiasFrequencyAction;
	QAction *openAction;
	QAction *importAction;
	QAction *exitAction;
	
	QLabel *statusTip;
	QProgressBar *processStatus;
	QLabel *processState;
	int oldScale;
	
};

#endif

