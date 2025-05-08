#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Settings");
    setMinimumWidth(300);
    featureCheckbox = new QCheckBox("Strat with windows"); // Změněn text pro odlišení od JSON příkladu
    QSettings settings("Freelander", "Freelander");
    bool isEnabled = settings.value(FeatureSettingKey, false).toBool();
    featureCheckbox->setChecked(isEnabled);
    qDebug() << " QSettings: " << FeatureSettingKey << "=" << isEnabled;
    // ----------------------------------------------------------
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // Propojení signálu apply() s naším vlastním slotem applySettings()
 //   connect(buttonBox, &QDialogButtonBox::apply, this, &SettingsDialog::applySettings);


    // Vytvoření hlavního layoutu
    mainLayout = new QVBoxLayout(this);

    // Přidání checkboxu a button boxu do layoutu
    mainLayout->addWidget(featureCheckbox);
    mainLayout->addStretch(); // Přidá pružinu, která odsune button box dolů
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
    // --- Uložení nastavení do QSettings při kliknutí na Apply ---
    bool enabled = featureCheckbox->isChecked();

    QSettings settings("Freelander", "Freelander");
    settings.setValue(FeatureSettingKey, enabled);
    // QSettings automaticky ukládá změny v závislosti na platformě a konfiguraci.
    // Pro okamžité uložení můžete použít settings.sync();
    // settings.sync();
    qDebug() << "Nastavení použito (Apply) a uloženo do QSettings: " << FeatureSettingKey << "=" << enabled;

    // V reálné aplikaci byste zde aplikovali změny do běžící aplikace
    // (např. změnili nějaké chování na základě nového nastavení).
    // Dialog zůstává otevřený.
    // ----------------------------------------------------------
}

