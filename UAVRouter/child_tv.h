#ifndef CHILD_TV_H
#define CHILD_TV_H

#include <QDialog>
#include <qstandarditemmodel.h>
#include <list>

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

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    QString getFileNameFromPath(QString strPath);
    void ReDisplyOnTabView();

    Ui::child_tv *ui;
    QStandardItemModel * ptrmodel;
    //std::set<QString> setDeletedRows;
    //std::set<QString> setAddedRows;
    //std::set<QString> setInputFiles;
    MainWindow* ptrMainDlg;
    QString strCurPath;

private:
    // inner class
    // to check the given string if exist in the base string list
    class is_exist
    {
    public:
        is_exist(const std::list<QString>& listStrs)
        {
            listBaseStrs = listStrs;
        }
        ~is_exist(){}
        bool operator()(const QString& strVle)
        {
            bool bExistFlag = false;
            for(std::list<QString>::iterator iter = listBaseStrs.begin();
                iter != listBaseStrs.end(); ++iter)
            {
                QString strBaseStr = *iter;
                if(strBaseStr.toLower() == strVle.toLower())
                {
                    bExistFlag = true;
                    break;
                }
            }
            return bExistFlag;
        }
    private:
        std::list<QString> listBaseStrs;
    };

};

#endif // CHILD_TV_H
