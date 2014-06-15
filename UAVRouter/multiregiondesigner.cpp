#include "multiregiondesigner.h"
#include "designtaskfactory.h"

    MultiRegionDesigner::MultiRegionDesigner()
    {
    }

    MultiRegionDesigner::MultiRegionDesigner(const FlightParameter & parameter)
        :FlightRouteDesign(parameter)
    {

    }

//    void MultiRegionDesigner:: SetFightParameters(FlightParameter & parameter )
//    {

//    }

    void MultiRegionDesigner::PerformRouteDesign()
    {
        qDebug("MultiRegionDesigner::PerformRouteDesign()");

        FlightParameter param_single;// single region in multi-regions collection
        param_single = m_parameter;
        param_single.ClearFlightRegions(); // make sure the DesignTaskFactory would not create MultiRegionDesigner

        UAVFlightPoint pre_last_point;

        if ( m_parameter.GetRegionCount()>=1)
        {
            for(int i=0; i<m_parameter.GetRegionCount(); i++ )
            {

                // make param_single as the parameter for the i-th region
                param_single.FightRegion = m_parameter.GetFlightRegionGeometry(i);

                // the regions except the first one do not have there real 'Airport',
                // so here a pseudo airport is constructed from the last flight point of the previous flight region
                if( i>0)
                {
                    Gomo::FlightRoute::Airport airport_pseudo;
                    airport_pseudo.SetLocation(pre_last_point.ToOGRPoint());
                    param_single.airport =  airport_pseudo;
                }

                FlightRouteDesign * single_reg_desinger = DesignTaskFactory::CreateFlightRouteDeigner(param_single);
                single_reg_desinger->PerformRouteDesign();

                pre_last_point = single_reg_desinger->GetLastFlightPoint();

                ///to do , copy design result from single_reg_desinger to current class, ie MultiRegionDesigner
                if( i==0)
                {
                    ShareDesign(*single_reg_desinger);
                }
                else
                {
                    ShareDesign(*single_reg_desinger,true);
                }


                delete single_reg_desinger;
                single_reg_desinger=NULL;
            }
        }

    }





    // space holder to make the function not virtual
    void MultiRegionDesigner::DesignInGaussPlane()
    {

    }

    // space holder to make the function not virtual
    void MultiRegionDesigner::CreateNewFilghtPoint(unsigned int strip_id,
                              unsigned int id_in_strip,
                              enumFlightPointType ptType,
                              double longitude,
                              double latitude )
    {

    }
