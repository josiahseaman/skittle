/********************************************************************************
** Form generated from reading UI file 'BookmarkDialog.ui'
**
** Created: Mon Jan 31 16:05:41 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOOKMARKDIALOG_H
#define UI_BOOKMARKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_BookmarkDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *sequence;
    QLineEdit *source;
    QLineEdit *feature;
    QLineEdit *start;
    QLineEdit *end;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QFrame *line;
    QCheckBox *strand;
    QComboBox *frame;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *score;
    QPlainTextEdit *note;

    void setupUi(QDialog *BookmarkDialog)
    {
        if (BookmarkDialog->objectName().isEmpty())
            BookmarkDialog->setObjectName(QString::fromUtf8("BookmarkDialog"));
        BookmarkDialog->resize(377, 315);
        BookmarkDialog->setMinimumSize(QSize(377, 315));
        BookmarkDialog->setMaximumSize(QSize(377, 2250));
        buttonBox = new QDialogButtonBox(BookmarkDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 280, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        sequence = new QLineEdit(BookmarkDialog);
        sequence->setObjectName(QString::fromUtf8("sequence"));
        sequence->setGeometry(QRect(66, 71, 137, 20));
        source = new QLineEdit(BookmarkDialog);
        source->setObjectName(QString::fromUtf8("source"));
        source->setGeometry(QRect(66, 100, 136, 20));
        feature = new QLineEdit(BookmarkDialog);
        feature->setObjectName(QString::fromUtf8("feature"));
        feature->setGeometry(QRect(66, 130, 137, 20));
        start = new QLineEdit(BookmarkDialog);
        start->setObjectName(QString::fromUtf8("start"));
        start->setGeometry(QRect(66, 10, 137, 20));
        end = new QLineEdit(BookmarkDialog);
        end->setObjectName(QString::fromUtf8("end"));
        end->setGeometry(QRect(66, 40, 137, 20));
        label = new QLabel(BookmarkDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(7, 70, 72, 20));
        label_2 = new QLabel(BookmarkDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(7, 100, 69, 20));
        label_3 = new QLabel(BookmarkDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(7, 130, 69, 20));
        label_4 = new QLabel(BookmarkDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(8, 10, 68, 20));
        label_5 = new QLabel(BookmarkDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(8, 40, 68, 20));
        label_6 = new QLabel(BookmarkDialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(8, 155, 68, 20));
        line = new QFrame(BookmarkDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(210, 5, 20, 143));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        strand = new QCheckBox(BookmarkDialog);
        strand->setObjectName(QString::fromUtf8("strand"));
        strand->setGeometry(QRect(230, 43, 110, 18));
        strand->setChecked(true);
        frame = new QComboBox(BookmarkDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(270, 68, 81, 22));
        label_7 = new QLabel(BookmarkDialog);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(230, 70, 72, 20));
        label_8 = new QLabel(BookmarkDialog);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(232, 10, 68, 20));
        score = new QLineEdit(BookmarkDialog);
        score->setObjectName(QString::fromUtf8("score"));
        score->setGeometry(QRect(265, 10, 106, 20));
        note = new QPlainTextEdit(BookmarkDialog);
        note->setObjectName(QString::fromUtf8("note"));
        note->setGeometry(QRect(67, 160, 304, 111));
        note->setAcceptDrops(false);
        note->setTabChangesFocus(true);
        QWidget::setTabOrder(buttonBox, note);
        QWidget::setTabOrder(note, start);
        QWidget::setTabOrder(start, end);
        QWidget::setTabOrder(end, sequence);
        QWidget::setTabOrder(sequence, source);
        QWidget::setTabOrder(source, feature);
        QWidget::setTabOrder(feature, score);
        QWidget::setTabOrder(score, strand);
        QWidget::setTabOrder(strand, frame);

        retranslateUi(BookmarkDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), BookmarkDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), BookmarkDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(BookmarkDialog);
    } // setupUi

    void retranslateUi(QDialog *BookmarkDialog)
    {
        BookmarkDialog->setWindowTitle(QApplication::translate("BookmarkDialog", "Bookmark", 0, QApplication::UnicodeUTF8));
        source->setText(QApplication::translate("BookmarkDialog", "Skittle", 0, QApplication::UnicodeUTF8));
        feature->setText(QApplication::translate("BookmarkDialog", "user_bookmark", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BookmarkDialog", "Sequence", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("BookmarkDialog", "Source", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("BookmarkDialog", "Feature", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("BookmarkDialog", "Start", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("BookmarkDialog", "End", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("BookmarkDialog", "Note", 0, QApplication::UnicodeUTF8));
        strand->setText(QApplication::translate("BookmarkDialog", "Positive Strand", 0, QApplication::UnicodeUTF8));
        frame->clear();
        frame->insertItems(0, QStringList()
         << QApplication::translate("BookmarkDialog", ".", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BookmarkDialog", "-", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BookmarkDialog", "+", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BookmarkDialog", "0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BookmarkDialog", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BookmarkDialog", "2", 0, QApplication::UnicodeUTF8)
        );
        label_7->setText(QApplication::translate("BookmarkDialog", "Frame", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("BookmarkDialog", "Score", 0, QApplication::UnicodeUTF8));
        score->setText(QApplication::translate("BookmarkDialog", ".", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BookmarkDialog: public Ui_BookmarkDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOOKMARKDIALOG_H
