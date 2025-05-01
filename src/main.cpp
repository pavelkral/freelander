#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include "MainWidget.h"

int main(int argc, char *argv[])
{
    qputenv("QT_SCALE_FACTOR", QByteArray("1"));
    qputenv("QT_ENABLE_HIGHDPI_SCALING", QByteArray("1"));

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    a.setStyle(QStyleFactory::create("Fusion"));

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Error", "System try not found.");
        return 1;
    }

    MainWidget w;
    w.resize(200,380);
    w.show();
    return a.exec();
}
