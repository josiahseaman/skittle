/****************************************************************************
* Skittle Author's Note:  ColorListEditor is copied directly from the 
* Qt Demo library > Itemviews > Color Editor Factory Example.
* It is provided Open Source along with the rest of Skittle Genome Visualizer.
****************************************************************************/


#ifndef COLORLISTEDITOR_H
#define COLORLISTEDITOR_H

#include <QComboBox>

QT_BEGIN_NAMESPACE
class QColor;
class QWidget;
QT_END_NAMESPACE


class ColorListEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor USER true)

public:
    ColorListEditor(QWidget *widget = 0);

public:
    QColor color() const;
    void setColor(QColor c);

private:
    bool notGreyScale(QColor col);
    void populateList();
    void insertColorItem(int index, QColor col, QString name);
    QIcon createIcon(QColor col);
};
//! [0]

#endif
