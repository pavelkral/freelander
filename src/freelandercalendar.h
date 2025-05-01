
#ifndef FREELANDERCALENDAR_H
#define FREELANDERCALENDAR_H

#include <QCalendarWidget>
#include <QPainter>
#include <QDate>
#include <QRect>
#include <QColor>
#include <QPen>

class FreelanderCalendar : public QCalendarWidget
{
    Q_OBJECT

public:
    FreelanderCalendar(QWidget *parent = nullptr);

    void setNavigationIcons(const QIcon &prevIcon, const QIcon &nextIcon);
protected:
     void paintEvent(QPaintEvent *) override;
     virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const ;
};

#endif // FREELANDERCALENDAR_H


