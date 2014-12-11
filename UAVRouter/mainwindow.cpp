#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"

#include "cogrgeometryfilereader.h"
#include "flightroutedesign.h"
#include "designtaskfactory.h"

#include <qfiledialog.h>
#include "gomologging.h"

#include <QDebug>

#include <QFile>

#include <QException>

#include <QXmlStreamReader>

#include "uicontroller.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ui->btn_unittest->hide();



    //load the default Camera parameters
//    QString focus("55");
//    QString h("7760");
//    QString w("10328");
//    QString pixel("5.2");
//    ui->editFocus->setText(focus);
//    ui->editCamHeight->setText(h);
//    ui->editCamWidth->setText(w);
//    ui->editPixelsize->setText(pixel);

    UIController * myUiController = UIController::GetUIControllerPtr();
    myUiController->AddMainWindow(ui);
    myUiController->LoadDefaultDesignParameters2UI();

    m_pCopyRightDialog=NULL;
    ptrChild_TV = NULL;
    listInputKmlFile.clear();
}

MainWindow::~MainWindow()
{
    delete ptrChild_TV;
    delete ui;
}


void MainWindow::on_toolButtonAirport_clicked()
{


        QString s = QFileDialog::getOpenFileName(this,"Choose a Airport Location File...",
                            ".",
                            "KML (*.kml *.KML)"
                            );

        if(s.length() == 0)
        {
            QMessageBox::information(NULL, tr("Path error!"), tr("You didn't select any files."));
          }
        else
        {
            std::auto_ptr<OGRGeometry> airport_geom_ptr =COGRGeometryFileReader::GetFirstOGRPointFromFile(s.toStdString());

            OGRPoint * pPt=dynamic_cast<OGRPoint *>(airport_geom_ptr.get());

            OGRPoint airportLoc(pPt->getX(), pPt->getY());

            //m_flight_param.AirportLocation=airportLoc;
            m_flight_param.airport.SetLocation(airportLoc);

            QFileInfo fi(s);
            QString airportfilebasename=fi.baseName();
            m_flight_param.airport.SetName(airportfilebasename.toStdString());

//            char *location=NULL;
//            airportLoc.exportToWkt(&location);
//            QString strloc(location);
//            ui->labelAirportLocation->setText(strloc);

            QString sLat= QString::number(airportLoc.getY(),'f',6);
            ui->editAirportLatitude->setText(sLat);
            QString sLon= QString::number(airportLoc.getX(),'f',6);
            ui->editAirportLongitude->setText(sLon);
            QString sAlt= QString::number(airportLoc.getZ(),'f',6);
            ui->editAirportAltitude->setText(sAlt);


            //QMessageBox::information(NULL, tr("Path"), tr("You selected ") + s);
        }






}

void MainWindow::on_toolButton_Region_clicked()
{

    /*m_flight_param.ClearFlightRegions();

    if(ui->radioSinglePolygon->isChecked())
    {
        QString s = QFileDialog::getOpenFileName(this,"Choose a Flight Region Defination File...",
                            ".",
                            "KML (*.kml *.KML)"
                            );

        m_flight_param.FightRegion=COGRGeometryFileReader::GetFirstOGRGeometryFromFile(s.toStdString());

        if(s.length() == 0)
        {
            QMessageBox::information(NULL, tr("Path Error"), tr("You didn't select any files."));
        }
        else
        {
            GomoLogging * pLog = GomoLogging::GetInstancePtr();
            std::string logstr("hello!");
            pLog->logging(logstr);
        }

        ui->textRegionFile->setText(s);

    }
    else
    {

        QStringList slist = QFileDialog::getOpenFileNames(this,"Choose Flight Region Files(Use Ctrl+)...",
                            ".",
                            "KML (*.kml *.KML)"
                            );

        if(slist.size()>0)
        {
            slist.sort(Qt::CaseInsensitive);//important to make the file list in dictionary order

            GomoLogging * pLog = GomoLogging::GetInstancePtr();

            for(int i=0; i< slist.size(); i++)
            {
                std::string regionfilepath = slist.at(i).toStdString();

                pLog->logging(regionfilepath);

                m_flight_param.AddFlightRegionGeometry(
                            COGRGeometryFileReader::GetFirstOGRGeometryFromFile(regionfilepath));
            }

        }
        else
        {
            QMessageBox::information(NULL, tr("Path Error"), tr("You didn't select any files."));
        }



    }*/

    if(0 == listInputKmlFile.size())
    {
        QFileDialog dlg(this,
                        "Choose Flight Region Defination File(s)...",
                        ".",
                        "KML (*.kml *.KML)");
        dlg.setFileMode(QFileDialog::ExistingFiles);
        if(dlg.exec() == QFileDialog::Accepted)
        {
            QStringList slist = dlg.selectedFiles();
            listInputKmlFile.clear();
            for(int i=0; i<slist.size(); ++i)
            {
                listInputKmlFile.push_back(slist.at(i));
            }
        }
    }

    // show the management dialog
    if(NULL != ptrChild_TV)
    {
        delete ptrChild_TV;
    }

    ptrChild_TV = new child_tv(this);

    ptrChild_TV->move(this->x() + this->frameSize().width(), this->y());
    ptrChild_TV->show();
}

