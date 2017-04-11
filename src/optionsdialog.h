#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class optionsDialog;
}

class optionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit optionsDialog(QWidget *parent = 0);
    ~optionsDialog();

    void showSettings();
    void applySettings();

private:
    Ui::optionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
