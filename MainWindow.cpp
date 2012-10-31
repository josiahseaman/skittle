#include <QtGui>
#include <QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QCheckBox>
#include <QtCore/QProcess>
#include <QtCore/QStringList>

#include <iostream>
#include <string>
#include <sstream>

#include "MainWindow.h"
#include "FastaReader.h"
#include "GtfReader.h"
#include "glwidget.h"
#include "AbstractGraph.h"
#include "NucleotideDisplay.h"
#include "RepeatMap.h"
#include "AnnotationDisplay.h"
#include "CylinderDisplay.h"
#include "RepeatOverviewDisplay.h"
#include "OligomerDisplay.h"
#include "HighlightDisplay.h"
#include "ViewManager.h"
#include "TextureCanvas.h"

/** *******************************************
MainWindow is the primary UI frame for Qt Widgets.  It is the master window
that the OS interfaces with.  It contains a series of QToolBar with the UI elements
that can be moved around by the user (Visualization Graphs, Mouse Tools,
Sequence View Settings, File Actions, Information Display).
These same buttons are tied to actions which can be triggered from the standard window
menu bar in all applications.  The arrangement of buttons on the screen is remembered
between instances of the program as a user preference.

In addition to the actions, buttons and layout, all the logic for internal connections
is within: createUiConnections(), createFileConnections() and addDisplayActions().

Window Hierarchy:
MainWindow -> (1) Viewmanager -> (many) MdiChildWindow -> (1) GLWidget -> (1)FastaReader -> (1)File
MainWindow is at the top of a window hierarchy.  The center widget of MainWindow is a single
ViewManager which inherits from MdiArea.  The multiple document interface (MDI) can have multiple
MdiChildWindows.  Each MdiChildWindow has only one GLWidget tied to one file.  MainWindow is
the primary owner of the UiVariables object that is passed for signals all throughout the program.
********************************************/

MainWindow::MainWindow()
{
    textArea = NULL;
    setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
    setWindowTitle( "Skittle Genome Visualizer");
    createDocks();
    createUiVars();
    createActions();
    createMenus();
    createToolbars();
    createStatusBar();
    createUiConnections();

    readSettings();
    viewManager	= new ViewManager(this, ui);
    setCentralWidget(viewManager);
    createFileConnections();

    ensureDocksAreVisible();
    setWindowIcon(QIcon(":/skittle.svg"));

    //openAction->trigger();
}

void MainWindow::addDisplayActions(AbstractGraph* display)
{
    if(display->actionLabel.size() > 0)
    {
        QAction* presetAction = new QAction(QString(display->actionLabel.c_str()),this);
        presetAction->setStatusTip(QString(display->actionTooltip.c_str()));
        presetAction->setToolTip(QString(display->actionTooltip.c_str()));
        //presetAction->setData(QString(display->actionData.c_str()));
        display->toggleButton = presetAction;
        connect(presetAction,SIGNAL(triggered()),display, SLOT(toggleVisibility()));
        connect(display, SIGNAL(deleteButton(QAction*)), this, SLOT(removeGraphButton(QAction*)));

        //this section of code inserts a new displayAction at the beginning of the list, rather than appending it
        //this is for the sake of the AnnotationDisplays which are added last and listed first
        presetMenu->addAction(presetAction);
        QAction* before = NULL;
        QList<QAction*> list = presetToolBar->actions();
        if(!list.isEmpty())
            before = list.first();
        presetToolBar->insertAction(before, presetAction);
        display->setButtonFont();
    }
    else
    {
        print("Tried to add display mode with no label, aborting...");
    }
}

void MainWindow::showUpdateButton()
{
    updateSkittle->setVisible(true);
    connect(updateSkittle, SIGNAL(triggered()), this, SLOT(closeAndUpdateSkittle()));
}

void MainWindow::closeAndUpdateSkittle()
{
    print("Update command received.");
    QString program = QString("Skittle.exe");
    QStringList arguments;
    arguments<<"update";

    /* Create a QProcess instance. It does not matter if it is created on the stack or
         * on the heap. - Ahem, I tested it on Linux only. :-)*/
    QProcess* updateProcess = new QProcess();
    updateProcess->start(program, arguments);
    this->close();
}

