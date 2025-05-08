#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSettings>
#include <QVBoxLayout>

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

private:
    QCheckBox *featureCheckbox;
    QDialogButtonBox *buttonBox;
    QVBoxLayout *mainLayout;

    const QString FeatureSettingKey = "feature/enable_super_feature";
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
