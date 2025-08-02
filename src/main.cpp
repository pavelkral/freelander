#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include "MainWidget.h"
#include "utils/logger.h"

int main(int argc, char* argv[])
{
	qputenv("QT_SCALE_FACTOR", QByteArray("0.9"));
	qputenv("QT_ENABLE_HIGHDPI_SCALING", QByteArray("1"));

	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication a(argc, argv);
	QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
	qInstallMessageHandler(myMessageHandler);
	Logger::instance().setEnabled(true);

#ifndef QT_DEBUG
	qDebug() << "Release mode";
    Logger::instance().setDebug(false);
	//todo: add release mode specific settings or optimizations here
#else
	qDebug() << "Debug mode";
	Logger::instance().setDebug(true);
#endif
    //Logger::instance().setDebug(true);
	a.setStyle(QStyleFactory::create("Fusion"));

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(nullptr, "Error", "System try not found.");
		return 1;
	}

	MainWidget w;
	w.resize(200, 380);
	w.show();

    // QObject::connect(qApp, &QGuiApplication::applicationStateChanged,
    //                  [&](Qt::ApplicationState state) {
    //                      if (state == Qt::ApplicationActive) {
    //                          //qDebug() << "System probably woke up";
    //                          //Logger::instance().reopenLogFile();
    //                          Logger::instance().log("Resumed from sleep",Qt::green);
    //                      }
    //                  });


	return a.exec();
}