void MainWindow::removeGraphButton(QAction* presetAction)
{
    presetMenu->removeAction(presetAction);
    presetToolBar->removeAction(presetAction);
}

void MainWindow::addDisplayDivider()
{
    presetToolBar->addSeparator();
}

void MainWindow::createUiVars()
{
    settingToolBar = addToolBar("Sequence View Settings");
    settingToolBar->setObjectName("Sequence View Settings");

    QFont boldFont = QFont();
    boldFont.setBold(true);
    QLabel* activeW = new QLabel("Active Window:");
    activeW->setFont(boldFont);
    settingToolBar->addWidget(activeW);
    settingToolBar->addSeparator();


    //Note:Creating the visual representation of UiVariables is split between Mainwindow and the UiVariables constructor
    ui = UiVariables::Instance();
    ui->textArea = textArea;
    vector<QSpinBox*> dials = ui->getDialPointers();
    int i = 0;
    settingToolBar->addWidget(new QLabel("Width"));
    settingToolBar->addWidget(dials[i++]);

    doubleDisplayWidth = new QPushButton("x2",this);
    settingToolBar->addWidget(doubleDisplayWidth);
    halveDisplayWidth = new QPushButton("/2",this);
    settingToolBar->addWidget(halveDisplayWidth);
    zoomExtents = new QPushButton("Scale to \nChromosome",this);
    settingToolBar->addWidget(zoomExtents);

    settingToolBar->addWidget(new QLabel("Scale"));
    settingToolBar->addWidget(dials[i++]);

    settingToolBar->addWidget(new QLabel("Zoom"));
    settingToolBar->addWidget(dials[i++]);

    settingToolBar->addWidget(new QLabel("Start Index"));
    settingToolBar->addWidget(dials[i++]);

    settingToolBar->addWidget(new QLabel("Display Length"));
    settingToolBar->addWidget(dials[i++]);


    //settingToolBar->addSeparator();

    QLabel* multiW = new QLabel("Multiple Windows:");
    multiW->setFont(boldFont);
    settingToolBar->addWidget(multiW);
    settingToolBar->addSeparator();
    //QActionGroup* multiGroup = new QActionGroup(this);
    settingToolBar->addWidget( new QLabel("Offsets:"));

}

