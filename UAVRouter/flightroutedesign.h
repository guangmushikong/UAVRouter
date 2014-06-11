#ifndef FLIGHTROUTEDESIGN_H
#define FLIGHTROUTEDESIGN_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "flightparameter.h"
#include "GomoGeometry2D.h"
using namespace Gomo::Geometry2D;

#include "UAVRoute.h"
using namespace Gomo::FlightRoute;

#include "uavrouteoutputer.h"

class FlightRouteDesign
{
public:
    FlightRouteDesign();
    FlightRouteDesign(const FlightParameter & parameter);

    virtual void PerformRouteDesign();
    virtual void OutputRouteFile();

    void AddOutPutFileName(std::string);

    //for convenience of multi-region routing connection,
    //provide the last flight point of the current region as the airport of next region
    UAVFlightPoint GetLastFlightPoint();


protected:
    vector<std::string> m_output_files;

    void InitialSpatialReference();
    void GaussProjection();
    void InverseGaussProjection();

    virtual void DesignInGaussPlane()=0;

    void ScaleCamera2Ground(); // calculate the scale and the rectangle on the ground for each photo

    virtual void CreateNewFilghtPoint(unsigned int strip_id,
                              unsigned int id_in_strip,
                              enumFlightPointType ptType,
                              double longitude,
                              double latitude )=0;

    //for multi-region data share
    void ShareDesign(const FlightRouteDesign & src,bool append=false);


protected:
    FlightParameter m_parameter;

    double m_scale;
    double m_baseline_length; // in meters
    double m_cross_strip_distance;  // in meters

    // the rectangle that project from camera frame to ground
    float m_camera_width_to_ground;
    float m_camera_height_to_ground;

    std::auto_ptr<OGRGeometry> m_FightRegion_Gauss;//摄区, 面状或者线状,in guass proj
    OGRPoint   m_AirportLoc_Gauss;           // 机场中心,in guass proj

    UAVRouteDesign m_route_design_CaussProj;
    UAVRouteDesign m_route_design_WGS84;

    //for Guass(Tranverse Mecator) projection
    double m_major_meridian;
    OGRSpatialReference    m_ProjTM;

};

#endif // FLIGHTROUTEDESIGN_H
