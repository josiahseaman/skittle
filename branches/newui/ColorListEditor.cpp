/****************************************************************************
* Skittle Author's Note:  ColorListEditor is copied directly from the 
* Qt Demo library > Itemviews > Color Editor Factory Example.
* It is provided Open Source along with the rest of Skittle Genome Visualizer.
****************************************************************************/

/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QtGui>

#include <stdlib.h>
#include <ctime>

#include "ColorListEditor.h"

ColorListEditor::ColorListEditor(QWidget *widget) : QComboBox(widget)
{
	//srand(time(0));
    populateList();
}

QColor ColorListEditor::color() const
{
    return qVariantValue<QColor>(itemData(currentIndex(), Qt::WhatsThisRole));
}

void ColorListEditor::setColor(QColor color)
{
    setCurrentIndex(findData(color, int(Qt::WhatsThisRole)));
}

/*Gray scale colors have roughly the same r,g, and b values*/
bool ColorListEditor::notGreyScale(QColor col)
{
	int red, green, blue;
	col.getRgb(&red, &green, &blue);
	float average = (red + green + blue) / 3.0;
	int min = (int)(average * .8);
	int max = (int)(average * 1.2);
	if( red > max || red < min )
		return true;
	if( green > max || green < min )
		return true;		
	if( blue > max || blue < min )
		return true;
		
	return false;
}

QIcon ColorListEditor::createIcon(QColor col)
{
	QPixmap pix(30,30);
	pix.fill(col);
	return QIcon(pix);
}

void ColorListEditor::insertColorItem(int index, QColor col, QString name)
{
	insertItem(index, createIcon(col), name);
	setItemData(index, col, Qt::WhatsThisRole);
}
    
void ColorListEditor::populateList()
{	
    QStringList colorNames = QColor::colorNames();
    
	insertColorItem(0, QColor(0,0,0), QString("Black"));
	insertColorItem(1, QColor(255,0,0), QString("Red"));
	insertColorItem(2, QColor(0,255,0), QString("Green"));
	insertColorItem(3, QColor(0,0,255), QString("Blue"));
	
	int k = 4;
    for (int i = 0; i < colorNames.size(); ++i) 
	{
        QColor col(colorNames[i]);
        if(notGreyScale(col))
        {
			insertColorItem(k, col, colorNames[k]);
        	k++;
		}
    }
    setCurrentIndex( rand() % count() );
}