void MainWindow::createActions()
{
    moveAction = new QAction("Move",this);
    moveAction->setToolTip(QString("Horizontal and vertical scroll"));
    resizeAction = new QAction("Resize",this);
    resizeAction->setToolTip(QString("Adjust Width and Start position"));
    zoomAction = new QAction("Zoom",this);
    zoomAction->setToolTip(QString("Shift+Click to zoom out"));
    selectAction = new QAction("Select",this);
    selectAction->setToolTip(QString("Displays index and sequence information"));
    addAnnotationAction = new QAction("Annotate",this);
    addAnnotationAction->setToolTip(QString("Select a region and enter text to create a new annotation in ..skittle_notes.gtf"));
    findAction = new QAction("&Find",this);
    screenCaptureAction = new QAction("Screen &Capture", this);
    screenCaptureAction->setToolTip(QString("Take a screen shot"));
    nextAnnotationAction = new QAction("Next Annotation",this);
    prevAnnotationAction = new QAction("Previous Annotation",this);
    //browseCommunityAction = new QAction("Browse Community Research",this);
    //delAnnotationAction = new QAction("Delete Current Bookmark",this);

    /*****TODO: NOT CURRENTLY IN USE ********/
    findSequenceAction = new QAction("Find Sequence",this);
    findSequenceAction->setStatusTip("Find Arbitrary Sequence");
    findNextAction = new QAction("Find Next", this);
    findNextAction->setStatusTip("Jump to Next Instance of Current Sequence");
    findPrevAction = new QAction("Find Previous", this);
    findPrevAction->setStatusTip("Jump to Previous Instance of Current Sequence");

    QIcon uIcon = QIcon(":/updatebutton.png");
    updateSkittle =new QAction(uIcon, QString("Click here to update"), this);
    updateSkittle->setVisible(false);

    addViewAction = new QAction("New Window",this);

    openAction = new QAction("&Open File",this);
    openAction->setStatusTip("Open a Sequence File");

    openGtfAction = new QAction("Open Annotation",this);
    openGtfAction->setStatusTip("Open GTF / GFF Annotation File");

    exitAction = new QAction("E&xit",this);
    helpAction = new QAction("Online &Help",this);
    aboutQtAct = new QAction("About Qt", this);
    exitAction->setStatusTip("Close Program");
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAction);
    fileMenu->addAction(addViewAction);
    fileMenu->addSeparator();
    fileMenu->addAction(openGtfAction);
    fileMenu->addAction(screenCaptureAction);
    fileMenu->addAction(exitAction);
    /*searchMenu = menuBar()->addMenu("&Search");
    searchMenu->addAction(findSequenceAction);
    searchMenu->addAction(findNextAction);
    searchMenu->addAction(findPrevAction);*/
    viewMenu = menuBar()->addMenu("&View");
    presetMenu = viewMenu->addMenu("Visualization Graphs");

    annotationMenu = menuBar()->addMenu("&Annotations");
    annotationMenu->addAction(nextAnnotationAction);
    annotationMenu->addAction(prevAnnotationAction);
    //annotationMenu->addAction(browseCommunityAction);
    //annotationMenu->addAction(delAnnotationAction);

    toolMenu = menuBar()->addMenu("&Mouse Tools");
    toolMenu->addAction(moveAction);
    toolMenu->addAction(resizeAction);
    toolMenu->addAction(zoomAction);
    toolMenu->addAction(selectAction);
    toolMenu->addAction(findAction);
    toolMenu->addAction(addAnnotationAction);
    toolActionGroup = new QActionGroup(this);
    toolActionGroup->addAction(moveAction);
    toolActionGroup->addAction(resizeAction);
    toolActionGroup->addAction(zoomAction);
    toolActionGroup->addAction(selectAction);
    toolActionGroup->addAction(findAction);
    toolActionGroup->addAction(addAnnotationAction);

    colorSettingsMenu = menuBar()->addMenu("Color &Settings");
    colorGroup = new QActionGroup( this );
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(colorSelected(int)));
    connect(this, SIGNAL(colorSelected(int)), ui, SLOT(changeColorSetting(int)));

    createColorPalleteAction(QString("Classic"), UiVariables::CLASSIC, QIcon(":/icons/classic.png"), colorGroup, signalMapper );
    createColorPalleteAction(QString("Summer (Color Blind Safe)"), UiVariables::COLORBLINDSAFE, QIcon(":/icons/colorblindsafe.png"), colorGroup, signalMapper );
    createColorPalleteAction(QString("Spring (Color Blind Safe)"), UiVariables::BETTERCBSAFE, QIcon(":/icons/colorblindsafe2.png"), colorGroup, signalMapper );
    createColorPalleteAction(QString("Dark Classic"), UiVariables::DARK, QIcon(":/icons/dark.png"), colorGroup, signalMapper );
    createColorPalleteAction(QString("DRuMS"), UiVariables::DRUMS, QIcon(":/icons/drums.png"), colorGroup, signalMapper );
    createColorPalleteAction(QString("Blues"), UiVariables::BLUES, QIcon(":/icons/blues.png"), colorGroup, signalMapper );
    createColorPalleteAction(QString("Reds"), UiVariables::REDS, QIcon(":/icons/reds.png"), colorGroup, signalMapper );

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutQtAct);
}

QAction* MainWindow::createColorPalleteAction(QString label, int colorPallete, QIcon palleteIcon, QActionGroup* group, QSignalMapper* signalMapper)
{
    QAction* colorPalleteAction = new QAction (palleteIcon, label, this );

    colorPalleteAction->setCheckable(true);
    colorPalleteAction->setActionGroup(group);
    colorSettingsMenu->addAction(colorPalleteAction);

    signalMapper->setMapping(colorPalleteAction, colorPallete);
    connect(colorPalleteAction, SIGNAL(triggered()), signalMapper, SLOT(map()));

    return colorPalleteAction;
}

