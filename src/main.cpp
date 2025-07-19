#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include "MainWidget.h"
#include "logger.h"

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
	Logger::instance().setDebug(true);

	#ifndef QT_DEBUG
		qDebug() << "Release mode";
		Logger::instance().setDebug(false);
		//todo: add release mode specific settings or optimizations here
	#else
		qDebug() << "Debug mode";
		Logger::instance().setDebug(true);
	#endif

	a.setStyle(QStyleFactory::create("Fusion"));

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(nullptr, "Error", "System try not found.");
		return 1;
	}

	MainWidget w;
	w.resize(200, 380);
	w.show();
	return a.exec();
}
