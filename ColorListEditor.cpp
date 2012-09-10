/****************************************************************************
* Skittle Author's Note:  ColorListEditor is copied directly from the 
* Qt Demo library > Itemviews > Color Editor Factory Example.
* It is provided Open Source along with the rest of Skittle Genome Visualizer.
****************************************************************************/
#include <QtGui>

#include <stdlib.h>
#include <ctime>

#include "ColorListEditor.h"


/** ************************
  This class is for picking the color that HighlightDisplay uses for a particular sequence.
  It provides the user with a list of colors and names and gives the RGB for whatever value
  is selected.
  ************************/


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