void MainWindow::on_cmdDesignStart_clicked()
{
    if(!fillInFlightParamRegionFiles())
    {
        QMessageBox::information(this,
                                 tr("Path Error"),
                                 tr("You didn't select any files."));
        return;
    }

    DigitalCameraInfo cam;

    cam.f=ui->editFocus->text().toDouble();
    cam.width = ui->editCamWidth->text().toLong();
    cam.height = ui->editCamHeight->text().toLong();
    cam.pixelsize = ui->editPixelsize->text().toDouble()/1000.0; //um => mm


    m_flight_param.CameraInfo=cam;

    m_flight_param.AverageElevation= ui->editDatumHeight->text().toDouble();
    m_flight_param.FightHeight=ui->editFlightHeight->text().toDouble();
    m_flight_param.GuidanceEntrancePointsDistance=ui->editGEDist->text().toDouble();
    m_flight_param.overlap=ui->editoverlaprate->text().toDouble()/100.0;
    m_flight_param.overlap_crossStrip=ui->editOverlapSide->text().toDouble()/100.0;
    m_flight_param.RedudantBaselines=ui->editRedudantBaseline->text().toInt();

    //Airport
    double x_airport,y_airport,z_airport;
    x_airport = ui->editAirportLongitude->text().toDouble();
    y_airport = ui->editAirportLatitude->text().toDouble();
    z_airport = ui->editAirportAltitude->text().toDouble();
    OGRPoint airportLoc(x_airport,y_airport,z_airport);
    m_flight_param.airport.SetLocation(airportLoc);


    //m_flight_param.OutputFilePathname=ui->textOutputFile->toPlainText().toStdString();

    FlightRouteDesign * route_desinger = DesignTaskFactory::CreateFlightRouteDeigner(m_flight_param);
    QFileInfo fi(ui->textOutputFile->toPlainText());
    QString outputbasename=fi.absolutePath()+"/"+fi.baseName();
    QString outputBinary = outputbasename +".bht";
    QString outputText = outputbasename +".ght";
    QString outputKML = outputbasename +".kml";
    QString outputGST = outputbasename +".gst";

    qDebug("route_desinger->AddOutPutFileName(outputBinary.toStdString());()");


    //route_desinger->AddOutPutFileName(outputBinary.toStdString());
    route_desinger->AddOutPutFileName(outputText.toStdString());
    route_desinger->AddOutPutFileName(outputKML.toStdString());
    //route_desinger->AddOutPutFileName(outputGST.toStdString());
    qDebug("route_desinger->PerformRouteDesign()");
    route_desinger->PerformRouteDesign();
    route_desinger->OutputRouteFile();

    delete route_desinger;
    route_desinger=NULL;

}

void MainWindow::on_toolButtonOutputSelect_clicked()
{
    QString s = QFileDialog::getSaveFileName(this,"Choose a Flight Route Design File...",
                        ".",
                        "ght, kml(*.ght(text file) *.kml(KML file))"
                        );

    ui->textOutputFile->setText(s);


}


