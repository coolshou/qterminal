#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class updatedialog;
}

class updatedialog : public QDialog
{
    Q_OBJECT

public:
    explicit updatedialog(QWidget *parent = 0);
    ~updatedialog();

private:
    Ui::updatedialog *ui;
};

#endif // UPDATEDIALOG_H
