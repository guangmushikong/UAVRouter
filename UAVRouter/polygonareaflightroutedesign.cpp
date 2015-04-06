#include "polygonareaflightroutedesign.h"
#include<vector>
using std::vector;

#include <fstream>
using std::ofstream;
#include <sstream>
using std::ostringstream;

#include <QDebug>

#include "geomertyconvertor.h"
using namespace Gomo::Geometry2D;

#include "gomologging.h"

PolygonAreaFlightRouteDesign::PolygonAreaFlightRouteDesign()
{
}

PolygonAreaFlightRouteDesign::PolygonAreaFlightRouteDesign(const FlightParameter & parameter)
    :FlightRouteDesign(parameter)
{

}

void PolygonAreaFlightRouteDesign::DesignInGaussPlane()
{

    Point2DArray region_Points_GaussCoords;

    GeomertyConvertor::OGRGeomery2Point2DArray(m_FightRegion_Gauss.get(),region_Points_GaussCoords);

    CalculatePolygonOrientaion(region_Points_GaussCoords,m_region_center_GuassProj,m_angle_region_GuassProj);



    ostringstream streamdebug;
    streamdebug.str("");
    streamdebug<< "PolygonAreaFlightRouteDesign::DesignInGaussPlane(): CalculatePolygonOrientaion"<<std::endl;
    streamdebug<<"region_center="<<m_region_center_GuassProj.X<<","<<m_region_center_GuassProj.Y<<"  "<<"angle="<<m_angle_region_GuassProj<<std::endl;
    qDebug(streamdebug.str().c_str());

    Point2D pt_AirportLoc_Gauss;
    GeomertyConvertor::OGRPoint2Point2D(m_AirportLoc_Gauss,pt_AirportLoc_Gauss);

    PlaneTransform(region_Points_GaussCoords,pt_AirportLoc_Gauss);

    DesignInTransformedCoords();

    InversePlaneTransform();
}

void PolygonAreaFlightRouteDesign::PerformRouteDesign()
{

    qDebug("PolygonAreaFlightRouteDesign::PerformRouteDesign()");

    FlightRouteDesign::PerformRouteDesign();

}



bool PolygonAreaFlightRouteDesign::CalculatePolygonOrientaion(const Point2DArray&  polygon_2d,Point2D& center,double& angle)
{

    PolygonOrientation2D Polygon_orien(polygon_2d);

    center = Polygon_orien.GetCenter();

    Polygon_orien.SetExtendBaseLineLength(m_baseline_length*(m_parameter.RedudantBaselines+1)*2);

    return Polygon_orien.GetOptimalOrientation(angle);


    //return Gomo::Geometry2D::CalculatePolygonOrientaion2D(polygon_2d,center,angle);
}




// 1.rotate the gauss projection axis to the m_angle_region, which is the orientation of the flight polygon
// 2.move the gauss projection origin to m_region_center
void PolygonAreaFlightRouteDesign::PlaneTransform(
        const Point2DArray & polygonPoints_gauss,
        const Point2D & ptAirport)
{
    qDebug("PolygonAreaFlightRouteDesign::PlaneTransform()");
    ostringstream streamdebug;
    streamdebug.str("");

    m_region_polygonPoints_planetransformed.clear();
    streamdebug<<"region after plane transform:"<<std::endl;

    Point2D sum_verify(0,0);

    Point2DArray::const_iterator it=polygonPoints_gauss.begin();
    for ( ; it!=polygonPoints_gauss.end(); it++)
    {
        // 1. first translate the origin point to m_region_center_GuassProj, ie centralized
        Point2D point_centralized = *it - m_region_center_GuassProj;
        // 2. rotate the points, in order to make m_angle_region_GuassProj as the x-axis of Guass plane coords sys
        Point2D point_rotated     = Rotate2D( point_centralized, -m_angle_region_GuassProj);
        m_region_polygonPoints_planetransformed.push_back(point_rotated);
        streamdebug<<"("<<point_rotated.X<<","<<point_rotated.Y<<")  ";

        sum_verify+=point_rotated;
    }

    streamdebug<<std::endl<<"sum of points roated:"<<sum_verify.X<<","<<sum_verify.Y<<")  ";//sum of points roated:1.06593e-009,-7.45786e-011)

    Point2D pt_airport_centralized = ptAirport - m_region_center_GuassProj;
    m_airport_planetransformed     = Rotate2D( pt_airport_centralized, -m_angle_region_GuassProj);
    streamdebug<<std::endl<<"airport after plane tranform: ("<<m_airport_planetransformed.X<<","<<m_airport_planetransformed.Y<<")  ";

    qDebug(streamdebug.str().c_str());

}

