#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "eventdialog.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QMenu>


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),ui(new Ui::MainWidget),
    tokenManager(new TokenManager(this)),
    googleClient(new GoogleClient(this))

{
    ui->setupUi(this);
    setWindowTitle("Freelander");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint );
    QSettings settings("Freelander", "Freelander");
    restoreGeometry(settings.value("geometry").toByteArray());
//| Qt::WindowStaysOnTopHint
    trayIcon = new QSystemTrayIcon(this);
    //  ikonu z .qrc
    QIcon icon(":/icons/icon.png");

    if (!icon.isNull()) {
        trayIcon->setIcon(icon);
    } else {
        // Fallback emoji
        trayIcon->setIcon(QIcon::fromTheme("face-smile")); // nebo něco, co systém najde
        trayIcon->setToolTip(QString::fromUtf8("📅Freelander"));
    }
    trayIcon->setToolTip("📅 Freelander");

    QMenu *menu = new QMenu(this);
    QAction *showAction = new QAction("📂 Zobrazit", this);
    QAction *exitAction = new QAction("❌ Ukončit", this);
    connect(showAction, &QAction::triggered, this, &QWidget::showNormal);
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    menu->addAction(showAction);
    menu->addSeparator();
    menu->addAction(exitAction);

    trayIcon->setContextMenu(menu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
             if (isVisible()) hide();
             else
            showNormal();
        }
    });

    trayIcon->show();

    auto *lay = new QVBoxLayout(this);
    calendar   = new QCalendarWidget(this);
    textEdit   = new QTextEdit(this); textEdit->setReadOnly(true);
    titleInput = new QLineEdit(this); titleInput->setPlaceholderText("Název události");
    startInput = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    endInput   = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), this);
    //addBtn     = new QPushButton("➕ Přidat", this);
    //updateBtn  = new QPushButton("💾 Uložit", this);
    //deleteBtn  = new QPushButton("❌ Smazat", this);

    lay->addWidget(calendar);
    lay->addWidget(textEdit);
    lay->addWidget(titleInput);
    lay->addWidget(startInput);
    lay->addWidget(endInput);
    auto *hl = new QHBoxLayout;
   // hl->addWidget(addBtn);
   // hl->addWidget(updateBtn);
    //hl->addWidget(deleteBtn);
    lay->addLayout(hl);

    connect(tokenManager, &TokenManager::tokenReady,this, &MainWidget::onTokenReady);
    connect(tokenManager, &TokenManager::authenticationFailed,this, [&](const QString &err){ QMessageBox::warning(this,"Auth failed",err); });

    connect(calendar, &QCalendarWidget::currentPageChanged,this, &MainWidget::onCalendarPageChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,this, &MainWidget::onEventClicked);
    connect(googleClient, &GoogleClient::eventsFetched,this, &MainWidget::onEventsFetched);
    connect(googleClient, &GoogleClient::eventDetailsFetched,this, &MainWidget::onEventDetailsFetched);
    connect(calendar, &QCalendarWidget::activated, this, &MainWidget::onCalendarDateActivated);

    tokenManager->initialize();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::onTokenReady(const QString &token) {
    qDebug() << "Access token ready:" << token;
    googleClient->setAccessToken(token);
    googleClient->fetchEvents(calendar->selectedDate(), calendar);
}

void MainWidget::onEventsFetched(const QString &text, const QSet<QDate> &dates) {
     qDebug() << "Events:" << text;
    for (const QDate &date : dates) {
        qDebug() << "Datum:" << date.toString();
    }
    textEdit->setPlainText(text);
    calendar->setDateTextFormat(QDate(), QTextCharFormat());
    QTextCharFormat fmt; fmt.setBackground(QColor(200,230,255));
    for (auto d : dates) calendar->setDateTextFormat(d, fmt);
}

void MainWidget::onEventDetailsFetched(const QString &sum, const QDateTime &st, const QDateTime &en) {
    titleInput->setText(sum);
    startInput->setDateTime(st);
    endInput->setDateTime(en);
}


void MainWidget::onCalendarPageChanged(int year,int month) {
    googleClient->fetchEvents(QDate(year,month,1),  calendar);
}

void MainWidget::onEventClicked() {
    QTextCursor c = textEdit->textCursor();
    c.select(QTextCursor::LineUnderCursor);
    selectedLine = c.selectedText();
    QString id = googleClient->eventIdMap.value(selectedLine);
    if (!id.isEmpty()) googleClient->fetchEventDetails(id);
}

void MainWidget::onAddClicked() {
    googleClient->createEvent(titleInput->text(), startInput->dateTime(), endInput->dateTime(),calendar);
}

void MainWidget::onUpdateClicked() {
    QString id = googleClient->eventIdMap.value(selectedLine);
    if (!id.isEmpty()) googleClient->updateEvent(id, titleInput->text(), startInput->dateTime(), endInput->dateTime(),calendar);
}

void MainWidget::onDeleteClicked() {
    QString id = googleClient->eventIdMap.value(selectedLine);
    if (!id.isEmpty()) googleClient->deleteEvent(id,calendar);
}
void MainWidget::onDeleteClickedId(QString id) {
   // QString id = googleClient->eventIdMap.value(selectedLine);
    if (!id.isEmpty()) googleClient->deleteEvent(id,calendar);
}
void MainWidget::onCalendarDateActivated(const QDate &date) {
    QDateTime dateTime(date, QTime(8, 0)); // výchozí čas

    QString existingSummary;
    QString existingEventId;

    for (const auto &entry : googleClient->eventIdMap.keys()) {
        if (entry.startsWith(date.toString("dd.MM.yyyy"))) {
            existingSummary = entry.section(" - ", 1);
            existingEventId = googleClient->eventIdMap.value(entry);
            break;
        }
    }


    EventDialog dialog(this);
    dialog.setDateTime(dateTime);
    dialog.setText(existingSummary);
    dialog.setEditMode(!existingSummary.isEmpty());

    if (!existingEventId.isEmpty()) {

        dialog.setEventId(existingEventId);
        dialog.setWidget(this);

        qDebug() << "id exist:" << existingEventId;

    } else {
        // Přidat novou událost
        dialog.deleteButton->hide();
        qDebug() << "id is null:" << existingEventId;
    }



    if (dialog.exec() == QDialog::Accepted) {
        QString summary = dialog.text();
        QDateTime dt = dialog.dateTime();

        if (!existingEventId.isEmpty()) {
            // Upravit událost
           googleClient->updateEvent(existingEventId, summary, dt, dt,calendar);
        } else {
            // Přidat novou událost
             googleClient->createEvent(summary, dt, dt,calendar);
        }

        // Obnovit události pro zobrazený měsíc
        QDate currentPage(calendar->yearShown(), calendar->monthShown(), 1);
        googleClient->fetchEvents(currentPage, calendar);
    }
}
void MainWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible())
            hide();
        else
            show();
    }
}
void MainWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}
void MainWidget::closeEvent(QCloseEvent *event) {
    // Uložíme pozici a velikost
    QSettings settings("Freelander", "Freelander");
    settings.setValue("geometry", saveGeometry());

    // Místo zavření okno jen schováme do tray
    hide();
    event->ignore();
}

