#include "designtaskfactory.h"
#include "linearflightroutedesign.h"
#include "polygonareaflightroutedesign.h"
#include "multiregiondesigner.h"

#include <QDebug>

DesignTaskFactory::DesignTaskFactory()
{
}


FlightRouteDesign *
    DesignTaskFactory::CreateFlightRouteDeigner(FlightParameter & parameter)
{

    FlightRouteDesign * flightdes=NULL;

    if(parameter.GetRegionCount()>1)
    {
        return new MultiRegionDesigner(parameter);
    }
    else
    {
        if(wkbFlatten(parameter.FightRegion.get()->getGeometryType()) == wkbLineString)
        {
            qDebug("Create Linear flightt route designer");
            return new LinearFlightRouteDesign(parameter);
        }
        else if (wkbFlatten(parameter.FightRegion.get()->getGeometryType()) == wkbPolygon)
        {
            qDebug("Create polygon flightt route designer");
            return new PolygonAreaFlightRouteDesign(parameter);

        }
        else
        {
            throw "Unknown GeometryType of parameter.FightRegion in DesignTaskFactory::CreateFlightRouteDeigner";
        }

    }



    return flightdes;

}
