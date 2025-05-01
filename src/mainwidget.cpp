#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "calendardelegate.h"
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
#include <QTableView>
#include <QHeaderView>
#include <QLabel>


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),ui(new Ui::MainWidget),
    tokenManager(new TokenManager(this)),
    googleClient(new GoogleClient(this))

{
    ui->setupUi(this);
    setWindowTitle("Freelander");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint );
    //| Qt::WindowStaysOnTopHint
    QSettings settings("Freelander", "Freelander");
    restoreGeometry(settings.value("geometry").toByteArray());

    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/icons/icon.png");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(QString::fromUtf8("📅Freelander"));

    QMenu *menu = new QMenu(this);
    QAction *showAction = new QAction("⚙️ Settings", this);
    QAction *exitAction = new QAction("❌ Exit", this);
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



    calendar = new FreelanderCalendar(this);
    calendar->setGridVisible(true);
    //calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    calendar->setContextMenuPolicy(Qt::CustomContextMenu);
    calendar->setStyleSheet(R"(
        QCalendarWidget {
            background: transparent;
            color: white;
            font-size: 9pt;
        }
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background: transparent;
            color: white;
        }
    )");
    //CalendarDelegate *delegate = new CalendarDelegate(this);
    QTableView *tableView = calendar->findChild<QTableView *>();
    if (tableView) {
        tableView->viewport()->setAutoFillBackground(false);
        tableView->setStyleSheet("background: transparent;");
      //  tableView->setItemDelegate(delegate);
        QHeaderView *header = tableView->horizontalHeader();
        if (header) {
            header->setStyleSheet(R"(
            background: transparent;
            color: white;
            font-size: 10pt;

        )");
        }
    }
    QWidget *navBar = calendar->findChild<QWidget *>("qt_calendar_navigationbar");
    if (navBar) {
        navBar->setStyleSheet(R"(
        background: transparent;
            color: white;
            font-size: 12pt;
    )");
    }
    QIcon prevIcon(":/icons/left-icon.png");
    QIcon nextIcon(":/icons/right-icon.png");


    if (!prevIcon.isNull() && !nextIcon.isNull()) {
        calendar->setNavigationIcons(prevIcon, nextIcon);
    } else {
        qWarning("icon error.");

    }


    QLabel *label = new QLabel("EVENTS",this);
    label->setStyleSheet("background-color: #2a2a2a; color: white; font-size: 16px;");
    label->setAlignment(Qt::AlignCenter);
   // label->setAttribute(Qt::WA_TranslucentBackground);

    textEdit   = new ClickableTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setStyleSheet("QTextEdit { background-color: transparent; }");

    lay->addWidget(calendar);
    lay->addWidget(label);
    lay->addWidget(textEdit);

    connect(tokenManager, &TokenManager::tokenReady,this, &MainWidget::onTokenReady);
    connect(tokenManager, &TokenManager::authenticationFailed,this, [&](const QString &err){ QMessageBox::warning(this,"Auth failed",err); });

    connect(googleClient, &GoogleClient::eventsFetched,this, &MainWidget::onEventsFetched);
    connect(googleClient, &GoogleClient::eventDetailsFetched,this, &MainWidget::onEventDetailsFetched);

    connect(calendar, &QCalendarWidget::activated, this, &MainWidget::onCalendarDateActivated);  
    connect(calendar, &QCalendarWidget::currentPageChanged,this, &MainWidget::onCalendarPageChanged);
    connect(calendar, &QCalendarWidget::clicked, this, &MainWidget::handleDateClicked);
    connect(calendar ,&QCalendarWidget::customContextMenuRequested, this, &MainWidget::calendarContextMenuRequested);

    connect(textEdit, &ClickableTextEdit::lineDoubleClicked, this,&MainWidget::handleLineClick);
    // connect(textEdit, &ClickableTextEdit::cursorPositionChanged,this, &MainWidget::onEventClicked);
    tokenManager->initialize();
}

MainWidget::~MainWidget()
{
    delete ui;
}
void MainWidget::paintEvent(QPaintEvent *) {

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor backgroundColor(0, 0, 0, 128);
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);
    //calendar->repaint();

}
void MainWidget::onTokenReady(const QString &token) {
    //qDebug() << "Access token ready:" << token;
    googleClient->setAccessToken(token);
    googleClient->fetchEvents(calendar->selectedDate(), calendar);
}

