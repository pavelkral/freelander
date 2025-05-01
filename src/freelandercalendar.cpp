#include "src/freelandercalendar.h"

#include <QToolButton>

#include <QDebug>
FreelanderCalendar::FreelanderCalendar(QWidget *parent)
    : QCalendarWidget(parent)
{

}

void FreelanderCalendar::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    qDebug() << "Metoda paintCell:" << date;
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
        qDebug() <<"paint calendar date." << date;
    }
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
        qWarning("Nepodařilo se najít tlačítko dalšího měsíce.");
    }

}

void FreelanderCalendar::paintEvent(QPaintEvent *)
{

}
