#include "eventdialog.h"
#include "ui_eventdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>


EventDialog::EventDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EventDialog) , m_textEdit(new QTextEdit(this)),
    m_dateEdit(new QDateTimeEdit(QDateTime::currentDateTime(), this)),
    m_okButton(new QPushButton("Uložit", this))
{
        ui->setupUi(this);
        m_dateEdit->setCalendarPopup(true);
        m_dateEdit->setDisplayFormat("dd.MM.yyyy HH:mm");

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(m_dateEdit);
        layout->addWidget(m_textEdit);

        auto *btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        deleteButton = new QPushButton("Smazat");
         btnLayout->addWidget(deleteButton);
        btnLayout->addWidget(m_okButton);

        QPushButton *cancelButton = new QPushButton("Zrušit", this);
        btnLayout->addWidget(cancelButton);


        layout->addLayout(btnLayout);

        connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        connect(deleteButton, &QPushButton::clicked, this, &EventDialog::onDeleteClicked);

}

EventDialog::~EventDialog()
{
    delete ui;
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
void EventDialog::setEditMode(bool edit) {
    m_okButton->setText(edit ? "Upravit" : "Uložit");
}
void EventDialog::setWidget(MainWidget *w)
{
    pointerToWidget = w;
}
void EventDialog::setEventId(QString id)
{
    eventId = id;
}

void EventDialog::onDeleteClicked()
{
    if(pointerToWidget && !eventId.isEmpty()){
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Potvrzení",
                                      "Opravdu chcete událost smazat?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            pointerToWidget->onDeleteClickedId(eventId);
            accept(); // nebo close(), podle potřeby
        }
    }
    else{

    }  qDebug() << "id not exist:" << eventId;


}
