#include "freelandercalendar.h"

#include <QToolButton>
#include <QCalendarWidget>
#include <QPainter>
#include <QDate>
#include <QRect>
#include <QDebug>

FreelanderCalendar::FreelanderCalendar(QWidget *parent) : QCalendarWidget(parent){

}

void FreelanderCalendar::setNavigationIcons(const QIcon &prevIcon, const QIcon &nextIcon){
    QToolButton *prevButton = findChild<QToolButton*>("qt_calendar_prevmonth");
    QToolButton *nextButton = findChild<QToolButton*>("qt_calendar_nextmonth");

    if (prevButton) {
        prevButton->setIcon(prevIcon);

    } else {
        qWarning("arrow icons err.");
    }

    if (nextButton) {
        nextButton->setIcon(nextIcon);
        // nextButton->setIconSize(QSize(24, 24));
    } else {
        qWarning("arrow icons err.");
    }

}

void FreelanderCalendar::paintCell(QPainter *painter, const QRect &rect, QDate date) const {
    //qDebug().noquote() << "\033[1;31m Call Paint cell " << date << " !\033[0m";
    QCalendarWidget::paintCell(painter, rect, date);
    QColor backgroundColor(0, 0, 0, 8);
    painter->setBrush(backgroundColor);

    if (date == QDate::currentDate()) {
        qDebug() <<"paint calendar." << date;

        painter->save();
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter->setPen(pen);
        int margin = 2;
        int side = qMin(rect.width(), rect.height()) - 2 * margin;
        if (side < 0) side = 0;
        QRect square(0, 0, side, side);
        square.moveCenter(rect.center());
        painter->drawEllipse(square);
        painter->restore();
    }
    else{

        // qDebug() <<"paint current date error." << date;
    }
}


