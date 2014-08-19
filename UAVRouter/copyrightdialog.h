#ifndef COPYRIGHTDIALOG_H
#define COPYRIGHTDIALOG_H

#include <QDialog>

namespace Ui {
class CopyRightDialog;
}

class CopyRightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyRightDialog(QWidget *parent = 0);
    ~CopyRightDialog();

private:
    Ui::CopyRightDialog *ui;
};

#endif // COPYRIGHTDIALOG_H
