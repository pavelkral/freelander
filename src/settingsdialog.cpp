#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Settings");
    setMinimumWidth(300);
    featureCheckbox = new QCheckBox("Strat with windows");
    QSettings settings("Freelander", "Freelander");
    bool isEnabled = settings.value(FeatureSettingKey, false).toBool();
    featureCheckbox->setChecked(isEnabled);
    qDebug() << " QSettings: " << FeatureSettingKey << "=" << isEnabled;
    // ----------------------------------------------------------
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

 //   connect(buttonBox, &QDialogButtonBox::apply, this, &SettingsDialog::applySettings);
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(featureCheckbox);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::isFeatureEnabled() const
{
    return featureCheckbox->isChecked();
}

void SettingsDialog::setFeatureEnabled(bool enabled)
{
    featureCheckbox->setChecked(enabled);
}

void SettingsDialog::applySettings()
{

    bool enabled = featureCheckbox->isChecked();
    QSettings settings("Freelander", "Freelander");
    settings.setValue(FeatureSettingKey, enabled);
    qDebug() << "Nastavení použito (Apply) a uloženo do QSettings: " << FeatureSettingKey << "=" << enabled;
    QString appName = "MojeAplikace"; // Název, pod kterým bude v registru
    QString appPath = QCoreApplication::applicationFilePath(); // Cesta k vaší .exe aplikaci

    // addToStartup(appName, appPath);

    // removeFromStartup(appName);

}

void SettingsDialog::addToStartup(const QString &appName, const QString &appPath) {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    settings.setValue(appName, QDir::toNativeSeparators(appPath));
}

void SettingsDialog::removeFromStartup(const QString &appName) {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    settings.remove(appName);
}

