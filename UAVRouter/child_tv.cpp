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

    this->setFixedSize(this->width(), this->height());

    this->setDeletedRows.clear();
    this->setAddedRows.clear();

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

    reDisplyOnTabView();
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
    addToSrc();

    QItemSelectionModel* selections = ui->tableView->selectionModel();
    QModelIndexList selected = selections->selectedRows();

    std::set<int> setRows;
    foreach (QModelIndex idx, selected) {
        setRows.insert(idx.row());
        setDeletedRows.insert(ptrmodel->data(ptrmodel->index(idx.row(), 1)).toString());
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
    deleteFromSrc();

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
            setAddedRows.insert(slist.at(i));

            std::set<QString>::iterator iterFind = ptrMainDlg->setInputKmlFile.find(slist.at(i));
            if(iterFind == ptrMainDlg->setInputKmlFile.end())
            {
                QStandardItem* ptrItem1 = new QStandardItem(getFileNameFromPath(slist.at(i)));
                QStandardItem* ptrItem2 = new QStandardItem(slist.at(i));
                QList<QStandardItem*> item;
                item<<ptrItem1<<ptrItem2;
                ptrmodel->appendRow(item);
            }
        }
    }
}

void child_tv::on_buttonBox_rejected()
{
    setDeletedRows.clear();
    setAddedRows.clear();
}

void child_tv::on_buttonBox_accepted()
{
    deleteFromSrc();
    addToSrc();

    setDeletedRows.clear();
    setAddedRows.clear();
}

void child_tv::reDisplyOnTabView()
{
    if(0 < ptrmodel->rowCount())
    {
        ptrmodel->removeRows(0, ptrmodel->rowCount());
    }

    for(std::set<QString>::iterator iter = ptrMainDlg->setInputKmlFile.begin();
        iter != ptrMainDlg->setInputKmlFile.end(); ++iter)
    {
        QStandardItem* ptrItem1 = new QStandardItem(getFileNameFromPath(*iter));
        QStandardItem* ptrItem2 = new QStandardItem(*iter);
        QList<QStandardItem*> item;
        item<<ptrItem1<<ptrItem2;
        ptrmodel->appendRow(item);
    }

    ui->tableView->show();
}

void child_tv::deleteFromSrc()
{
    if(0 >= setDeletedRows.size())
        return;

    for(std::set<QString>::iterator iter = setDeletedRows.begin();
        iter != setDeletedRows.end(); iter++)
    {
        if(ptrMainDlg->setInputKmlFile.find(*iter) != ptrMainDlg->setInputKmlFile.end())
        {
            ptrMainDlg->setInputKmlFile.erase(ptrMainDlg->setInputKmlFile.find(*iter));
        }
    }
}

void child_tv::addToSrc()
{
    if(0 >= setAddedRows.size())
        return;

    for(std::set<QString>::iterator iter = setAddedRows.begin();
        iter != setAddedRows.end(); ++iter)
    {
        ptrMainDlg->setInputKmlFile.insert(*iter);
    }
}
