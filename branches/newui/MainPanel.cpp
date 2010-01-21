//MainPanel.cpp

/*
Control the scroll bars by setting their range, value, page step, and tracking their movements.
Draw the contents of the area in the viewport according to the values of the scroll bars.
Handle events received by the viewport in viewportEvent() - notably resize events.
Use viewport->update() to update the contents of the viewport instead of update() as all painting 
operations take place on the viewport.
*/

MainPanel::MainPanel()
{
     QSize areaSize = viewport()->size();
     QSize  widgetSize = widget->size();

     verticalScrollBar()->setPageStep(widgetSize.height());
     horizontalScrollBar()->setPageStep(widgetSize.width());
     verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
     horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
     updateWidgetPosition();

     int hvalue = horizontalScrollBar()->value();
     int vvalue = verticalScrollBar()->value();
     QPoint topLeft = viewport()->rect().topLeft();

     widget->move(topLeft.x() - hvalue, topLeft.y() - vvalue);
}


