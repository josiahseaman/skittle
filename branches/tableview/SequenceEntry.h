#ifndef SEQ_ENTRY
#define SEQ_ENTRY

#include <QWidget>
#include <QString>
#include <string>
#include <vector>
#include "ColorListEditor.h"
#include "BasicTypes.h"

using std::string;
class QLineEdit;
class QPushButton;
class QLabel;

class SequenceEntry : public QWidget
{
	Q_OBJECT
public:
	QLabel* label;
	QLineEdit* lineEdit;
	QPushButton* removeButton;
	ColorListEditor* colorBox;
	string seq;
	color matchColor;
	color mismatchColor;
	
	SequenceEntry(QLabel* Label, QLineEdit* line);

public slots:
	void setHighlightSequence(const QString&);
	void removeSequence();
	void changeColor();
	
signals:
	void colorChanged();
	void removeEntry(SequenceEntry*);
	
};

#endif
