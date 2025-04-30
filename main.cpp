#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include "MainWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    a.setStyle(QStyleFactory::create("Fusion"));
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Error", "System try not found.");
        return 1;
    }
    MainWidget w;
    w.resize(300,450);
    w.show();
    return a.exec();
}
