#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "flightparameter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:

    void on_toolButtonAirport_clicked();

    void on_toolButton_Region_clicked();

    void on_cmdDesignStart_clicked();

    void on_toolButtonOutputSelect_clicked();

    void on_btn_unittest_clicked();

    void on_MainWindow_iconSizeChanged(const QSize &iconSize);

    void on_radioSinglePolygon_clicked();

    void on_radioMultiPolygon_clicked();

    void on_radioMultiPolygon_toggled(bool checked);

    void on_radioSinglePolygon_toggled(bool checked);

private:
    Ui::MainWindow *ui;

protected:
    Gomo::FlightRoute::FlightParameter m_flight_param;
};

#endif // MAINWINDOW_H