// recover the coods to gauss projection
void PolygonAreaFlightRouteDesign::InversePlaneTransform()
{
    GomoLogging * pLog = GomoLogging::GetInstancePtr();


    qDebug("PolygonAreaFlightRouteDesign::InversePlaneTransform()");
    ostringstream streamdebug;
    streamdebug.str("");

    //verify header, ie, airport
    Point2D pt_airport_centralized = Rotate2D( m_airport_planetransformed, m_angle_region_GuassProj);
    Point2D airport_guass_recovered= pt_airport_centralized + m_region_center_GuassProj;
    Point2D pt_AirportLoc_Gauss;
    GeomertyConvertor::OGRPoint2Point2D(m_AirportLoc_Gauss,pt_AirportLoc_Gauss);
    if (airport_guass_recovered == pt_AirportLoc_Gauss)
    {
        streamdebug<<"PASS: Airport InversePlaneTransform"<<std::endl;
    }
    else
    {
        streamdebug<<"FAIL!: Airport InversePlaneTransform"<<std::endl;
        streamdebug<<"airport_guass_recovered: ("<<airport_guass_recovered.X<<","
                  <<airport_guass_recovered.Y<<")"<<std::endl;
    }

    qDebug(streamdebug.str().c_str());

    //points
    unsigned int count_exposure = 0;
    std::vector< UAVFlightPoint >::iterator it= m_route_design_plane.__flight_point.begin();

    UAVFlightPoint prevPt;
    prevPt.__strip_id=0;
    double course_length_guass =0.0;

    for( ; it!= m_route_design_plane.__flight_point.end() ; it++ )
    {
        UAVFlightPoint flight_pt_guass;
        flight_pt_guass.__strip_id         = (*it).__strip_id;
        flight_pt_guass.__id_in_strip      = (*it).__id_in_strip;
        flight_pt_guass.__flight_point_type= (*it).__flight_point_type;

        Point2D pt((*it).__longitude,(*it).__latitude);
        Point2D pt_guass_recover = m_region_center_GuassProj + Rotate2D( pt, m_angle_region_GuassProj);

        flight_pt_guass.__longitude = pt_guass_recover.X;
        flight_pt_guass.__latitude  = pt_guass_recover.Y;

        m_route_design_CaussProj.__flight_point.push_back(flight_pt_guass);

        if (flight_pt_guass.__flight_point_type == FLIGTH_POINT_TYPE_EXPOSURE)
        {
            count_exposure++;
        }

        if( prevPt.__strip_id == flight_pt_guass.__strip_id  )
        {
            double dist_pts=pt_guass_recover.DistanceTo(prevPt.__longitude,prevPt.__latitude);
            course_length_guass += dist_pts;
            streamdebug.str("");
            streamdebug<<"prePoint:"
                      <<"longitude"<<prevPt.__longitude
                      <<"latitude" <<prevPt.__latitude << std::endl;
            streamdebug<<"current:"
                      <<"longitude"<<pt_guass_recover.X
                      <<"latitude" <<pt_guass_recover.Y<< std::endl;
            streamdebug<<"dist of points in guass: "<<dist_pts<<std::endl;
            std::string logstr = streamdebug.str();
            pLog->logging(logstr);
        }

        prevPt = flight_pt_guass;
    }

    //statistic
    streamdebug.str("");
    m_route_design_CaussProj.__flight_statistic.__count_exposures   = count_exposure;
    m_route_design_CaussProj.__flight_statistic.__count_strips      = m_route_design_plane.__flight_statistic.__count_strips ;
    m_route_design_CaussProj.__flight_statistic.__flight_region_area= dynamic_cast<OGRPolygon*>(m_FightRegion_Gauss.get())->get_Area();
    m_route_design_CaussProj.__flight_statistic.__photo_flight_course_chainage  = course_length_guass;
    m_route_design_CaussProj.__flight_statistic.__MBR_Area = m_route_design_plane.__flight_statistic.__MBR_Area;

//    OGREnvelope env;
//    dynamic_cast<OGRPolygon*>(m_FightRegion_Gauss.get())->getEnvelope(&env);
//    m_route_design_CaussProj.__flight_statistic.__MBR_Area = (env.MaxX -env.MinX) * (env.MaxY -env.MinY);

    streamdebug<< "__flight_region_area on gauss plane:"
               <<m_route_design_CaussProj.__flight_statistic.__flight_region_area<<"sqr meters"<<std::endl;
    streamdebug<< "MBR_Area on guass:"<<m_route_design_CaussProj.__flight_statistic.__MBR_Area<<"sqr meters"<<std::endl;

    streamdebug<< "course chainage in creating:"
               <<m_route_design_plane.__flight_statistic.__photo_flight_course_chainage<<" meters" <<std::endl;
    streamdebug<< "course chainage after InversePlaneTransform recover to Guass:"
               <<m_route_design_CaussProj.__flight_statistic.__photo_flight_course_chainage<<" meters" <<std::endl;

    qDebug(streamdebug.str().c_str());
}


