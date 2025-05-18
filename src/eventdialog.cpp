#include "eventdialog.h"
#include "ui_eventdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>


EventDialog::EventDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EventDialog) , m_textEdit(new QTextEdit(this)),
    m_dateEdit(new QDateTimeEdit(QDateTime::currentDateTime(), this)),
    m_dateEndEdit(new QDateTimeEdit(QDateTime::currentDateTime(), this)),
    saveButton(new QPushButton("Add Event", this))
{
        ui->setupUi(this);
        setWindowTitle("Event");

        m_dateEdit->setCalendarPopup(true);
        m_dateEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
        m_dateEndEdit->setCalendarPopup(true);
        m_dateEndEdit->setDisplayFormat("dd.MM.yyyy HH:mm");



        auto *layout = new QVBoxLayout(this);
        layout->addWidget(m_dateEdit);
        layout->addWidget(m_dateEndEdit);
        layout->addWidget(m_textEdit);

        layout->addWidget(new QLabel("Start Date & Time:", this));
        layout->addWidget(m_dateEdit);

        layout->addWidget(new QLabel("End Date & Time:", this));
        layout->addWidget(m_dateEndEdit);

        layout->addWidget(new QLabel("Event Description:", this));
        layout->addWidget(m_textEdit);
        auto *btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        cancelButton = new QPushButton("Cancel", this);
        btnLayout->addWidget(cancelButton);
        deleteButton = new QPushButton("Delete",this);
        btnLayout->addWidget(deleteButton);
        btnLayout->addWidget(saveButton);
        layout->addLayout(btnLayout);

        connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        connect(deleteButton, &QPushButton::clicked, this, &EventDialog::onDeleteClicked);
}

EventDialog::~EventDialog()
{
    delete ui;
}
void EventDialog::onDeleteClicked()
{
    if(pointerToMainWidget && !eventId.isEmpty()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirm",
                                      "Delete event?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            pointerToMainWidget->onDeleteClickedId(eventId);
            accept(); //  close()
        }
    }
    else{
        qDebug() << "event id not exist:" << eventId;
    }
}
void EventDialog::setDateTime(const QDateTime &dt) {
    m_dateEdit->setDateTime(dt);
}
void EventDialog::setText(const QString &text) {
    m_textEdit->setText(text);
}
QString EventDialog::text() const {
    return m_textEdit->toPlainText();
}
QDateTime EventDialog::dateTime() const {
    return m_dateEdit->dateTime();
}
QDateTime EventDialog::dateEndTime() const {
    return m_dateEndEdit->dateTime();
}
void EventDialog::setEndDateTime(const QDateTime &dt) {
    m_dateEndEdit->setDateTime(dt);
}
void EventDialog::setEditMode(bool edit) {
    saveButton->setText(edit ? "Update" : "Add Event");
}
void EventDialog::setWidget(MainWidget *w)
{
    pointerToMainWidget = w;
}
void EventDialog::setEventId(QString id)
{
    eventId = id;
}
void EventDialog::hideDeleteButton()
{
    deleteButton->hide();
}


