#ifndef LINEARFLIGHTROUTEDESIGN_H
#define LINEARFLIGHTROUTEDESIGN_H

#include "flightroutedesign.h"

class LinearFlightRouteDesign : public FlightRouteDesign
{
public:
    LinearFlightRouteDesign();
    LinearFlightRouteDesign(const  FlightParameter & parameter);

    void PerformRouteDesign();
    virtual void OutputRouteFile();

protected:
    virtual void DesignInGaussPlane();
    void CreateNewFilghtPoint(unsigned int strip_id,
                              unsigned int id_in_strip,
                              enumFlightPointType ptType,
                              double longitude,
                              double latitude );

    double CreateLinearStrip(int strip_id,
                        const Point2D & begin,
                        const Point2D & end);
};

#endif // LINEARFLIGHTROUTEDESIGN_H
