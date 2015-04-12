#include "uicontroller.h"

UIController::UIController()
{

}

UIController::~UIController()
{


}



void UIController::LoadDefaultDesignParameters2UI()
{
    //camera
    QString focus("55");
    QString h("10328");
    QString w("7760");
    QString pixel("5.2");

    m_ui_main_window->editFocus->setText(focus);
    m_ui_main_window->editCamHeight->setText(h);
    m_ui_main_window->editCamWidth->setText(w);
    m_ui_main_window->editPixelsize->setText(pixel);

    /*
    //set the camera fixed for specific customer
    m_ui_main_window->editFocus->setReadOnly(true);
    m_ui_main_window->editCamHeight->setReadOnly(true);
    m_ui_main_window->editCamWidth->setReadOnly(true);
    m_ui_main_window->editPixelsize->setReadOnly(true);
*/


    //flight info
    QString datumHeight("0.0");
    QString flightHeight("1000.0");
    QString guidanceEntrancePointsDistance("100.0");
    QString overlap("70");
    QString cross_overlap("30");
    QString redudantBaselines("0");

    m_ui_main_window->editDatumHeight->setText(datumHeight);
    m_ui_main_window->editFlightHeight->setText(flightHeight);
    m_ui_main_window->editGEDist->setText(guidanceEntrancePointsDistance);
    m_ui_main_window->editoverlaprate->setText(overlap);
    m_ui_main_window->editOverlapSide->setText(cross_overlap);
    m_ui_main_window->editRedudantBaseline->setText(redudantBaselines);




}


void UIController::AddMainWindow(Ui::MainWindow * main_win)
{
    if (main_win!=NULL)
    {
        m_ui_main_window = main_win;
    }
}


UIController* UIController::GetUIControllerPtr()
{

    static UIController controllerInstance;

    return &controllerInstance;

}



