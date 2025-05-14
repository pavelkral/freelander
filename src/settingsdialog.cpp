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
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    settings.setValue(appName, QDir::toNativeSeparators(appPath));
}

void SettingsDialog::removeFromStartup(const QString &appName) {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    settings.remove(appName);
}

