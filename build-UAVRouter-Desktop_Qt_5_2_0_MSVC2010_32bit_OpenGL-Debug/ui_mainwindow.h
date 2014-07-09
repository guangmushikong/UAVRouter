/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_4;
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QLabel *label;
    QLineEdit *editFocus;
    QLabel *label_2;
    QLineEdit *editCamWidth;
    QLineEdit *editCamHeight;
    QLabel *label_3;
    QLabel *label_13;
    QLineEdit *editPixelsize;
    QGroupBox *groupBox_2;
    QLabel *label_6;
    QLineEdit *editDatumHeight;
    QGroupBox *groupBox_3;
    QLabel *label_4;
    QLineEdit *editGEDist;
    QLabel *label_5;
    QLineEdit *editRedudantBaseline;
    QLabel *label_7;
    QLineEdit *editFlightHeight;
    QLabel *label_11;
    QLabel *label_12;
    QLineEdit *editoverlaprate;
    QLineEdit *editOverlapSide;
    QCommandLinkButton *cmdDesignStart;
    QTextEdit *textOutputFile;
    QLabel *label_10;
    QToolButton *toolButtonOutputSelect;
    QGroupBox *groupBox_4;
    QTextEdit *textRegionFile;
    QToolButton *toolButton_Region;
    QToolButton *toolButton_Region_2;
    QPushButton *btn_unittest;
    QGroupBox *groupBox_5;
    QLabel *label_8;
    QToolButton *toolButtonAirport;
    QLabel *labelAirportLocation;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(621, 435);
        action_4 = new QAction(MainWindow);
        action_4->setObjectName(QStringLiteral("action_4"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 311, 141));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 121, 16));
        editFocus = new QLineEdit(groupBox);
        editFocus->setObjectName(QStringLiteral("editFocus"));
        editFocus->setGeometry(QRect(210, 20, 91, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 50, 141, 16));
        editCamWidth = new QLineEdit(groupBox);
        editCamWidth->setObjectName(QStringLiteral("editCamWidth"));
        editCamWidth->setGeometry(QRect(210, 50, 91, 20));
        editCamHeight = new QLineEdit(groupBox);
        editCamHeight->setObjectName(QStringLiteral("editCamHeight"));
        editCamHeight->setGeometry(QRect(210, 80, 91, 20));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 80, 131, 16));
        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(20, 110, 131, 16));
        editPixelsize = new QLineEdit(groupBox);
        editPixelsize->setObjectName(QStringLiteral("editPixelsize"));
        editPixelsize->setGeometry(QRect(210, 110, 91, 20));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(340, 10, 271, 51));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 20, 121, 16));
        editDatumHeight = new QLineEdit(groupBox_2);
        editDatumHeight->setObjectName(QStringLiteral("editDatumHeight"));
        editDatumHeight->setGeometry(QRect(130, 20, 131, 20));
        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 160, 311, 181));
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 110, 171, 16));
        editGEDist = new QLineEdit(groupBox_3);
        editGEDist->setObjectName(QStringLiteral("editGEDist"));
        editGEDist->setGeometry(QRect(210, 110, 91, 20));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 140, 171, 16));
        editRedudantBaseline = new QLineEdit(groupBox_3);
        editRedudantBaseline->setObjectName(QStringLiteral("editRedudantBaseline"));
        editRedudantBaseline->setGeometry(QRect(210, 140, 91, 20));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 20, 171, 16));
        editFlightHeight = new QLineEdit(groupBox_3);
        editFlightHeight->setObjectName(QStringLiteral("editFlightHeight"));
        editFlightHeight->setGeometry(QRect(210, 20, 91, 20));
        label_11 = new QLabel(groupBox_3);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(20, 50, 131, 16));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(20, 80, 171, 16));
        editoverlaprate = new QLineEdit(groupBox_3);
        editoverlaprate->setObjectName(QStringLiteral("editoverlaprate"));
        editoverlaprate->setGeometry(QRect(210, 50, 91, 20));
        editOverlapSide = new QLineEdit(groupBox_3);
        editOverlapSide->setObjectName(QStringLiteral("editOverlapSide"));
        editOverlapSide->setGeometry(QRect(210, 80, 91, 20));
        cmdDesignStart = new QCommandLinkButton(centralWidget);
        cmdDesignStart->setObjectName(QStringLiteral("cmdDesignStart"));
        cmdDesignStart->setGeometry(QRect(390, 300, 172, 41));
        cmdDesignStart->setAutoFillBackground(true);
        textOutputFile = new QTextEdit(centralWidget);
        textOutputFile->setObjectName(QStringLiteral("textOutputFile"));
        textOutputFile->setGeometry(QRect(360, 270, 241, 21));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(360, 250, 171, 16));
        toolButtonOutputSelect = new QToolButton(centralWidget);
        toolButtonOutputSelect->setObjectName(QStringLiteral("toolButtonOutputSelect"));
        toolButtonOutputSelect->setGeometry(QRect(470, 250, 37, 18));
        groupBox_4 = new QGroupBox(centralWidget);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(340, 140, 271, 101));
        textRegionFile = new QTextEdit(groupBox_4);
        textRegionFile->setObjectName(QStringLiteral("textRegionFile"));
        textRegionFile->setGeometry(QRect(20, 30, 181, 61));
        toolButton_Region = new QToolButton(groupBox_4);
        toolButton_Region->setObjectName(QStringLiteral("toolButton_Region"));
        toolButton_Region->setGeometry(QRect(210, 40, 51, 20));
        toolButton_Region_2 = new QToolButton(groupBox_4);
        toolButton_Region_2->setObjectName(QStringLiteral("toolButton_Region_2"));
        toolButton_Region_2->setGeometry(QRect(210, 70, 51, 20));
        btn_unittest = new QPushButton(centralWidget);
        btn_unittest->setObjectName(QStringLiteral("btn_unittest"));
        btn_unittest->setGeometry(QRect(10, 350, 71, 21));
        groupBox_5 = new QGroupBox(centralWidget);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(340, 70, 271, 61));
        label_8 = new QLabel(groupBox_5);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 20, 121, 16));
        toolButtonAirport = new QToolButton(groupBox_5);
        toolButtonAirport->setObjectName(QStringLiteral("toolButtonAirport"));
        toolButtonAirport->setGeometry(QRect(130, 20, 37, 18));
        labelAirportLocation = new QLabel(groupBox_5);
        labelAirportLocation->setObjectName(QStringLiteral("labelAirportLocation"));
        labelAirportLocation->setGeometry(QRect(20, 40, 241, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 621, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menu->addAction(action_4);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "UAV Route Designer", 0));
        action_4->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216\346\210\221\344\273\254", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "\347\233\270\346\234\272", 0));
        label->setText(QApplication::translate("MainWindow", "\347\204\246\350\267\235 (mm)", 0));
        label_2->setText(QApplication::translate("MainWindow", "\347\233\270\347\211\207\345\256\275 (Pixels)", 0));
        label_3->setText(QApplication::translate("MainWindow", "\347\233\270\347\211\207\351\253\230 (Pixels)", 0));
        label_13->setText(QApplication::translate("MainWindow", "\350\261\241\345\205\203\345\244\247\345\260\217 (um)", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "\345\234\260\345\275\242\345\256\232\344\271\211", 0));
        label_6->setText(QApplication::translate("MainWindow", "\345\237\272\345\207\206\351\235\242\351\253\230\345\272\246 (m)", 0));
        editDatumHeight->setText(QString());
        groupBox_3->setTitle(QApplication::translate("MainWindow", "\350\207\252\345\256\232\344\271\211", 0));
        label_4->setText(QApplication::translate("MainWindow", "\345\274\225\345\257\274\347\202\271\350\277\233\345\205\245\347\202\271\350\267\235\347\246\273 (m)", 0));
        editGEDist->setText(QString());
        label_5->setText(QApplication::translate("MainWindow", "\345\206\227\344\275\231\345\237\272\347\272\277\346\225\260", 0));
        editRedudantBaseline->setText(QString());
        label_7->setText(QApplication::translate("MainWindow", "\350\256\276\350\256\241\350\210\252\351\253\230  (m)", 0));
        editFlightHeight->setText(QString());
        label_11->setText(QApplication::translate("MainWindow", "\350\210\252\345\220\221\351\207\215\345\217\240\347\216\207 (100%)", 0));
        label_12->setText(QApplication::translate("MainWindow", "\346\227\201\345\220\221\351\207\215\345\217\240\347\216\207 (100%)", 0));
        editoverlaprate->setText(QString());
        editOverlapSide->setText(QString());
        cmdDesignStart->setText(QApplication::translate("MainWindow", "\350\256\276\350\256\241\350\210\252\347\272\277", 0));
#ifndef QT_NO_WHATSTHIS
        textOutputFile->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_10->setText(QApplication::translate("MainWindow", "\350\276\223\345\207\272\346\226\207\344\273\266 ...", 0));
        toolButtonOutputSelect->setText(QApplication::translate("MainWindow", "...", 0));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "\346\221\204\345\214\272\345\256\232\344\271\211", 0));
        toolButton_Region->setText(QApplication::translate("MainWindow", "Choose", 0));
        toolButton_Region_2->setText(QApplication::translate("MainWindow", "Manage", 0));
        btn_unittest->setText(QApplication::translate("MainWindow", "unittest", 0));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "\346\234\272\345\234\272\345\256\232\344\271\211", 0));
        label_8->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251kml\346\226\207\344\273\266...", 0));
        toolButtonAirport->setText(QApplication::translate("MainWindow", "...", 0));
        labelAirportLocation->setText(QApplication::translate("MainWindow", "...", 0));
        menu->setTitle(QApplication::translate("MainWindow", "\345\270\256\345\212\251", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