void MainWindow::createToolbars()
{
    QFont boldFont = QFont();
    boldFont.setBold(true);

    fileToolBar = addToolBar("File Actions");
    fileToolBar->setObjectName("file");
    fileToolBar->setIconSize( QSize( 100, 20 ) );
    fileToolBar->addAction(updateSkittle);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(addViewAction);
    fileToolBar->addAction(openGtfAction);
    fileToolBar->addAction(nextAnnotationAction);
    fileToolBar->addAction(prevAnnotationAction);
    fileToolBar->addAction(screenCaptureAction);
    //annotationToolBar->addAction(browseCommunityAction);
    //annotationToolBar->addAction(delAnnotationAction);

    presetToolBar = new QToolBar("Visualization Graphs");
    presetToolBar->setObjectName("Visualization Graphs");
    presetToolBar->setOrientation(Qt::Horizontal);

    toolToolBar = addToolBar("Mouse Tools");
    toolToolBar->setObjectName("Mouse Tools");
    toolToolBar->addAction(moveAction);
    toolToolBar->addAction(resizeAction);
    toolToolBar->addAction(zoomAction);
    toolToolBar->addAction(selectAction);
    toolToolBar->addAction(findAction);
    toolToolBar->addAction(addAnnotationAction);

    //previous location of createUiVars()

    addToolBar(Qt::RightToolBarArea,presetToolBar);
    addToolBar(Qt::LeftToolBarArea,toolToolBar);
    addToolBar(Qt::LeftToolBarArea,fileToolBar);
    addToolBar(Qt::TopToolBarArea,settingToolBar);

}

void MainWindow::ensureDocksAreVisible()
{
    presetToolBar->setVisible(true);
    toolToolBar->setVisible(true);
    fileToolBar->setVisible(true);
    settingToolBar->setVisible(true);
    infoDock->setVisible(true);

}

void MainWindow::createDocks()
{	
    infoDock = new QDockWidget("Information Display", this);
    infoDock->setObjectName("infodock");
    infoDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    infoDock->setFeatures(QDockWidget::NoDockWidgetFeatures);//in particular we want to avoid Closable
    infoDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    tabWidget = new QTabWidget(infoDock);
    infoDock->setWidget(tabWidget);
    textArea = new QTextEdit(tabWidget);
    tabWidget->addTab(textArea, QString("Text Output"));
    addDockWidget(Qt::BottomDockWidgetArea, infoDock);

    /*
    QWidget *filters = new QWidget;
    QVBoxLayout *filterLayout = new QVBoxLayout;
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->addTab(new QListWidget, "Presets");
    //tabWidget->addTab(new QTreeView, "Advanced");
    QTableWidget *propsWidget = new QTableWidget(4,3);

    filterLayout->addWidget(tabWidget);
    filterLayout->addWidget(propsWidget);
    filters->setLayout(filterLayout);*/
}

void MainWindow::createStatusBar()
{
    //QLabel *statusTip = new QLabel("This is a status Tip",this);
    //QProgressBar *processStatus = new QProgressBar(this);
    //processStatus->setMinimumWidth(300);
    //processStatus->setMaximumWidth(300);
    //processStatus->setValue(75);
    //QLabel *processState = new QLabel("Loading...",this);

    //statusBar()->addPermanentWidget(processStatus);
    //statusBar()->addPermanentWidget(processState);
    statusBar()->showMessage(tr("Ready"));
}
void MainWindow::createUiConnections()
{	/******Internal UI Logic*********/
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(helpDialog()));
    
    connect(doubleDisplayWidth, SIGNAL(clicked()), this, SLOT(doubleWidth()));
    connect(halveDisplayWidth, SIGNAL(clicked()), this, SLOT(halveWidth()));
    //    connect(zoomExtents, SIGNAL(clicked()), , SLOT(zoomExtents()));

}
void MainWindow::createFileConnections()
{
    //Consider making this a function-call chain.... or handling this specifically, in a thread.
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(this, SIGNAL(newFileOpen(QString)), viewManager, SLOT(changeFile(QString)));

    connect(openGtfAction, SIGNAL(triggered()), this, SLOT(openGtf()));
    connect(this, SIGNAL(newGtfFileOpen(QString)), viewManager, SLOT(addAnnotationDisplay(QString)));
    connect(nextAnnotationAction, SIGNAL(triggered()), viewManager, SLOT(jumpToNextAnnotation()));
    connect(prevAnnotationAction, SIGNAL(triggered()), viewManager, SLOT(jumpToPrevAnnotation()));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,"Open Sequence File",
                "",
                "FASTA files (*.fa *.fasta);; Image files (*.png *.xpm *.jpg);; Text files (*.txt);; All files (*)"
                );

    if (!fileName.isEmpty())
        emit newFileOpen(fileName);
}

