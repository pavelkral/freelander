#include "calendardelegate.h"

CalendarDelegate::CalendarDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void CalendarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    painter->save();

    QStyledItemDelegate::paint(painter, option, index);


    QVariant dateData = index.data(Qt::DisplayRole);
    QDate cellDate;

    if (dateData.typeId() == QMetaType::QDate) {
        cellDate = dateData.toDate();
    } else if (dateData.typeId() == QMetaType::QString) {

        cellDate = QDate::fromString(dateData.toString(), Qt::ISODate);
    }

    if (cellDate.isValid() && cellDate == QDate::currentDate()) {

        QPen pen(Qt::red);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);


        const QRect &rect = option.rect;

        int margin = 2;
        painter->drawEllipse(rect.adjusted(margin, margin, -margin, -margin));
    }


    painter->restore();


    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    QDate date = index.data(Qt::UserRole).toDate();

    painter->save();

    QRect r = option.rect;

    // Zvýraznění dat s připomínkami (na pozadí)
    if (highlighted.contains(date)) {
        qDebug() << "Vykresluji den:" << date << "Zvýrazněný:" << highlighted.contains(date);
        painter->setBrush(QColor(255, 215, 0, 100));  // Světlá žlutá pro zvýraznění
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(r.center(), 6, 6);  // Elipsa kolem zvýrazněného data
    }

    // Výběr
    if (opt.state & QStyle::State_Selected) {
        painter->setBrush(QColor(255, 255, 255, 30)); // Poloprůhledná bílá barva pro výběr
        painter->setPen(Qt::NoPen);
        painter->drawRect(r); // Vykreslí obdélník pro výběr
    }

    // Dnešní den
    if (date == QDate::currentDate()) {
        painter->setBrush(QColor(0, 120, 255, 100)); // Modrá pro dnešní den
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(r.center(), 10, 10); // Vykreslí větší elipsu pro dnešní den
    }

    // Text
    painter->setPen(Qt::white);
    painter->drawText(r, Qt::AlignCenter, opt.text);

    painter->restore();
}
