//MainPanel.h
#ifndef SKITTLE_MAINPANEL_H
#define SKITTLE_MAINPANEL_H

#include <QAbstractScrollArea>
#include <QScrollBar>


class MainPanelPrivate;

class MainPanel : public QAbstractScrollArea
{
	Q_OBJECT
public:
	MainPanel();
	
void setWidget(QWidget* widget);
void setWidgetResizable(bool b);

void contextMenuEvent ( QContextMenuEvent * e );
void dragEnterEvent ( QDragEnterEvent * event );
void dragLeaveEvent ( QDragLeaveEvent * event );
void dragMoveEvent ( QDragMoveEvent * event );
void dropEvent ( QDropEvent * event );
void keyPressEvent ( QKeyEvent * e );
void mouseDoubleClickEvent ( QMouseEvent * e );
void mouseMoveEvent ( QMouseEvent * e );
void mousePressEvent ( QMouseEvent * e );
void mouseReleaseEvent ( QMouseEvent * e );
void paintEvent ( QPaintEvent * event );
void resizeEvent ( QResizeEvent * event );
void scrollContentsBy ( int dx, int dy );
bool viewportEvent ( QEvent * event );   
void wheelEvent ( QWheelEvent * e );

private:
	
};


#endif // QSCROLLAREA_H
