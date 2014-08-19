#include "copyrightdialog.h"
#include "ui_copyrightdialog.h"

CopyRightDialog::CopyRightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyRightDialog)
{
    ui->setupUi(this);


    QPalette pal=ui->mywidget->palette();
    //pal.setBrush(QPalette::Window,QPixmap("C:/Users/wangmiao/Documents/GitHub/UAVRouter/build-UAVRouter-Desktop_Qt_5_2_1_MSVC2010_32bit-Release/release/pics/logo.png"));
    pal.setBrush(QPalette::Window,QPixmap("./pics/logo.png"));
    ui->mywidget->setPalette(pal);
    ui->mywidget->setAutoFillBackground(true);
    //ui->mywidget->show();


}

CopyRightDialog::~CopyRightDialog()
{
    delete ui;
}
