#include "flightroutedesign.h"

#include "geomertyconvertor.h"
using namespace Gomo::Geometry2D;

#include <QDebug>
#include <sstream>
using std::ostringstream;

#include <QFile>
#include <QFileInfo>

#include "uavrouteoutputer.h"

FlightRouteDesign::FlightRouteDesign()
{
    m_FightRegion_Gauss = std::auto_ptr<OGRGeometry>(NULL);//add this for the auto destructor, otherwise the m_FightRegion_Gauss would free the random memory

    InitialSpatialReference();
}


FlightRouteDesign::FlightRouteDesign(const FlightParameter & parameter)
{
    m_parameter=parameter;

    m_FightRegion_Gauss = std::auto_ptr<OGRGeometry>(NULL);//add this for the auto destructor, otherwise the m_FightRegion_Gauss would free the random memory

    ScaleCamera2Ground();

    InitialSpatialReference();

}

void FlightRouteDesign::InitialSpatialReference()
{
    m_ProjTM.SetProjCS("Local Guass-Krugger on WGS84");
    m_ProjTM.SetWellKnownGeogCS( "WGS84" );
}

void FlightRouteDesign::ScaleCamera2Ground()
{
    qDebug("FlightRouteDesign::ScaleCamera2Ground()");
    ostringstream streamdebug;

    m_scale= m_parameter.CameraInfo.f / (m_parameter.FightHeight - m_parameter.AverageElevation ) /1000;

    m_camera_height_to_ground   = ((m_parameter.CameraInfo.height *  m_parameter.CameraInfo.pixelsize)/m_scale)/1000;
    m_camera_width_to_ground    = ((m_parameter.CameraInfo.width *   m_parameter.CameraInfo.pixelsize)/m_scale)/1000;

    m_baseline_length = m_camera_width_to_ground * (1.0 - m_parameter.overlap) ;
    m_cross_strip_distance = m_camera_height_to_ground * (1.0 - m_parameter.overlap_crossStrip);

    streamdebug.str("");
    streamdebug<< "camera_height_to_ground: "<<m_camera_height_to_ground<<",";
    streamdebug<< "camera_width_to_ground: "<<m_camera_width_to_ground  <<",";
    streamdebug<< "baseline_length: "<<m_baseline_length  <<",";
    streamdebug<< "cross_strip_distance: "<<m_cross_strip_distance  <<";";
    qDebug(streamdebug.str().c_str());
}

void FlightRouteDesign::PerformRouteDesign()
{
    qDebug("FlightRouteDesign::PerformRouteDesign()");

    GaussProjection();

    DesignInGaussPlane();

    InverseGaussProjection();

}

void FlightRouteDesign::GaussProjection()
{
    ostringstream streamdebug;
    qDebug("FlightRouteDesign::GaussProjection()");

    m_FightRegion_Gauss=std::auto_ptr<OGRGeometry>( m_parameter.FightRegion.get()->clone() );
    m_AirportLoc_Gauss =m_parameter.airport.GetLocation();

    //Get centroid to create projection parameter
    OGRPoint region_ceneter;
    m_FightRegion_Gauss.get()->Centroid(&region_ceneter);
    double centermeridian=region_ceneter.getX();
    double centerlat=region_ceneter.getY();
    streamdebug.str("");
    streamdebug<< "centermeridian of the region: "<<centermeridian;
    qDebug(streamdebug.str().c_str());

    double x_airport=m_AirportLoc_Gauss.getX();
    double y_airport=m_AirportLoc_Gauss.getY();
    streamdebug.str("");
    streamdebug<<"airport lat,long:("<<x_airport<<","<<y_airport<<")";
    qDebug(streamdebug.str().c_str());

    //define spatial reference and transform
    OGRSpatialReference    *poLatLong;
    OGRCoordinateTransformation *poTransform;
    m_ProjTM.SetTM( 0, centermeridian, 0.9996,
           500000.0, (centerlat >= 0.0) ? 0.0 : 10000000.0 );
    poLatLong = m_ProjTM.CloneGeogCS();

    poTransform = OGRCreateCoordinateTransformation( poLatLong,&m_ProjTM);

    //Guass projection of airport
    m_AirportLoc_Gauss.transform(poTransform);
    streamdebug.str("");
    streamdebug<<"airport after project:("<<m_AirportLoc_Gauss.getX()<<","<<m_AirportLoc_Gauss.getY();
    qDebug(streamdebug.str().c_str());

    //Guass projection of flight region polygon or polyline
    char * geom_before_proj_wkt="";
    m_FightRegion_Gauss.get()->exportToWkt(&geom_before_proj_wkt);
    streamdebug.str("");
    streamdebug<<"flight region geometry before gauss projection:"<<geom_before_proj_wkt<<std::endl;
    qDebug(streamdebug.str().c_str());
    m_FightRegion_Gauss.get()->transformTo(&m_ProjTM);


}

