#ifndef CLICKABLETEXTEDIT_H
#define CLICKABLETEXTEDIT_H

#include <QMouseEvent>
#include <QTextEdit>


class ClickableTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit ClickableTextEdit(QWidget *parent = nullptr);

signals:
    void lineDoubleClicked(const QString &lineText);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            QTextCursor cursor = cursorForPosition(event->pos());
            cursor.select(QTextCursor::LineUnderCursor);
            emit lineDoubleClicked(cursor.selectedText().trimmed());
        }
        QTextEdit::mouseDoubleClickEvent(event); // Call base class implementation
    }
};
#endif // CLICKABLETEXTEDIT_H
