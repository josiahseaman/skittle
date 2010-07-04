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

void ColorListEditor::populateList()
{	
	srand(time(0));
    QStringList colorNames = QColor::colorNames();

    for (int i = 0; i < colorNames.size(); ++i) 
	{
        QColor color(colorNames[i]);
		QPixmap pix(30,30);
		pix.fill(color);
		QIcon icon(pix);

        insertItem(i, icon, colorNames[i]);
        setItemData(i, color, Qt::WhatsThisRole);
    }
    setCurrentIndex( rand() % count() );
}
