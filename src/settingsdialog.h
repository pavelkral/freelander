#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSettings>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    bool isFeatureEnabled() const;
    void setFeatureEnabled(bool enabled);
    void applySettings();
    void addToStartup(const QString& appName, const QString& appPath);
    void removeFromStartup(const QString& appName);


private:
    QCheckBox *featureCheckbox;
    QDialogButtonBox *buttonBox;
    QVBoxLayout *mainLayout;
    QString settingsFilePath = QCoreApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    const QString autoStartKey = "autostart/enable_start";
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
