#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Settings");
    setMinimumWidth(300);
    startUpCheckbox = new QCheckBox("Strat with windows");

    QSettings settings(settingsFilePath, QSettings::IniFormat);
    bool isEnabled = settings.value(autoStartKey, false).toBool();
    startUpCheckbox->setChecked(isEnabled);
    qDebug() << " QSettings: " << autoStartKey << "=" << isEnabled;

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startUpCheckbox);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::isFeatureEnabled() const
{
    return startUpCheckbox->isChecked();
}

void SettingsDialog::setFeatureEnabled(bool enabled)
{
    startUpCheckbox->setChecked(enabled);
}

void SettingsDialog::applySettings()
{
    bool enabled = startUpCheckbox->isChecked();
    QSettings settings(settingsFilePath, QSettings::IniFormat);
    settings.setValue(autoStartKey, enabled);
    qDebug() << " QSettings: " << autoStartKey << "=" << enabled;
    QString appName = "Freelander"; //  register name
    QString appPath = QCoreApplication::applicationFilePath(); // path
    if (enabled) {
         addToStartup(appName, appPath);
    } else {
         removeFromStartup(appName);
    }
}
void SettingsDialog::addToStartup(const QString &appName, const QString &appPath) {

#ifdef Q_OS_WIN

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    settings.setValue(appName, QDir::toNativeSeparators(appPath));
    //qDebug() << "Added to Windows startup:" << appName << "Path:" << appPath;

#elif defined(Q_OS_LINUX)

    //(using .desktop files)
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QDir autostartDir(autostartPath);
    if (!autostartDir.exists()) {
        if (!autostartDir.mkpath(".")) {
            qWarning() << "Could not create autostart directory:" << autostartPath;
            return;
        }
        qDebug() << "Created autostart directory:" << autostartPath;
    }

    //(replace spaces)
    QString desktopFileName = appName.simplified().replace(" ", "_") + ".desktop";
    QString desktopFilePath = autostartDir.filePath(desktopFileName);

    QFile desktopFile(desktopFilePath);
    if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&desktopFile);
        out << "[Desktop Entry]\n";
        out << "Type=Application\n";
        out << "Name=" << appName << "\n";

        // if it contains spaces

        out << "Exec=\"" << QDir::toNativeSeparators(appPath) << "\"\n";
        out << "Terminal=false\n";
        out << "Hidden=false\n";   // Not hidden
        out << "NoDisplay=false\n"; // Display in menus
        out << "X-GNOME-Autostart-enabled=true\n"; // Common for GNOME-based environments

        // add an Icon entry :
        // out << "Icon=/path/to/your/icon.png\n";
        desktopFile.close();
        qDebug() << "Added to Linux startup. Created desktop file:" << desktopFilePath;
    } else {
        qWarning() << "Could not create .desktop file:" << desktopFilePath << "Error:" << desktopFile.errorString();
    }

#else
    qWarning() << "addToStartup: Unsupported operating system.";
#endif
}
void SettingsDialog::removeFromStartup(const QString &appName) {
#ifdef Q_OS_WIN

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    settings.remove(appName);
    //qDebug() << "Removed from Windows startup:" << appName;

#elif defined(Q_OS_LINUX)
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";

    QString desktopFileName = appName.simplified().replace(" ", "_") + ".desktop";
    QString desktopFilePath = QDir(autostartPath).filePath(desktopFileName);

    if (QFile::exists(desktopFilePath)) {
        if (QFile::remove(desktopFilePath)) {
            qDebug() << "Removed from Linux startup. Deleted desktop file:" << desktopFilePath;
        } else {
            qWarning() << "Could not remove .desktop file:" << desktopFilePath;
        }
    } else {
        qDebug() << "removeFromStartup: .desktop file not found, nothing to remove:" << desktopFilePath;
    }

#else
    qWarning() << "removeFromStartup: Unsupported operating system.";
#endif

}