void PolygonAreaFlightRouteDesign::DesignInTransformedCoords()
{
    qDebug("PolygonAreaFlightRouteDesign::DesignInTransformedCoords()");

    //-----------------------------------
    // 1.Header
    //-----------------------------------
    m_route_design_plane.__header.airport_height = m_parameter.airport.getZ();
    m_route_design_plane.__header.airport_longitude = m_airport_planetransformed.X;
    m_route_design_plane.__header.airport_latitude  = m_airport_planetransformed.Y;

    //-----------------------------------
    // 2.main: create flight points in strips
    //-----------------------------------
    // figure out the MBR of the region after plane transformed
    Point2D leftTop,rightBot;
    MBR2D(m_region_polygonPoints_planetransformed,leftTop,rightBot);
    ostringstream streamdebug;
    streamdebug.str("");
    streamdebug<<"mbr of the polygon after transform: ("<<leftTop.X<<","<<leftTop.Y<<") - ("
              <<rightBot.X<<","<<rightBot.Y<<")"<<std::endl;
    qDebug(streamdebug.str().c_str());


    //caculate the orientation of plane,
    // if the airport near the lefttop, then make the first strip start from lefttop,
    // otherwise the coordinate should be mirrored by x or y to make this is the case
    bool isAirportleft  =true;
    bool isAirportUp =true;
    Point2D orthoplane_center(0,0);
    AirportOrientationInOrthoPlane(m_airport_planetransformed,leftTop,rightBot,orthoplane_center,isAirportleft,isAirportUp);

    streamdebug.str("");

    // create strips from left top of MBR of flight region        
    double course_length = 0.0;    
    int strip_seq =1;

    float mbr_height =fabs(leftTop.Y - rightBot.Y);
    bool isSingleStrip = fabs(m_cross_strip_distance) >  mbr_height*1.2;

    if(isSingleStrip)
    {
        //create the single strip in the center of the y coordinate
        //Create the first strip
        float current_strip_y = (leftTop.Y+ rightBot.Y)/2.0;
        double strip_legth=CreateFirstStrip(strip_seq,current_strip_y,leftTop.X,rightBot.X);
        course_length += strip_legth;

        streamdebug<<"CreateSingleStrip: "<<std::endl;
        qDebug(streamdebug.str().c_str());
        streamdebug.str("");
    }
    else
    {
        //Create the first strip
        float current_strip_y = leftTop.Y;
        double strip_legth=CreateFirstStrip(strip_seq,current_strip_y,leftTop.X,rightBot.X);
        course_length += strip_legth;

        streamdebug<<"CreateFirstStrip: "<<std::endl;
        qDebug(streamdebug.str().c_str());
        streamdebug.str("");

        //create the other strips based on the first
        while (current_strip_y > (rightBot.Y + m_cross_strip_distance/2)) // modified 2015-04-06, to reduce the last reductant line
        {
            strip_seq++;
            current_strip_y -= m_cross_strip_distance;
            strip_legth=CreateNewStripBasedOnLastStrip(strip_seq,current_strip_y);
            course_length += strip_legth;
        }

    }


    // flip the flight points on the design plane according to the relative position of airport to lefttop
    //if isAirportleft==false, then flip the flight points on X axis
    //if isAirportUp == false, then flip the flight points on Y axis
    FlipOrthoPlaneOrientation(orthoplane_center,isAirportleft,isAirportUp);

    //-----------------------------------
    // 2.statistic
    //-----------------------------------
    m_route_design_plane.__flight_statistic.__count_strips  = strip_seq ;
    m_route_design_plane.__flight_statistic.__MBR_Area      = (rightBot.X - leftTop.X)*(leftTop.Y-rightBot.Y );
    m_route_design_plane.__flight_statistic.__photo_flight_course_chainage = course_length;
    streamdebug.str("");
    streamdebug<<"mbr area of the polygon after transform: "
              <<m_route_design_plane.__flight_statistic.__MBR_Area
             <<" sqr meters"<<std::endl;
    qDebug(streamdebug.str().c_str());
}

