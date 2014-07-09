#ifndef CHILD_TV_H
#define CHILD_TV_H

#include <QDialog>
#include <qstandarditemmodel.h>
#include <set>

namespace Ui {
class child_tv;
}

class MainWindow;

class child_tv : public QDialog
{
    Q_OBJECT

public:
    explicit child_tv(QWidget *parent = 0);
    ~child_tv();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    QString getFileNameFromPath(QString strPath);
    void reDisplyOnTabView();
    void deleteFromSrc();
    void addToSrc();

    Ui::child_tv *ui;
    QStandardItemModel * ptrmodel;
    std::set<QString> setDeletedRows;
    std::set<QString> setAddedRows;
    //std::set<QString> setInputFiles;

    MainWindow* ptrMainDlg;

    QString strCurPath;

};

#endif // CHILD_TV_H
