#ifndef SEQ_ENTRY
#define SEQ_ENTRY

#include <QWidget>
#include <QString>
#include <string>
#include <vector>

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
	string seq;
	
	SequenceEntry(QLabel* Label, QLineEdit* line, QPushButton* remove, string sequence);

public slots:
	void setHighlightSequence(const QString&);
	void removeSequence();
	
signals:
	void removeEntry(SequenceEntry*);
	
};

#endif