// create the first strip(paralle with x axis)
double PolygonAreaFlightRouteDesign::CreateFirstStrip(
        int strip_id,
        double current_strip_y,
        double first_valid_exposure_x,
        double last_valid_exposure_x)
{
    m_current_strip.Clear();

    double flight_course_length =0.0;

    if ( first_valid_exposure_x < last_valid_exposure_x)
    {
        unsigned int id=1;

        double x_entrance;
        x_entrance = first_valid_exposure_x - m_baseline_length*(m_parameter.RedudantBaselines+1) ;

qDebug("//A1:Guidance start\n");
        //A1:Guidance start
        double x_guidance = x_entrance -m_parameter.GuidanceEntrancePointsDistance;
        CreateNewFilghtPoint(strip_id,id,(enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_A_POINT_MASK),x_guidance,current_strip_y);
        id++;

qDebug("//A2:entrance\n");
        //A2:entrance
        CreateNewFilghtPoint(strip_id,id,(enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_A_POINT_MASK) ,x_entrance,current_strip_y);

qDebug("//first exposure points\n");
        id=1;
        double x_redudant;
        //exposure points for reductant baselines:
        for (int i=0; i<m_parameter.RedudantBaselines ; i++)
        {
            x_redudant = first_valid_exposure_x - m_baseline_length*(m_parameter.RedudantBaselines - i);
            CreateNewFilghtPoint(strip_id,id,FLIGTH_POINT_TYPE_EXPOSURE ,x_redudant,current_strip_y);
            id++;
        }

qDebug("//exposure points\n");
        // exposure points
        double current_x = first_valid_exposure_x;
        while(current_x < last_valid_exposure_x + m_baseline_length )
        {
            CreateNewFilghtPoint(strip_id,id,FLIGTH_POINT_TYPE_EXPOSURE,current_x,current_strip_y);
            current_x += m_baseline_length;
            id++;
        }

        double firs_redudant = current_x;

        //exposure points for reductant baselines:
        for (int i=0; i<m_parameter.RedudantBaselines ; i++)
        {
            x_redudant = firs_redudant + m_baseline_length*i;
            CreateNewFilghtPoint(strip_id,id,FLIGTH_POINT_TYPE_EXPOSURE ,x_redudant,current_strip_y);
            id++;
        }
qDebug("//B2:exit\n");
        //B2:exit
        double x_exit=firs_redudant;
        if (m_parameter.RedudantBaselines!=0)
        {
            x_exit = firs_redudant+m_baseline_length;
        }

        CreateNewFilghtPoint(strip_id,0,(enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_B_POINT_MASK),x_exit,current_strip_y);


        //B1:Guidance end
        double x_guidance_end = x_exit + m_parameter.GuidanceEntrancePointsDistance;
        CreateNewFilghtPoint(strip_id,0,(enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_B_POINT_MASK),x_guidance_end,current_strip_y);

        flight_course_length= x_guidance_end - x_guidance ;
    }
    else
    {
        throw "first_valid_exposure_x > last_valid_exposure_x !";
    }

    return flight_course_length;
}

