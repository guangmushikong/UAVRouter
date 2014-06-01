#include "linearflightroutedesign.h"
#include <fstream>
using std::ofstream;

#include <QDebug>

#include "geomertyconvertor.h"
using namespace Gomo::Geometry2D;


LinearFlightRouteDesign::LinearFlightRouteDesign()
{
}

LinearFlightRouteDesign::LinearFlightRouteDesign(const  FlightParameter & parameter):
    FlightRouteDesign(parameter)
{
}

void LinearFlightRouteDesign::PerformRouteDesign()
{
    qDebug("PolygonAreaFlightRouteDesign::PerformRouteDesign()");

    FlightRouteDesign::PerformRouteDesign();

}


double LinearFlightRouteDesign::CreateLinearStrip(int strip_id,
                    const Point2D & begin,
                    const Point2D & end)
{
    double length = 0;

    return length;

}

void LinearFlightRouteDesign::DesignInGaussPlane()
{
    Point2DArray region_Points_GaussCoords;

    GeomertyConvertor::OGRGeomery2Point2DArray(m_FightRegion_Gauss.get(),region_Points_GaussCoords);

    Point2D prevPt = region_Points_GaussCoords[0];
    Point2DArray::iterator it=region_Points_GaussCoords.begin()+1;
    int strip_id =1;
    for( ; it!=region_Points_GaussCoords.end(); it++)
    {
        Point2D curPt = *it;

        CreateLinearStrip(strip_id,prevPt,curPt);
        strip_id++;
        prevPt = curPt;

    }

    //CalculatePolygonOrientaion(region_Points_GaussCoords,m_region_center_GuassProj,m_angle_region_GuassProj);

//    ostringstream streamdebug;
//    streamdebug.str("");
//    streamdebug<< "LinearFlightRouteDesign::DesignInGaussPlane(): CalculatePolygonOrientaion"<<std::endl;
//    streamdebug<<"region_center="<<m_region_center_GuassProj.X<<","<<m_region_center_GuassProj.Y<<"  "<<"angle="<<m_angle_region_GuassProj<<std::endl;
//    qDebug(streamdebug.str().c_str());

    Point2D pt_AirportLoc_Gauss;
    GeomertyConvertor::OGRPoint2Point2D(m_AirportLoc_Gauss,pt_AirportLoc_Gauss);

}


void LinearFlightRouteDesign::CreateNewFilghtPoint(unsigned int strip_id,
                                                   unsigned int id_in_strip,
                                                   enumFlightPointType ptType,
                                                   double longitude,
                                                   double latitude )
{
    UAVFlightPoint pt;
    pt.__strip_id = strip_id;
    pt.__id_in_strip = id_in_strip;
    pt.__flight_point_type = ptType;
    pt.__longitude= longitude;
    pt.__latitude = latitude;
    m_route_design_CaussProj.__flight_point.push_back(pt);

}

void LinearFlightRouteDesign::OutputRouteFile( )
{
    qDebug("PolygonAreaFlightRouteDesign::OutputRouteFile()");

    FlightRouteDesign::OutputRouteFile();


}