void MainWindow::open(QString fileName)
{
    if (!fileName.isEmpty())
        emit newFileOpen(fileName);
}

void MainWindow::changeWindowName(std::string name)
{
    string title = "Skittle Genome Visualizer";
    title.append(name);
    setWindowTitle( "Skittle Genome Visualizer");
}

void MainWindow::openGtf()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open GTF File", "", tr("Annotation files (*.gtf *.gff);; Any files (*)"));
    
    if (!fileName.isEmpty())
        emit newGtfFileOpen(fileName);
}

void MainWindow::doubleWidth()
{
    ui->changeWidth( 2 * ui->widthDial->value() );
}

void MainWindow::halveWidth()
{
    ui->changeWidth( (int)(0.5 * ui->widthDial->value()) );
}


void MainWindow::updateProgress(int val)
{
    //processStatus->setValue(val);
}
void MainWindow::updateState(QString state){
    //processState->setText(state);
}
void MainWindow::updateStatus(QString tip){
    statusBar()->showMessage(tip);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
    //	QMainWindow::closeEvent(event);
}

void MainWindow::readSettings()
{
    print("Reading User Settings");
    QSettings settings("Skittle", "Preferences");
    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    QList<QAction*> colorIndex = colorGroup->actions();
    colorIndex[settings.value("nucleotideColors").toInt()]->trigger();
    settings.endGroup();
}

void MainWindow::writeSettings()
{	
    print("Writing Settings");
    QSettings settings("Skittle", "Preferences");
    settings.beginGroup("mainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("nucleotideColors", ui->getColorSetting() );
    settings.endGroup();
}

void MainWindow::helpDialog()
{
    /*QMessageBox help(this);
    help.setText("Up to date help information can be found at http://dnaskittle.com/faq.");
    help.exec();*/

    QMessageBox mb(this);

    QString c("About Qt");
    mb.setWindowTitle(c);
    mb.setText(QString(
                   "<h3>About Skittle</h3>"
                   "<p>Skittle is an Open Source program for browsing genome sequences.</p>"
                   "<p>Up to date help information is available online at "
                   "<a href=\"http://dnaskittle.com/faq/\">http://DNASkittle.com/faq/</a> </p>"
                   "<p>For those new to the program, there is a tutorial at "
                   "<a href=\"http://dnaskittle.com/getting-started/\">http://DNASkittle.com/getting-started/</a>"
                   " that walks through the basic tools of Skittle and some real world"
                   " genome patterns that can be identified with Skittle.</p>"
                   ));
    /** /
#ifndef QT_NO_IMAGEFORMAT_XPM
    QImage logo(qtlogo_xpm);
#else
    QImage logo;
#endif

    if (qGray(mb.palette().color(QPalette::Active, QPalette::Text).rgb()) >
        qGray(mb.palette().color(QPalette::Active, QPalette::Base).rgb()))
    {
        // light on dark, adjust some colors
        logo.setColor(0, 0xffffffff);
        logo.setColor(1, 0xff666666);
        logo.setColor(2, 0xffcccc66);
        logo.setColor(4, 0xffcccccc);
        logo.setColor(6, 0xffffff66);
        logo.setColor(7, 0xff999999);
        logo.setColor(8, 0xff3333ff);
        logo.setColor(9, 0xffffff33);
        logo.setColor(11, 0xffcccc99);
    }
    QPixmap pm = QPixmap::fromImage(logo);
    if (!pm.isNull())
        mb.setIconPixmap(pm);*/
#if defined(Q_OS_WINCE)
    mb.setDefaultButton(mb.addButton(QMessageBox::Ok));
#else
    mb.addButton(QMessageBox::Ok);
#endif
    mb.exec();
}

/**********Print Functions**********/
void MainWindow::print(const char* str)
{
    if(textArea != NULL)
        textArea->append(QString(str));
}
void MainWindow::print(QString str)
{
    if(textArea != NULL)
        textArea->append(str);
}

void MainWindow::reportFinished(){
    print("Report Finished");
}