double PolygonAreaFlightRouteDesign::CreateNewStripBasedOnLastStrip(
        int strip_id,
        double current_strip_y)
{
    m_last_strip=m_current_strip;
    m_current_strip.Clear();

    double x_guidance=0;
    double x_guidance_end=0;

    if(m_last_strip.__flight_point.size()>0)
    {
//        ostringstream streamdebug;
//        streamdebug.str("");
//        streamdebug<<"m_last_strip.__flight_point.size() ("<<m_last_strip.__flight_point.size()
//                  <<"m_last_strip.__count_exposures="<<m_last_strip.__count_exposures<<std::endl;
//        qDebug(streamdebug.str().c_str());


        std::vector<UAVFlightPoint>::reverse_iterator itr=m_last_strip.__flight_point.rbegin();

        for (; itr!=m_last_strip.__flight_point.rend();itr++)
        {
            UAVFlightPoint pt= *itr;
//            streamdebug.str("");
//            streamdebug<<"pt.__id="<<pt.__id_in_strip<<std::endl;
//            qDebug(streamdebug.str().c_str());

            switch (pt.__flight_point_type) {
            // exposure points
            case FLIGTH_POINT_TYPE_EXPOSURE:
                CreateNewFilghtPoint(strip_id,
                                     m_last_strip.__count_exposures-pt.__id_in_strip+1,
                                     FLIGTH_POINT_TYPE_EXPOSURE ,
                                     pt.__longitude,
                                     current_strip_y);
                break;
            //B1, transformed to A1
            case (enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_B_POINT_MASK):
                x_guidance=pt.__longitude;
                CreateNewFilghtPoint(strip_id,
                                     0,
                                     (enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_A_POINT_MASK) ,
                                     pt.__longitude,
                                     current_strip_y);
                break;
            //B2, transformed to A2
            case (enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_B_POINT_MASK):
                CreateNewFilghtPoint(strip_id,
                                     0,
                                     (enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_A_POINT_MASK) ,
                                     pt.__longitude,
                                     current_strip_y);
                break;
            //A1, transformed to B1
            case (enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_A_POINT_MASK):
                x_guidance_end=pt.__longitude;
                CreateNewFilghtPoint(strip_id,
                                     0,
                                     (enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_B_POINT_MASK) ,
                                     pt.__longitude,
                                     current_strip_y);
                break;
            //A2, transformed to B2
            case (enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_A_POINT_MASK ):
                CreateNewFilghtPoint(strip_id,
                                     0,
                                     (enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_B_POINT_MASK ) ,
                                     pt.__longitude,
                                     current_strip_y);
                break;
            default:
                break;
            }

        }

    }

    double flight_course_length=fabs(x_guidance_end - x_guidance) ;
    return flight_course_length;

}

