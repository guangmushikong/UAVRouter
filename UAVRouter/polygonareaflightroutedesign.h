#ifndef POLYGONAREAFLIGHTROUTEDESIGN_H
#define POLYGONAREAFLIGHTROUTEDESIGN_H

#include "flightroutedesign.h"



class PolygonAreaFlightRouteDesign : public FlightRouteDesign
{
public:
    PolygonAreaFlightRouteDesign();
    PolygonAreaFlightRouteDesign(const FlightParameter & parameter);

    void PerformRouteDesign();
    virtual void OutputRouteFile( );

    //overide
protected:
    virtual void DesignInGaussPlane();

protected:

    ///
    ///highest level: Causs-projection Plane
    ///

    /*@param
     *      input:  polygon_2d
     *      output: center_2d,angle
     *@return
     *      true if succeed,otherwise false
     */
    bool CalculatePolygonOrientaion(const Point2DArray&  polygon_2d,Point2D& center_2d,double& angle);

    // 1.rotate the gauss projection axis to the m_angle_region, which is the orientation of the flight polygon
    // 2.move the gauss projection origin to m_region_center
    void PlaneTransform(const Point2DArray & polygonPoints_gauss, const Point2D & ptAirport );

    // recover the coods to gauss projection
    void InversePlaneTransform();

    ///
    ///middle level: Transformed Plane: centralized via the average point of Guass-projection coordinates, and rotated by the region angle on Guass-projection plane
    ///
    void DesignInTransformedCoords();

    // to make sure the Airport be put near the lefttop of the flight region box
    void FlipOrthoPlaneOrientation(Point2D orthoplane_center,
                                    bool isAirportleft,
                                    bool isAirportUp);

    //"Ortho Plane" is infact the plane after "PlaneTransform"
    // here we should figure out the relative orientation of the airport to the original point of "Ortho Plane";
    // then we could make a mirror transform to the output flight points to make the first flight point close to airport
    void AirportOrientationInOrthoPlane(
            Point2D airport2d,
            Point2D mbr_leftTop,
            Point2D mbr_rightBot,
            Point2D& mbr_center,
            bool& isAirportleft,
            bool& isAirportUp);


    ///
    ///lowest level: OrthoPlane,ie, design plane: strip parralle with X axis, and Airport close to left top of the region mbr
    ///

    void CreateNewFilghtPoint(unsigned int strip_id,
                              unsigned int id_in_strip,
                              enumFlightPointType ptType,
                              double longitude,
                              double latitude);

    // return: the length of strip, including reductant baselines and guidance
    double CreateNewStrip(int strip_id,double current_strip_y,double first_valid_exposure_x,double last_valid_exposure_x);

    // create the first strip(paralle with x axis)    
    double CreateFirstStrip(int strip_id,double current_strip_y,double first_valid_exposure_x,double last_valid_exposure_x);
    // reuse the x coordinates of the last strip but in a reverse direction
    double CreateNewStripBasedOnLastStrip(int strip_id,double current_strip_y);

protected:

    Point2DArray m_region_polygonPoints_planetransformed;
    Point2D      m_airport_planetransformed;

    Point2D      m_region_center_GuassProj;
    double       m_angle_region_GuassProj;

    UAVRouteDesign m_route_design_plane;


protected:
    //the following two members are used in CreateNewStripBasedOnLastStrip() for reuse the last valid strip
    UAVRouteStrip m_last_strip;
    UAVRouteStrip m_current_strip;


};

#endif // POLYGONAREAFLIGHTROUTEDESIGN_H
