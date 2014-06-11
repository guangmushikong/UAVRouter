#ifndef MULTIREGIONDESIGNER_H
#define MULTIREGIONDESIGNER_H

/// MultiRegionDesigner: public FlightRouteDesign
/// special designer for combine the output of multi regions designing
/// Note: the multi-region is sequential dependent becasue the previous exit point
///       would be considered as the airport of current intermediate region



#include "flightroutedesign.h"

using namespace Gomo::FlightRoute;
using Gomo::FlightRoute::FlightParameter;

class MultiRegionDesigner: public FlightRouteDesign
{
public:
    MultiRegionDesigner();
    MultiRegionDesigner(const FlightParameter & parameter);


public:
    //void SetFightParameters(FlightParameter & parameter );

    void PerformRouteDesign();


/// space holders to make the function not virtual
protected:
    // space holder to make the function not virtual
    virtual void DesignInGaussPlane();
    // space holder to make the function not virtual
    virtual void CreateNewFilghtPoint(unsigned int strip_id,
                              unsigned int id_in_strip,
                              enumFlightPointType ptType,
                              double longitude,
                              double latitude );

};

#endif // MULTIREGIONDESIGNER_H
