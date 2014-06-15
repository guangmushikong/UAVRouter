#include "flightparameter.h"


namespace Gomo {

    namespace FlightRoute {

    FlightParameter::FlightParameter()
    {
		FightRegion = std::auto_ptr<OGRGeometry>(NULL);
        multiFlightRegionGeometries.clear();
    }


    FlightParameter& FlightParameter::
        operator=(const FlightParameter & rs)
    {
             CameraInfo         = rs.CameraInfo;
             AverageElevation   = rs.AverageElevation;
             FightHeight        = rs.FightHeight;
             GuidanceEntrancePointsDistance = rs.GuidanceEntrancePointsDistance;
             overlap            = rs.overlap;
             overlap_crossStrip = rs.overlap_crossStrip;
             RedudantBaselines  = rs.RedudantBaselines;             
             airport            = rs.airport;

			 if( rs.FightRegion.get()==NULL)
			 {
				 FightRegion = std::auto_ptr<OGRGeometry>(NULL);
			 }
			 else
			 {
				 FightRegion = std::auto_ptr<OGRGeometry>(rs.FightRegion.get()->clone());
			 }			 

             multiFlightRegionGeometries.clear();
             std::vector<std::auto_ptr<OGRGeometry>>::const_iterator it_reg = rs.multiFlightRegionGeometries.begin();
             for( ; it_reg!= rs.multiFlightRegionGeometries.end(); it_reg++ )
             {
                 multiFlightRegionGeometries.push_back( std::auto_ptr<OGRGeometry>((*it_reg).get()->clone()) );
             }

             return *this;

    }

    size_t FlightParameter::AddFlightRegionGeometry(std::auto_ptr<OGRGeometry> region_ptr)
    {

        multiFlightRegionGeometries.push_back(std::auto_ptr<OGRGeometry>( region_ptr.get()->clone()) );

        return multiFlightRegionGeometries.size();
    }

    }
}




