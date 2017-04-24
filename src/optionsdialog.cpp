#include "optionsdialog.h"
#include "ui_optionsdialog.h"

optionsDialog::optionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::optionsDialog)
{
    ui->setupUi(this);
    showSettings();
}

optionsDialog::~optionsDialog()
{
    delete ui;
}
void optionsDialog::showSettings()
{
    QSettings set;
    set.beginGroup("Main");
    ui->StartOnBootCheckBox->setChecked(set.value("StartOnBoot", true).toBool());
    ui->StartMinimalCheckBox->setChecked(set.value("StartMinimal", false).toBool());
    ui->RestartSessionCheckBox->setChecked(set.value("RestortSession", true).toBool());
    ui->CheckUpdateCheckBox->setChecked(set.value("CheckUpdate", true).toBool());
    //set.setValue();
    set.endGroup();

}
void optionsDialog::applySettings()
{
    QSettings set;
    set.beginGroup("Main");
    set.setValue("StartOnBoot", ui->StartOnBootCheckBox->isChecked());
    set.setValue("StartMinimal", ui->StartMinimalCheckBox->isChecked());
    set.setValue("RestortSession", ui->RestartSessionCheckBox->isChecked());
    set.setValue("CheckUpdate", ui->CheckUpdateCheckBox->isChecked());
    set.endGroup();
}
