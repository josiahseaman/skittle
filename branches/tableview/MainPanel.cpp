//MainPanel.cpp
#include "MainPanel.h"
/*
Control the scroll bars by setting their range, value, page step, and tracking their movements.
Draw the contents of the area in the viewport according to the values of the scroll bars.
Handle events received by the viewport in viewportEvent() - notably resize events.
Use viewport->update() to update the contents of the viewport instead of update() as all painting 
operations take place on the viewport.
*/

MainPanel::MainPanel()
{
}

void MainPanel::setWidget(QWidget* widget)//TODO: claim GLWidget and pack it inside 
{
	//Copied from QScrollArea////////////////
	
    QWidget* d = ?
    if (widget == d->widget || !widget)
        return;

    delete d->widget;
    d->widget = 0;
    d->hbar->setValue(0);
    d->vbar->setValue(0);
    if (widget->parentWidget() != d->viewport)
        widget->setParent(d->viewport);
    if (!widget->testAttribute(Qt::WA_Resized))
        widget->resize(widget->sizeHint());
    d->widget = widget;
    d->widget->setAutoFillBackground(true);
    widget->installEventFilter(this);
    d->widgetSize = QSize();
    d->updateScrollBars();
    d->widget->show();
    /*///////////////////////////////////
	
     QSize areaSize = viewport()->size();
     QSize  widgetSize = widget->size();

     verticalScrollBar()->setPageStep(widgetSize.height());
     horizontalScrollBar()->setPageStep(widgetSize.width());
     verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
     horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
     //updateWidgetPosition();//<<<<<<<<<--------------------

     int hvalue = horizontalScrollBar()->value();
     int vvalue = verticalScrollBar()->value();
     QPoint topLeft = viewport()->rect().topLeft();

     widget->move(topLeft.x() - hvalue, topLeft.y() - vvalue);/**/
}

void MainPanel::setWidgetResizable(bool b) {}

void MainPanel::contextMenuEvent ( QContextMenuEvent * e )   {}

void MainPanel::dragEnterEvent ( QDragEnterEvent * event )   {}

void MainPanel::dragLeaveEvent ( QDragLeaveEvent * event )   {}

void MainPanel::dragMoveEvent ( QDragMoveEvent * event )   {}

void MainPanel::dropEvent ( QDropEvent * event )   {}

void MainPanel::keyPressEvent ( QKeyEvent * e )   {}

void MainPanel::mouseDoubleClickEvent ( QMouseEvent * e )   {}

void MainPanel::mouseMoveEvent ( QMouseEvent * e )   {}

void MainPanel::mousePressEvent ( QMouseEvent * e )   {}

void MainPanel::mouseReleaseEvent ( QMouseEvent * e )   {}

void MainPanel::paintEvent ( QPaintEvent * event )   {}

void MainPanel::resizeEvent ( QResizeEvent * event )   {}

void MainPanel::scrollContentsBy ( int dx, int dy )   {}

bool MainPanel::viewportEvent ( QEvent * event )   
{
	return false;
}

void MainPanel::wheelEvent ( QWheelEvent * e )   {}
