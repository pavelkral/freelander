#ifndef CALENDARDELEGATE_H
#define CALENDARDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QDate>
#include <QAbstractItemModel>

class CalendarDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    CalendarDelegate(QObject *parent = nullptr) ;


    void setHighlightedDates(const QMap<QDate, QString> &dates) {
        highlighted = dates;
        for (auto it = highlighted.begin(); it != highlighted.end(); ++it) {
            qDebug() << "Datum:" << it.key().toString(Qt::ISODate) << "Text:" << it.value();
        }
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override ;


private:
    QMap<QDate, QString> highlighted;
};
#endif // CUSTOMCALENDARDELEGATE_H
