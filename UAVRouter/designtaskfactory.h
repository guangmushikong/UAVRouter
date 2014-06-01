#ifndef DESIGNTASKFACTORY_H
#define DESIGNTASKFACTORY_H

#include "flightroutedesign.h"
#include "flightparameter.h"

class DesignTaskFactory
{
public:
    DesignTaskFactory();

public:
    static FlightRouteDesign * CreateFlightRouteDeigner(FlightParameter & parameter);

};

#endif // DESIGNTASKFACTORY_H
