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

    QString text() const;
    QDateTime dateTime() const;
    QDateTime dateEndTime() const;
    void setDateTime(const QDateTime &dt);
    void setText(const QString &text);
    void setEditMode(bool edit);
    void setWidget(MainWidget *w);
    void setEventId(QString id);
    void hideDeleteButton();
    void setEndDateTime(const QDateTime &dt);

private slots:
    void onDeleteClicked();

private:

    Ui::EventDialog *ui;
    QTextEdit *m_textEdit;
    QDateTimeEdit *m_dateEdit;
    QDateTimeEdit *m_dateEndEdit;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QPushButton *deleteButton ;
    MainWidget *pointerToMainWidget ;
    QString eventId;

};


#endif // EVENTDIALOG_H


