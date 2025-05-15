#include "freelandercalendar.h"

#include <QToolButton>
#include <QCalendarWidget>
#include <QPainter>
#include <QDate>
#include <QRect>
#include <QDebug>
#include <QTextCharFormat>

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

    QTextCharFormat format = dateTextFormat(date);
    format.setFontPointSize(12);

    if (format.hasProperty(QTextFormat::BackgroundBrush)) {
        painter->fillRect(rect, format.background());
    } else {
        painter->fillRect(rect, Qt::transparent);
    }

    painter->save();

    QColor finalTextColor;
    bool colorSetByFormat = false;

    if (format.hasProperty(QTextFormat::ForegroundBrush)) {
        finalTextColor = format.foreground().color();
        colorSetByFormat = true;
    }
    if((date.dayOfWeek() == Qt::Sunday || date.dayOfWeek() == Qt::Saturday) && !colorSetByFormat){
         finalTextColor = Qt::red;
    }
    else if (!colorSetByFormat) {
        bool isCurrentMonthDate = (date.month() == monthShown() && date.year() == yearShown());
        if (!isCurrentMonthDate) {
            finalTextColor = QColor(200, 200, 200, 150);
        } else {
            finalTextColor = Qt::white;
        }
    }

    painter->setPen(finalTextColor);

    QFont cellFont = this->font();
    if (format.isValid()) {
        if (format.hasProperty(QTextFormat::FontFamily)) cellFont.setFamily(format.fontFamily());
        if (format.hasProperty(QTextFormat::FontPointSize)) cellFont.setPointSize(format.fontPointSize());
        if (format.hasProperty(QTextFormat::FontFixedPitch)) cellFont.setFixedPitch(format.fontFixedPitch());
      ///  if (format.hasProperty(QTextFormat::FontBold)) cellFont.setBold(format.fontBold());
        if (format.hasProperty(QTextFormat::FontItalic)) cellFont.setItalic(format.fontItalic());
        if (format.hasProperty(QTextFormat::FontUnderline)) cellFont.setUnderline(format.fontUnderline());
        if (format.hasProperty(QTextFormat::FontStrikeOut)) cellFont.setStrikeOut(format.fontStrikeOut());
        if (format.hasProperty(QTextFormat::FontWeight)) cellFont.setWeight(static_cast<QFont::Weight>(format.fontWeight()));
    }

    painter->setFont(cellFont);
    painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
    painter->restore();

    bool isCurrentMonthDate = (date.month() == monthShown() && date.year() == yearShown());

    if (date == QDate::currentDate() && isCurrentMonthDate) {
        painter->save();
        QPen ellipsePen(Qt::red);
        ellipsePen.setWidth(1);
        painter->setPen(ellipsePen);
        painter->setBrush(Qt::NoBrush);

        int margin = 2;
        float side = qMin(rect.width(), rect.height()) - 2 * margin;
        if (side > 0) {
            qreal ellipseX = rect.left() + (rect.width() - side) / 2.0;
            qreal ellipseY = rect.top() + (rect.height() - side) / 2.0;
            QRectF ellipseRect(ellipseX, ellipseY, side, side);
            painter->drawEllipse(ellipseRect);
        }
        painter->restore();
    }
}



