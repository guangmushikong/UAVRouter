#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "ui_mainwindow.h"


namespace Ui {
class MainWindow;
}

class UIController
{
protected:
    UIController();

    ~UIController();


public:
    static   UIController* GetUIControllerPtr();

    void AddMainWindow(Ui::MainWindow * main_win);

public:
    void LoadDefaultDesignParameters2UI();


protected:
    Ui::MainWindow * m_ui_main_window;


};

#endif // UICONTROLLER_H
