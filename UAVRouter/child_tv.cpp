#include "child_tv.h"
#include "ui_child_tv.h"
#include "mainwindow.h"

#include <qmessagebox.h>
#include <qfiledialog.h>


child_tv::child_tv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::child_tv)
{
    ui->setupUi(this);

    // set the child dialog's appearance
    this->setFixedSize(this->width(), this->height());
    Qt::WindowFlags type = Qt::Dialog;
    type |= Qt::WindowMinimizeButtonHint;
    this->setWindowFlags(type);

    ptrmodel = new QStandardItemModel();
    ptrmodel->setColumnCount(2);
    ptrmodel->setHeaderData(0, Qt::Horizontal, "file name");
    ptrmodel->setHeaderData(1, Qt::Horizontal, "file path");
    ui->tableView->setModel(ptrmodel);
    ui->tableView->setColumnWidth(0, ui->tableView->width()/2);
    ui->tableView->setColumnWidth(1, ui->tableView->width());
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);

    ptrMainDlg = (MainWindow*)parent;

    ReDisplyOnTabView();
    strCurPath = ".";
}

child_tv::~child_tv()
{
    for(int i=0; i<ptrmodel->rowCount(); ++i)
    {
        for(int j=0; j<ptrmodel->columnCount(); ++j)
        {
            delete ptrmodel->item(i, j);
        }
    }

    delete ptrmodel;

    delete ui;
}

QString child_tv::getFileNameFromPath(QString strPath)
{
    QString strName = "";
    if(strPath.isEmpty())
    {
        return strName;
    }
    strPath.replace("\\", "/");
    strName = strPath.right(strPath.length() - strPath.lastIndexOf("/") - 1);
    return strName;
}

// delete selected item in tableview
void child_tv::on_pushButton_clicked()
{
    QItemSelectionModel* selections = ui->tableView->selectionModel();
    QModelIndexList selected = selections->selectedRows();

    std::set<int> setRows;
    foreach (QModelIndex idx, selected) {
        setRows.insert(idx.row());
        QString strSelected = ptrmodel->data(ptrmodel->index(idx.row(), 1)).toString();
        ptrMainDlg->listInputKmlFile.remove(strSelected);
    }

    for(std::set<int>::reverse_iterator riter = setRows.rbegin();
        riter != setRows.rend(); ++riter)
    {
        ptrmodel->removeRow(*riter);
    }
}

// add selected files and display the files in tableview
void child_tv::on_pushButton_2_clicked()
{
    QFileDialog dlg(this,
                    "Choose Flight Region Defination File(s)...",
                    strCurPath,
                    "KML (*.kml *.KML)");
    dlg.setFileMode(QFileDialog::ExistingFiles);
    if(dlg.exec() == QFileDialog::Accepted)
    {
        strCurPath = dlg.directory().path();
        QStringList slist = dlg.selectedFiles();
        for(int i=0; i<slist.size(); ++i)
        {
            //setAddedRows.insert(slist.at(i));
            is_exist checker(ptrMainDlg->listInputKmlFile);
            if(!checker(slist.at(i)))
            {
                ptrMainDlg->listInputKmlFile.push_back(slist.at(i));

                QStandardItem* ptrItem1 = new QStandardItem(getFileNameFromPath(slist.at(i)));
                QStandardItem* ptrItem2 = new QStandardItem(slist.at(i));
                QList<QStandardItem*> item;
                item<<ptrItem1<<ptrItem2;
                ptrmodel->appendRow(item);
            }
        }
    }
}

void child_tv::ReDisplyOnTabView()
{
    if(0 < ptrmodel->rowCount())
    {
        ptrmodel->removeRows(0, ptrmodel->rowCount());
    }

    for(std::list<QString>::iterator iter = ptrMainDlg->listInputKmlFile.begin();
        iter != ptrMainDlg->listInputKmlFile.end(); ++iter)
    {
        QStandardItem* ptrItem1 = new QStandardItem(getFileNameFromPath(*iter));
        QStandardItem* ptrItem2 = new QStandardItem(*iter);
        QList<QStandardItem*> item;
        item<<ptrItem1<<ptrItem2;
        ptrmodel->appendRow(item);
    }

    ui->tableView->show();
}

void child_tv::on_pushButton_3_clicked()
{
    QItemSelectionModel* selections = ui->tableView->selectionModel();
    QModelIndexList selected = selections->selectedRows();
    if(1 < selected.size() || 0 == selected.size())
    {
        QMessageBox::warning(this,
                             "Warning",
                             "please select one row...",
                             QMessageBox::Ok|QMessageBox::Cancel,
                             QMessageBox::Ok);
        return;
    }

    QModelIndex idx = selected.at(0);

    //selected file dialog
    QFileDialog dlg(this,
                    "Choose Flight Region Defination File(s)...",
                    strCurPath,
                    "KML (*.kml *.KML)");
    dlg.setFileMode(QFileDialog::ExistingFiles);
    if(dlg.exec() == QFileDialog::Accepted)
    {
        strCurPath = dlg.directory().path();
        QStringList slist = dlg.selectedFiles();
        for(int i=0; i<slist.size(); ++i)
        {
            QStandardItem* ptrItem1 = new QStandardItem(getFileNameFromPath(slist.at(i)));
            QStandardItem* ptrItem2 = new QStandardItem(slist.at(i));
            QList<QStandardItem*> item;
            item<<ptrItem1<<ptrItem2;
            //ptrMainDlg->listInputKmlFile.insert()
            ptrmodel->insertRow(idx.row(), item);
        }
    }

    // keep in line with the child dialog
    ptrMainDlg->listInputKmlFile.clear();
    for(int i=0; i<ptrmodel->rowCount(); ++i)
    {
        QString strPath = ptrmodel->data(ptrmodel->index(i, 1)).toString();
        ptrMainDlg->listInputKmlFile.push_back(strPath);
    }

}

void child_tv::on_pushButton_4_clicked()
{
    this->hide();
}
