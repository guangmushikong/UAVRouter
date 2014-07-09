/********************************************************************************
** Form generated from reading UI file 'child_tv.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHILD_TV_H
#define UI_CHILD_TV_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_child_tv
{
public:
    QDialogButtonBox *buttonBox;
    QTableView *tableView;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *child_tv)
    {
        if (child_tv->objectName().isEmpty())
            child_tv->setObjectName(QStringLiteral("child_tv"));
        child_tv->setEnabled(true);
        child_tv->resize(400, 300);
        buttonBox = new QDialogButtonBox(child_tv);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(210, 240, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tableView = new QTableView(child_tv);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setGeometry(QRect(20, 20, 351, 192));
        tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        tableView->setSortingEnabled(true);
        pushButton = new QPushButton(child_tv);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(20, 240, 81, 23));
        pushButton_2 = new QPushButton(child_tv);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(120, 240, 75, 23));

        retranslateUi(child_tv);
        QObject::connect(buttonBox, SIGNAL(accepted()), child_tv, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), child_tv, SLOT(reject()));

        QMetaObject::connectSlotsByName(child_tv);
    } // setupUi

    void retranslateUi(QDialog *child_tv)
    {
        child_tv->setWindowTitle(QApplication::translate("child_tv", "Management of input files", 0));
        pushButton->setText(QApplication::translate("child_tv", "Delete Item", 0));
        pushButton_2->setText(QApplication::translate("child_tv", "Add Item", 0));
    } // retranslateUi

};

namespace Ui {
    class child_tv: public Ui_child_tv {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHILD_TV_H