// form m_route_design_CaussProj to m_route_design_WGS84
void FlightRouteDesign::InverseGaussProjection()
{

    //define spatial reference and transform
    OGRSpatialReference    *poLatLong;
    OGRCoordinateTransformation *poTransform;

    poLatLong = m_ProjTM.CloneGeogCS();
    poTransform = OGRCreateCoordinateTransformation( &m_ProjTM,poLatLong);

    //---------------------------------------------------------------
    //Main section: flight strips:
    //----------------------------------------------------------------
    std::vector< UAVFlightPoint >::iterator it= m_route_design_CaussProj.__flight_point.begin();

    for( ; it!= m_route_design_CaussProj.__flight_point.end() ; it++ )
    {
        UAVFlightPoint flight_pt_wgs84;
        flight_pt_wgs84.__strip_id         = (*it).__strip_id;
        flight_pt_wgs84.__id_in_strip      = (*it).__id_in_strip;
        flight_pt_wgs84.__flight_point_type= (*it).__flight_point_type;

        OGRPoint pt((*it).__longitude,(*it).__latitude,m_parameter.FightHeight );
        pt.transform(poTransform);

        flight_pt_wgs84.__longitude = pt.getX();
        flight_pt_wgs84.__latitude  = pt.getY();
        flight_pt_wgs84.__height    = m_parameter.FightHeight;

        m_route_design_WGS84.__flight_point.push_back(flight_pt_wgs84);
    }

    //------------------------------------------------------------------------
    // Header
    //------------------------------------------------------------------------
    OGREnvelope env;
    dynamic_cast<OGRPolygon*>(m_parameter.FightRegion.get())->getEnvelope(&env);
    m_route_design_WGS84.__header.max_latitude  =env.MaxY;
    m_route_design_WGS84.__header.min_latitude  =env.MinY;
    m_route_design_WGS84.__header.min_longitude =env.MinX;
    m_route_design_WGS84.__header.max_longitude =env.MaxX;

    m_route_design_WGS84.__header.airport_height = m_parameter.airport.getZ();
    m_route_design_WGS84.__header.airport_longitude = m_parameter.airport.getX();
    m_route_design_WGS84.__header.airport_latitude = m_parameter.airport.getY();
    m_route_design_WGS84.__header.airport_name = m_parameter.airport.GetName();

    //------------------------------------------------------------------------
    // Statistic
    //------------------------------------------------------------------------
    m_route_design_WGS84.__flight_statistic.__count_exposures = m_route_design_CaussProj.__flight_statistic.__count_exposures;
    m_route_design_WGS84.__flight_statistic.__count_strips = m_route_design_CaussProj.__flight_statistic.__count_strips;
    m_route_design_WGS84.__flight_statistic.__flight_region_area = m_route_design_CaussProj.__flight_statistic.__flight_region_area;
    m_route_design_WGS84.__flight_statistic.__MBR_Area = m_route_design_CaussProj.__flight_statistic.__MBR_Area;
    m_route_design_WGS84.__flight_statistic.__photo_flight_course_chainage = m_route_design_CaussProj.__flight_statistic.__photo_flight_course_chainage;

}




void FlightRouteDesign::OutputRouteFile()
{
    QString suf_ght("ght");
    QString suf_bht("bht");
    QString suf_kml("kml");
    QString suf_gst("gst");

    std::vector<std::string>::iterator it=m_output_files.begin();

    for( ; it!=m_output_files.end();it++)
    {
        QString output((*it).c_str());

        QFileInfo fi(output);
        QString suffix=fi.suffix();

        if (suffix.compare(suf_ght, Qt::CaseInsensitive) ==0)
        {
            qDebug("suf_ght");
            qDebug((*it).c_str());

            UAVRouteOutputer::OutputRouteDesignFileAsText(m_route_design_WGS84,*it);
        }

        if (suffix.compare(suf_bht, Qt::CaseInsensitive) ==0)
        {
            qDebug("suf_bht");
            qDebug((*it).c_str());
            UAVRouteOutputer::OutputRouteDesignFileAsBinary(m_route_design_WGS84,*it);
        }

        if (suffix.compare(suf_kml, Qt::CaseInsensitive) ==0)
        {
            qDebug("suf_kml");
            qDebug((*it).c_str());
            UAVRouteOutputer::OutputRouteDesignFileAsKML(m_route_design_WGS84,*it);
        }

        if (suffix.compare(suf_gst, Qt::CaseInsensitive) ==0)
        {
            qDebug("suf_gst");
            qDebug((*it).c_str());
            UAVRouteOutputer::OutputRouteDesignFileAsTextEncrypted(m_route_design_WGS84,*it);
        }


    }

}


void FlightRouteDesign::AddOutPutFileName(std::string outputfilename)
{
    m_output_files.push_back(outputfilename);

}


UAVFlightPoint FlightRouteDesign::GetLastFlightPoint()
{
    return m_route_design_WGS84.__flight_point[m_route_design_WGS84.__flight_point.size()-1];
}

void FlightRouteDesign::ShareDesign(const FlightRouteDesign & src,bool append)
{
    if( append == false) //means just copy
    {
        m_route_design_WGS84 = src.m_route_design_WGS84;
    }
    else
    {
        //expand the first design by the second design
        m_route_design_WGS84 += src.m_route_design_WGS84;

    }
}