double PolygonAreaFlightRouteDesign::CreateNewStrip(
        int strip_id,
        double current_strip_y,
        double first_valid_exposure_x,
        double last_valid_exposure_x)
{
    double flight_course_length =0.0;

    if ( first_valid_exposure_x < last_valid_exposure_x)
    {
        unsigned int id=1;

        double x_entrance;
        x_entrance = first_valid_exposure_x - m_baseline_length*(m_parameter.RedudantBaselines+1) ;


        //A1:Guidance start
        double x_guidance = x_entrance -m_parameter.GuidanceEntrancePointsDistance;
        CreateNewFilghtPoint(strip_id,id,(enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_A_POINT_MASK),x_guidance,current_strip_y);
        id++;


        //A2:entrance
        CreateNewFilghtPoint(strip_id,id,(enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_A_POINT_MASK) ,x_entrance,current_strip_y);

        id=1;
        double x_redudant;
        //exposure points for reductant baselines:
        for (int i=0; i<m_parameter.RedudantBaselines ; i++)
        {
            x_redudant = first_valid_exposure_x - m_baseline_length*(m_parameter.RedudantBaselines - i);
            CreateNewFilghtPoint(strip_id,id,FLIGTH_POINT_TYPE_EXPOSURE ,x_redudant,current_strip_y);
            id++;
        }


        // exposure points
        double current_x = first_valid_exposure_x;
        while(current_x < last_valid_exposure_x + m_baseline_length )
        {
            CreateNewFilghtPoint(strip_id,id,FLIGTH_POINT_TYPE_EXPOSURE,current_x,current_strip_y);
            current_x += m_baseline_length;
            id++;
        }

        //exposure points for reductant baselines:
        for (int i=0; i<m_parameter.RedudantBaselines ; i++)
        {
            x_redudant = current_x + m_baseline_length*i;
            CreateNewFilghtPoint(strip_id,id,FLIGTH_POINT_TYPE_EXPOSURE ,x_redudant,current_strip_y);
            id++;
        }

        //B2:exit
        double x_exit=x_redudant+m_baseline_length;
        CreateNewFilghtPoint(strip_id,id,(enumFlightPointType)(FLIGTH_POINT_TYPE_ETRANCE_EXIT | FLIGTH_POINT_TYPE_B_POINT_MASK),x_exit,current_strip_y);


        //B1:Guidance end
        double x_guidance_end = x_exit + m_parameter.GuidanceEntrancePointsDistance;
        CreateNewFilghtPoint(strip_id,id,(enumFlightPointType)(FLIGTH_POINT_TYPE_GUIDE | FLIGTH_POINT_TYPE_B_POINT_MASK),x_guidance_end,current_strip_y);

        flight_course_length= x_guidance_end - x_guidance ;
    }
    else
    {
        throw "first_valid_exposure_x > last_valid_exposure_x !";

    }



    return flight_course_length;

}

void PolygonAreaFlightRouteDesign::CreateNewFilghtPoint(
                          unsigned int strip_id,
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
    m_route_design_plane.__flight_point.push_back(pt);

    m_current_strip.AddPoint(pt);

}

void PolygonAreaFlightRouteDesign::AirportOrientationInOrthoPlane(
        Point2D airport2d,
        Point2D leftTop,
        Point2D rightBot,
        Point2D& orthoplane_center,
        bool& isAirportleft,
        bool& isAirportUp)
{
    orthoplane_center = (leftTop+rightBot)/2.0;

    Point2D airport_relative_loc = airport2d - orthoplane_center;

    isAirportleft   = airport_relative_loc.X> 0 ? false : true;
    isAirportUp     = airport_relative_loc.Y>=0 ? true : false;


}

// flip the flight points on the design plane
//if isAirportleft==false, then flip the flight points on X axis
//if isAirportUp == false, then flip the flight points on Y axis
void PolygonAreaFlightRouteDesign::FlipOrthoPlaneOrientation(
        Point2D orthoplane_center,
        bool isAirportleft,
        bool isAirportUp)
{
    std::vector< UAVFlightPoint >::iterator it= m_route_design_plane.__flight_point.begin();

    for( ; it!= m_route_design_plane.__flight_point.end() ; it++ )
    {
        Point2D curfightpt = it->ToGomoPoint2D();

        //flip operation
        curfightpt.X= (isAirportleft==true) ? curfightpt.X : -(curfightpt.X- orthoplane_center.X)+ orthoplane_center.X;
        curfightpt.Y= (isAirportUp  ==true) ? curfightpt.Y : -(curfightpt.Y- orthoplane_center.Y)+ orthoplane_center.Y;

        it->__longitude = curfightpt.X;
        it->__latitude  = curfightpt.Y;
    }

}


void PolygonAreaFlightRouteDesign::OutputRouteFile()
{
    qDebug("PolygonAreaFlightRouteDesign::OutputRouteFile()");

    FlightRouteDesign::OutputRouteFile();

}