void MainWindow::on_btn_unittest_clicked()
{
    DigitalCameraInfo cam;
    cam.f= 55; //mm
    cam.width = 10328;
    cam.height = 7760;
    cam.pixelsize = 0.005; //mm

    m_flight_param.CameraInfo=cam;
    m_flight_param.AverageElevation= 0;
    m_flight_param.FightHeight=300; //flight height 300m in default
    m_flight_param.GuidanceEntrancePointsDistance=200;// 200m
    m_flight_param.overlap=0.6;
    m_flight_param.overlap_crossStrip=0.2;
    m_flight_param.RedudantBaselines=2;
    std::string region_filename("G:/Gomo/Data/0615/0615_testregion2.kml");
    m_flight_param.FightRegion=COGRGeometryFileReader::GetFirstOGRGeometryFromFile(region_filename);

    std::string airport_filename("G:/Gomo/Data/0615/airport_0615.kml");
    std::auto_ptr<OGRGeometry> airport_geom_ptr =COGRGeometryFileReader::GetFirstOGRPointFromFile(airport_filename);
    OGRPoint * pPt=dynamic_cast<OGRPoint *>(airport_geom_ptr.get());
    OGRPoint airportLoc(pPt->getX(), pPt->getY());
    m_flight_param.airport.SetLocation(airportLoc);
    QString fap(airport_filename.c_str());
    QFileInfo fiairport(fap);
    m_flight_param.airport.SetName(fiairport.baseName().toStdString());

    FlightRouteDesign * route_desinger = DesignTaskFactory::CreateFlightRouteDeigner(m_flight_param);
    QString output("G:/Gomo/Data/0615/output/testmoshikou_single_45deg.txt");
    QFileInfo fi(output);
    QString outputbasename=fi.absolutePath()+"/"+fi.baseName();
    QString outputBinary = outputbasename +".bht";
    QString outputText = outputbasename +".ght";
    QString outputKML = outputbasename +".kml";
    QString outputGST = outputbasename +".gst";

    route_desinger->AddOutPutFileName(outputBinary.toStdString());
    route_desinger->AddOutPutFileName(outputText.toStdString());
    route_desinger->AddOutPutFileName(outputKML.toStdString());
    route_desinger->AddOutPutFileName(outputGST.toStdString());
    route_desinger->PerformRouteDesign();
    route_desinger->OutputRouteFile();

    delete route_desinger;
    route_desinger=NULL;




}

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{

}

void MainWindow::on_radioSinglePolygon_clicked()
{

}

void MainWindow::on_radioMultiPolygon_clicked()
{

}

void MainWindow::on_radioMultiPolygon_toggled(bool checked)
{


}

void MainWindow::on_radioSinglePolygon_toggled(bool checked)
{

}

void MainWindow::on_menuAboutUs_triggered()
{
    m_pCopyRightDialog = new CopyRightDialog(this);

    m_pCopyRightDialog->show();

}


bool MainWindow::fillInFlightParamRegionFiles()
{
    if(0 >= listInputKmlFile.size())
        return false;

    QStringList slist;
    for(std::list<QString>::iterator iter = listInputKmlFile.begin();
        iter != listInputKmlFile.end(); ++iter)
        slist.append(*iter);

    slist.sort(Qt::CaseInsensitive);

    GomoLogging * pLog = GomoLogging::GetInstancePtr();

    if(1 == slist.size())
    {
        m_flight_param.FightRegion =
                COGRGeometryFileReader::GetFirstOGRGeometryFromFile(slist.at(0).toStdString());

        std::string logstr("hello!");
        pLog->logging(logstr);
    }
    else
    {
        m_flight_param.ClearFlightRegions();
        for(int i=0; i< slist.size(); i++)
        {
            std::string regionfilepath = slist.at(i).toStdString();

            pLog->logging(regionfilepath);

            m_flight_param.AddFlightRegionGeometry(
                        COGRGeometryFileReader::GetFirstOGRGeometryFromFile(regionfilepath));
        }
    }

    return true;
}
