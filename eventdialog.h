#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include "mainwidget.h"

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();
    void setDateTime(const QDateTime &dt);
    void setText(const QString &text);
    QString text() const;
    QDateTime dateTime() const;
    void setEditMode(bool edit);
    void setWidget(MainWidget *w);
    void setEventId(QString id);
    QPushButton *deleteButton ;

    QDateTime dateEndTime() const;
    void setEndDateTime(const QDateTime &dt);
private slots:
    void onDeleteClicked();


private:

    Ui::EventDialog *ui;
    QTextEdit *m_textEdit;
    QDateTimeEdit *m_dateEdit;
    QDateTimeEdit *m_dateEndEdit;
    QPushButton *m_okButton;
    QPushButton *cancelButton;

    MainWidget *pointerToWidget ;
    QString eventId;

};


#endif // EVENTDIALOG_H


