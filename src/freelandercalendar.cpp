#include "freelandercalendar.h"

#include <QToolButton>
#include <QCalendarWidget>
#include <QPainter>
#include <QDate>
#include <QRect>
#include <QDebug>

FreelanderCalendar::FreelanderCalendar(QWidget *parent)
    : QCalendarWidget(parent)
{

}


void FreelanderCalendar::setNavigationIcons(const QIcon &prevIcon, const QIcon &nextIcon)
{
    QToolButton *prevButton = findChild<QToolButton*>("qt_calendar_prevmonth");
    QToolButton *nextButton = findChild<QToolButton*>("qt_calendar_nextmonth");

    if (prevButton) {
        prevButton->setIcon(prevIcon);

    } else {
        qWarning(".");
    }

    if (nextButton) {
        nextButton->setIcon(nextIcon);
        // nextButton->setIconSize(QSize(24, 24));
    } else {
        qWarning("arrow icons.");
    }

}

void FreelanderCalendar::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    qDebug().noquote() << "\033[1;31m Call Paint cell " << date << " paint !\033[0m";

    QCalendarWidget::paintCell(painter, rect, date);

    if (date == QDate::currentDate()) {
        qDebug() <<"paint calendar." << date;
        painter->save();
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter->setPen(pen);

        int margin = 2;
        painter->drawEllipse(rect.adjusted(margin, margin, -margin, -margin));
        painter->restore();
    }
    else{
        qDebug() <<"paint current date error." << date;
    }
}