void MainWidget::onEventsFetched(const QString &text, const QSet<QDate> &dates) {
    // qDebug() << "Events:" << text;
    //for (const QDate &date : dates) {
     //   qDebug() << "Datum:" << date.toString();
    //}
    QColor backgroundColor(255, 0, 0, 0);
    textEdit->setPlainText(text);
    QFontMetrics fm(textEdit->font());
    int lineHeight = fm.lineSpacing();
    int lineCount = textEdit->document()->blockCount();
    lineCount = std::min(lineCount, 10);
    int newHeight = lineCount * lineHeight + 20; // 10px na padding
    //textEdit->setFixedHeight(newHeight);

    calendar->setDateTextFormat(QDate(), QTextCharFormat());
    QTextCharFormat fmt;
    fmt.setBackground(backgroundColor);
    fmt.setFontPointSize(12);
    //  fmt.setFontPixelSize(16);
    fmt.setFontWeight(QFont::Bold);
    QColor textColor(3, 232, 252, 255);

    fmt.setForeground(textColor);
    for (auto d : dates){
        calendar->setDateTextFormat(d, fmt);
    }
    calendar->repaint();
}

void MainWidget::onEventDetailsFetched(const QString &sum, const QDateTime &st, const QDateTime &en , const QString &eventId) {


    EventDialog dialog(this);
    dialog.setDateTime(st);
    dialog.setEndDateTime(en);
    dialog.setText(sum);
    dialog.setEditMode(!eventId.isEmpty());
    dialog.setEventId(eventId);
    dialog.setWidget(this);

    if (!eventId.isEmpty()) {
        dialog.setEventId(eventId);
        dialog.setWidget(this);
        qDebug() << "id exist:" << eventId;
    } else {

        qDebug() << "id is null:" << eventId;
    }

    if (dialog.exec() == QDialog::Accepted) {

        QString summary = dialog.text();
        QDateTime dt = dialog.dateTime();
        QDateTime enddt = dialog.dateEndTime();
        googleClient->updateEvent(eventId, summary, dt, enddt,calendar);
        QDate currentPage(calendar->yearShown(), calendar->monthShown(), 1);
        googleClient->fetchEvents(currentPage, calendar);
    }
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


void MainWidget::onDeleteClickedId(QString id) {
   // QString id = googleClient->eventIdMap.value(selectedLine);
    if (!id.isEmpty()) googleClient->deleteEvent(id,calendar);
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

    QSettings settings("Freelander", "Freelander");
    settings.setValue("geometry", saveGeometry());
    hide();
    event->ignore();
}

void MainWidget::handleDateClicked(const QDate &date) {
     qDebug() << "click:" << date;
    lastClickedDate = date; // Store the clicked date
}

void MainWidget::onCalendarDateActivated(const QDate &date) {

    QDateTime dateTime(date, QTime(8, 0));
    QString existingSummary;
    QString existingEventId;

    for (const auto &entry : googleClient->eventIdMap.keys()) {
        if (entry.startsWith(date.toString("dd.MM.yyyy"))) {
            //existingSummary = entry.section(" - ", 1);
            //existingEventId = googleClient->eventIdMap.value(entry);
            break;
        }
    }

    EventDialog dialog(this);
    dialog.setDateTime(dateTime);
    dialog.setEndDateTime(dateTime);
    dialog.setText(existingSummary);
    dialog.setEditMode(!existingSummary.isEmpty());

    if (!existingEventId.isEmpty()) {
        //dialog.setEventId(existingEventId);
        dialog.setWidget(this);
        qDebug() << "id exist:" << existingEventId;

    } else {

        dialog.deleteButton->hide();
        qDebug() << "id is null:" << existingEventId;
    }

    if (dialog.exec() == QDialog::Accepted) {
        QString summary = dialog.text();
        QDateTime dt = dialog.dateTime();
        QDateTime enddt = dialog.dateEndTime();

        if (!existingEventId.isEmpty()) {
            //googleClient->updateEvent(existingEventId, summary, dt, dt,calendar);
        } else {
            googleClient->createEvent(summary, dt, enddt,calendar);
        }

        QDate currentPage(calendar->yearShown(), calendar->monthShown(), 1);
        googleClient->fetchEvents(currentPage, calendar);
    }
}

void MainWidget::calendarContextMenuRequested(const QPoint &pos) {

    qDebug() << "calendarContextMenuRequested called at pos:" << pos;
    qDebug().noquote() << "\033[1;31mČervená " << pos << " chyba!\033[0m";
   // QPoint globalPos = QCursor::pos();
    QPoint globalPos = calendar->mapToGlobal(QCursor::pos());
    QDate date;
    // Attempt to get the date from the position using the internal view
    QTableView *view = calendar->findChild<QTableView*>();
    if (view) {
        qDebug() << "Internal calendar view found.";
        QModelIndex index = view->indexAt(globalPos);
        if (index.isValid()) {
            qDebug() << "Valid model index found at pos.";
            // Get the date from the model data for the index
            QVariant data = view->model()->data(index, Qt::DisplayRole);
            if (data.isValid() && data.canConvert<QDate>()) {
                date = data.toDate();
                qDebug() << "Date obtained from model index (DisplayRole):" << date << "isValid:" << date.isValid();
            } else {
                qDebug() << "Data at index (DisplayRole) is not valid or cannot convert to QDate. Trying UserRole...";
                // Try UserRole if DisplayRole fails
                data = view->model()->data(index, Qt::UserRole);
                if (data.isValid() && data.canConvert<QDate>()) {
                    date = data.toDate();
                    qDebug() << "Date obtained from model index (UserRole):" << date << "isValid:" << date.isValid();
                } else {
                    qDebug() << "Data at index (UserRole) is also not valid or cannot convert to QDate.";
                }
            }
        } else {
            qDebug() << "No valid model index found at pos.";
        }
    } else {
        qDebug() << "Internal calendar view (QTableView) not found. Falling back to selectedDate.";
    }

    if (!date.isValid()) {
        qDebug() << "Date not obtained from view. Falling back to selectedDate.";
        date = calendar->selectedDate();
        qDebug() << "Using selectedDate:" << date << "isValid:" << date.isValid();

    }

    if (!date.isValid()) {
        qDebug() << "No valid date found. Returning from context menu slot.";
        return; // Still no valid date, exit
    }

    qDebug() << "Using final determined date for context menu:" << date;

    // Fetch events for the determined date - Call member fetchEventsForDate
    googleClient->fetchEventsForDate(date, [&](const QList<QPair<QString, QString>>& events) {
        qDebug() << "fetchEventsForDate callback called. Events count:" << events.count();
        QMenu menu; // Create the context menu

        // If no events found, add a disabled action
        if (events.isEmpty()) {
            qDebug() << "No events found for date:" << date;
            QAction *noEventAction = new QAction("Žádné události", &menu);
            noEventAction->setEnabled(false);
            menu.addAction(noEventAction);
        } else {
            qDebug() << "Found" << events.count() << "events for date:" << date;
            // For each event, add an action to the menu
            for (const auto& event : events) {
                qDebug() << "Adding event to menu:" << event.first << "(" << event.second << ")";
                QAction *eventAction = new QAction(QIcon::fromTheme("edit-delete", QIcon(":/icons/delete.png")),"Delete " + event.first, &menu);
                eventAction->setData(event.second); // Store event ID in action data

                QObject::connect(eventAction, &QAction::triggered, [&,this]() {
                    QString eventId = eventAction->data().toString();
                    qDebug() << "Delete action triggered for event ID:" << eventId;
                    if (!eventId.isEmpty()) {
                        // Call the existing deleteEvent function - Call member deleteEvent
                        googleClient->deleteEvent(eventId,calendar);
                    } else {
                        qDebug() << "Event ID is empty, cannot delete.";
                    }
                });
                menu.addAction(eventAction);
            }
        }
        QPoint globalPos = QCursor::pos();
        // qDebug() << "Executing menu at global pos:" << globalPos;
        menu.exec(globalPos);

        qDebug() << "Menu execution finished.";
    });
}
void MainWidget::handleLineClick() {

    QTextCursor c = textEdit->textCursor();
    c.select(QTextCursor::LineUnderCursor);

    selectedLine = c.selectedText();
    QString id = googleClient->eventIdMap.value(selectedLine);
    if (!id.isEmpty()) googleClient->fetchEventDetails(id);
}
