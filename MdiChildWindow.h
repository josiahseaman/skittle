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
    UiVariables* ui;

    MdiChildWindow(QTabWidget* settings);
    void closeEvent(QCloseEvent *event);
    void connectWidget();
    void hideSettingsTabs();
    void showSettingsTabs();
    void mousePressEvent(QMouseEvent *event);

public slots:
    void setHorizontalWidth(int val);
    void setPageSize();
    void hideSettingsTab(QScrollArea*);
    void showSettingsTab(QScrollArea*);

signals:
    void subWindowClosing(MdiChildWindow*);
    void IveBeenClicked(MdiChildWindow*);

private:
    QTabWidget* settingsDock;
    vector<QScrollArea*> settingsTabs;

    void createSettingsTabs();
};	
#endif
