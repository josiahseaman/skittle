#include "SequenceEntry.h"
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>


SequenceEntry::SequenceEntry(QLabel* Label, QLineEdit* line)
{
	label = Label;
	lineEdit = line;
	removeButton = new QPushButton("Delete");
	seq = line->text().toStdString();
	colorBox = new ColorListEditor(this);
	changeColor();
	
	connect(removeButton, SIGNAL(clicked()), this, SLOT(removeSequence()) );
	connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setHighlightSequence(const QString&)));
	connect(colorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeColor()));
}

void SequenceEntry::changeColor()
{
	QColor val = colorBox->color();
	int r, g, b;
	val.getRgb(&r, &g, &b);
	matchColor = color(r,g,b);
	mismatchColor = matchColor * 0.4;
	emit colorChanged();
}

void SequenceEntry::setHighlightSequence(const QString& high_C)
{
	string high = high_C.toStdString();
	if(high.compare(seq) == 0)
		return;
		
	for(int l = 0; l < (int)high.size(); l++)
		if(high[l] >= 97 && high[l] <= 122) high[l] -= 32;//cast to uppercase

	seq = high;
//	if(reverseCheck->isChecked())
	//	targets.push_back( reverseComplement(high) );
	//ui->print("Number of strings currently highlighted: ", targets.size());
	
	QString copy(high.c_str());
	lineEdit->setText(copy);
}

void SequenceEntry::removeSequence()
{
	emit removeEntry( this );
	//delete this;
}

