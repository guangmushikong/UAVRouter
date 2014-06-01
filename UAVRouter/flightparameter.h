#ifndef FLIGHTPARAMETER_H
#define FLIGHTPARAMETER_H

#include <ogrsf_frmts.h>

#include "GomoPhotoBasic.h"
using namespace Gomo::PhotoCamera;

#include <string>
#include <memory>

using std::string;


namespace Gomo {

    namespace FlightRoute {

        class Airport
        {
        protected:
            OGRPoint   AirportLocation;           // 机场中心,in WGS84
            std::string TransliteratedName;       // in pinyin.china

        public:
            inline Airport&
                operator=(const Airport & rs){

                AirportLocation    = rs.AirportLocation;
                TransliteratedName = rs.TransliteratedName;

                return *this;
            };

            double
            getX() const { return AirportLocation.getX(); };
            double
            getY() const { return AirportLocation.getY(); };
            double
            getZ() const { return AirportLocation.getZ(); };

            OGRPoint GetLocation(){
                return AirportLocation;};
            void SetLocation(const OGRPoint & airportLoc){
                AirportLocation = airportLoc; };

            std::string GetName(int language=1 ){
                return TransliteratedName;};
            void SetName(const std::string& eng_name="Airport" ){
                TransliteratedName = eng_name;
            };


        };

        struct FlightParameter
        {
            enum REGION_TYPE
            {
                REGION_SINGLE_POLYGON,

                REGION_MULIT_POLYGON
            };


        protected:
            REGION_TYPE m_region_type;

        public:
            FlightParameter();

            DigitalCameraInfo CameraInfo;

            double AverageElevation;  //摄区地面平均高程,m,in WGS84
            double FightHeight;       //航高,m,in WGS84

            double GuidanceEntrancePointsDistance;  // 引导点,进入点距离,m
            double overlap;                         // (0,1)
            double overlap_crossStrip;              // 旁向重叠度

            unsigned int RedudantBaselines;         // 冗余基线

            std::auto_ptr<OGRGeometry> FightRegion;//摄区, 面状或者线状
            //OGRPoint   AirportLocation;           // 机场中心,in WGS84
            Airport airport;

        public:
            inline FlightParameter&
                operator=(const FlightParameter & rs){
                     CameraInfo         = rs.CameraInfo;
                     AverageElevation   = rs.AverageElevation;
                     FightHeight        = rs.FightHeight;
                     GuidanceEntrancePointsDistance = rs.GuidanceEntrancePointsDistance;
                     overlap            = rs.overlap;
                     overlap_crossStrip = rs.overlap_crossStrip;
                     RedudantBaselines  = rs.RedudantBaselines;
                     FightRegion        = std::auto_ptr<OGRGeometry>(rs.FightRegion.get()->clone());
                     airport            = rs.airport;
                     return *this;
                 };

        };


    }
}



#endif // FLIGHTPARAMETER_H
