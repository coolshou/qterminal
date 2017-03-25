#include "updatedialog.h"
#include "ui_updatedialog.h"

updatedialog::updatedialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updatedialog)
{
    ui->setupUi(this);
}

updatedialog::~updatedialog()
{
    delete ui;
}
