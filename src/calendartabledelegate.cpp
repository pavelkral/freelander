#include "calendartabledelegate.h"

CalendarTableDelegate::CalendarTableDelegate(QObject *parent) : QStyledItemDelegate(parent){
}

void CalendarTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    painter->save();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    QDate cellDate = index.data(Qt::UserRole).toDate();
    if (!cellDate.isValid()) {
        QVariant displayData = index.data(Qt::DisplayRole);
        if (displayData.typeId() == QMetaType::QDate) {
            cellDate = displayData.toDate();
        } else if (displayData.typeId() == QMetaType::QString) {

            cellDate = QDate::fromString(displayData.toString(), Qt::ISODate);
            if (!cellDate.isValid()) {
                bool ok;
                int dayOfMonth = displayData.toString().toInt(&ok);
                if (ok && opt.widget) {

                }
            }
        }
    }
    QStyledItemDelegate::paint(painter, opt, index);

    QRect r = opt.rect;

    if (cellDate.isValid() && highlighted.contains(cellDate)) {
        qDebug() << "day:" << cellDate << "highlite:" << highlighted.contains(cellDate);
        painter->setBrush(QColor(255, 215, 0, 200));
        int indicatorSize = 6;
        painter->drawEllipse(QPoint(r.right() - indicatorSize - 2, r.bottom() - indicatorSize - 2), indicatorSize / 2, indicatorSize / 2);
    }

    if (cellDate.isValid() && cellDate == QDate::currentDate()) {
        QPen pen(Qt::red);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        int margin = 1;
        painter->drawEllipse(r.adjusted(margin, margin, -margin, -margin));
    }

    if (opt.state & QStyle::State_Selected) {
        painter->setBrush(opt.palette.highlight());
        painter->setOpacity(0.4);
        painter->drawRect(r);
        painter->setOpacity(1.0);
    }

    painter->restore();
}
