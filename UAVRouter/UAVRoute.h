#ifndef UAVROUTE_H
#define UAVROUTE_H

/**
 * \file UAVRoute.h
 *
 * \brief define the UAVRoute file format and output method
 *
 * \history
 *       Date:  2013.11.09 First implementation of
 *
 *
 */


//This File is is the defination of UAV route design data struct

#include <cmath>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <sstream>
using std::ostringstream;


#include <algorithm>


#include <iomanip>
using std::ios;

#include "GomoGeometry2D.h"
using namespace Gomo::Geometry2D;

#include <QDebug>


#include <cstdint>


#include "ogrsf_frmts.h"

#include"coordinateoutput.h"



#define COORDINATE_DEGREE_PRECISION 5
#define SPACE " "

#define FRAME_DATA_LENGTH_IN_BYTE8 22


//#define LITTLE_ENDIAN_TO_BIG_ON

namespace Gomo {

namespace FlightRoute {

    typedef unsigned int   UINT4;
    typedef unsigned short WORD16;
    typedef unsigned char  BYTE8;
    typedef unsigned long long ULong64;

    // 2-byte number
    inline WORD16 SHORT_little_endian_TO_big_endian(WORD16 i)
    {
        return ((i>>8)&0xff)+((i << 8)&0xff00);
    };

    // 4-byte number
    inline UINT4 INT_little_endian_TO_big_endian(UINT4 i)
    {
        return((i&0xff)<<24)+((i&0xff00)<<8)+((i&0xff0000)>>8)+((i>>24)&0xff);
    };

    inline uint64_t swap_uint64( uint64_t val )
    {
        val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
        val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
        return (val << 32) | (val >> 32);
    }

    // pack 8 byte integer to 6 bytes array
    inline void ULong64Pack(ULong64 source, BYTE8 * pDest,int nBytes)
    {
        if(sizeof(ULong64)!=8)
        {
            throw("ULong64)!=8");
        }

        ULong64 src_swaped= swap_uint64(source);

        BYTE8 * pSource = (BYTE8 *)&src_swaped;

        for(int i=0;i<nBytes;i++)
        {
            *(pDest+i)=*(pSource+2+i);
        }

    };

    // pack double into 4 byte
    inline UINT4 DoublePackUnsigned(double double_num)
    {
        if(sizeof(UINT4)!=4)
        {
            throw("sizeof(UINT4)!=4");
        }

        unsigned float f_num = double_num;

        UINT4 packed= f_num+0.5;//for round

        UINT4 big_endian_pack= INT_little_endian_TO_big_endian(packed);


#ifdef LITTLE_ENDIAN_TO_BIG_ON
        return big_endian_pack;
#else
        return packed;
#endif


    };

    // pack the coordinate into 4 byte
    inline UINT4 CoordinatePack(double lat_long_in_degree)
    {
        if(sizeof(UINT4)!=4)
        {
            throw("sizeof(UINT4)!=4");
        }

        lat_long_in_degree *= 1e5;

        int toint = lat_long_in_degree+0.5;//for round

        UINT4 packed;
        if(toint>=0)
        {
            packed= toint;
        }
        else
        {
            //To do : add logic here:
            packed=0;
            throw "not support yet!";
        }


//        streamdebug<< "lat_long_in_degree="<<lat_long_in_degree<<std::endl;
//        streamdebug<<"toint="<<toint<<std::endl;
//        streamdebug<<"packed="<<packed<<std::endl;
//        streamdebug<<std::hex<<"packed in HEX="<<packed<<std::endl;

        UINT4 big_endian_pack= INT_little_endian_TO_big_endian(packed);
        //streamdebug<<"big_endian_pack="<<big_endian_pack<<std::endl;
        //qDebug(streamdebug.str().c_str());

#ifdef LITTLE_ENDIAN_TO_BIG_ON
        return big_endian_pack;
#else
        return packed;
#endif


    };

    // pack the height into 2 byte
    inline WORD16 HeightPack(double height)
    {
        if(sizeof(short)!=2)
        {
            throw("sizeof(short)!=2");
        }

        short height_short = height+0.5;// for round

        WORD16 packed;

        memcpy_s(&packed,
                 2,
                 &height_short,
                 2);

#ifdef LITTLE_ENDIAN_TO_BIG_ON
        return SHORT_little_endian_TO_big_endian(packed);
#else
        return packed;
#endif

    };



    typedef struct UAVROUTE_DATA_FRAME
    {
        static const WORD16  frameHeader = 0x09BE;
        BYTE8   data[FRAME_DATA_LENGTH_IN_BYTE8-1];
        BYTE8 checksum;

        inline BYTE8 checksum_data_XOR()
        {
            BYTE8 * pdata = (BYTE8 *)data;

            BYTE8 xor_sum = pdata[0];

            for (int i=1; i<FRAME_DATA_LENGTH_IN_BYTE8-1;i++)
            {
                xor_sum = xor_sum ^ pdata[i];
            }

            return xor_sum;


        };


        void OutputBinary(std::ostream & out_stream, bool encrypt=true);

        void OutputBinaryV2(std::ostream & out_stream, bool encrypt=true);

        WORD16 encryptWORD( const WORD16& word_obj);
        void SequentialXor(BYTE8* source,int length);      
       


    } UAVRouteDataFrame;

    typedef struct UAVROUTE_HEADER
    {
        //First line in txt: flight region
        double min_latitude,min_longitude;   //in output in  "degree.?????"
        double max_latitude,max_longitude;   //in output in "degree.?????"

        //Second Line in txt: Airport
        std::string  airport_name;             // max char number: 10
        double airport_latitude;             //output in "degree.?????"
        double airport_longitude;            //output in "degree.?????"
        double airport_height;               //in " meter. "

        inline void Output(std::ostream & out_stream,bool encrypt=false) const
        {
            //BEGIN FLAG
            out_stream<<"BEGIN"<<"\n";
            int intAirportHeight=airport_height;

            std::string airport_upper(airport_name);
            std::transform(airport_name.begin(), airport_name.end(), airport_upper.begin(), toupper);

            if(encrypt==false)
            {
                out_stream<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint);
                out_stream.precision(COORDINATE_DEGREE_PRECISION);

                //first line
                out_stream<<min_latitude
                          <<SPACE<<max_latitude
                          <<SPACE;

                ios state(nullptr);
                //output the height and pt_class
                state.copyfmt(out_stream); // save current formatting
                out_stream << std::setw(COORDINATE_DEGREE_PRECISION+4)
                          << std::setfill('0')
                          <<min_longitude
                          <<SPACE
                          << std::setw(COORDINATE_DEGREE_PRECISION+4)
                          << std::setfill('0')
                          <<max_longitude<<"\n";
                out_stream.copyfmt(state); // restore previous formatting


                //second line
                out_stream<<airport_upper
                          <<SPACE<<airport_latitude
                          <<SPACE;

                //output the height and pt_class
                state.copyfmt(out_stream); // save current formatting
                out_stream << std::setw(COORDINATE_DEGREE_PRECISION+4)
                          << std::setfill('0')
                          <<airport_longitude;
                out_stream.copyfmt(state); // restore previous formatting


                //output the height
                state.copyfmt(out_stream); // save current formatting
                out_stream <<SPACE
                          << std::setw(4)
                          << std::setfill('0')
                          <<intAirportHeight
                          <<"\n";
                out_stream.copyfmt(state); // restore previous formatting
            }
            else
            {
//                CoordinateOutput coordoutput;

//                //first line
//                coordoutput.SetAsDouble(min_latitude,max_latitude);
//                out_stream<<coordoutput.GetAsEncryptString();
//                coordoutput.SetAsDouble(min_longitude,max_longitude);
//                out_stream<<coordoutput.GetAsEncryptString()<<"\n";

//                //second line
//                coordoutput.SetAsDouble(airport_latitude,airport_longitude);
//                out_stream<<airport_upper
//                          <<SPACE<<coordoutput.GetAsEncryptString()
//                          <<SPACE<<intAirportHeight<<"\n";

            }

        };

        void OutputBinary(std::ostream & out_stream) const;

    }UAVRouteHEADER;

    typedef enum FLIGTH_POINT_TYPE
    {
        FLIGTH_POINT_TYPE_GUIDE = 1,
        FLIGTH_POINT_TYPE_ETRANCE_EXIT = 2,
        FLIGTH_POINT_TYPE_EXPOSURE = 3,
        FLIGTH_POINT_TYPE_A_POINT_MASK =8, // the direction of getting  into the strip
        FLIGTH_POINT_TYPE_B_POINT_MASK =16  // the direction of leaving the strip

    }enumFlightPointType;

    typedef struct UAVROUTE_FLIGHT_POINT
    {
        unsigned char __strip_id;       // 1~99
        unsigned int  __id_in_strip;
        double       __latitude;
        double       __longitude;
        double       __height;
        enumFlightPointType __flight_point_type;

        inline UAVROUTE_FLIGHT_POINT &
            operator=(const UAVROUTE_FLIGHT_POINT & rs)
            {
                __strip_id=rs.__strip_id;
                __id_in_strip=rs.__id_in_strip;
                __latitude=rs.__latitude;
                __longitude=rs.__longitude;
                __height=rs.__height;
                __flight_point_type=rs.__flight_point_type;

                return *this;
            };

        void Output(std::ostream & out_stream,bool encrypt=false) const;

       // void OutputTextEncrypted(std::ostream & out_stream) const;

        void OutputBinary(std::ostream & out_stream) const;

        void ToOGRFeature( OGRLayer *poLayer,OGRFeature ** ppOFeature) const;
        OGRPoint ToOGRPoint() const;

        Gomo::Geometry2D::Point2D ToGomoPoint2D()const;

    } UAVFlightPoint;


    typedef struct UAVFLIGHT_STATISTIC_INFO
    {
        float __MBR_Area;               // in sqr meters
        float __flight_region_area;     // in sqr meters
        unsigned int  __count_exposures;        // count of flight points of FLIGTH_POINT_TYPE_ETRANCE_EXPOSUR
        unsigned char __count_strips;           // count of strips
        float __photo_flight_course_chainage;   // the sum of valid flight course,maybe sum of baselines' length

        /*
         *为5行汉字标识的统计信息，每行都是一串汉字后紧跟一“:”，然后空一格，
         *再接相应的数据，其中面积和长度都是浮点数，默认单位分别是平方米和米；
         *点数和航线数为整数。
         *这5种标识分别是：
         *摄区外接矩形面积、摄区多边形面积、曝光点(标识为3的点)总数、航线总数、摄影航线航程总长度。
         */
        inline void Output(std::ostream & out_stream) const
        {
            unsigned int count_strips= __count_strips;

            out_stream<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint);
            out_stream.precision(2);

            out_stream<<"MBR Area(m2):"<< __MBR_Area<<"\n";
            out_stream<<"Flight Region Polygon Area(m2):"<< __flight_region_area <<"\n";
            out_stream<<"Exposure Points Count:"<< __count_exposures<<"\n";
            out_stream<<"Strips Count:"<< count_strips<<"\n";
            out_stream<<"Flight Course Length(m):"<< __photo_flight_course_chainage<<"\n";

        };

        void OutputBinary(std::ostream & out_stream) const;


    }UAVFlightStatisticInfo;

    typedef struct UAVFLIGHT_ROUTE_DESIGN
    {
        UAVRouteHEADER __header;

        std::vector< UAVFlightPoint > __flight_point;

        UAVFlightStatisticInfo __flight_statistic;

    public:
        UAVFLIGHT_ROUTE_DESIGN& operator=(const UAVFLIGHT_ROUTE_DESIGN & rs);
        UAVFLIGHT_ROUTE_DESIGN& operator +=(const UAVFLIGHT_ROUTE_DESIGN & rs);

    }UAVRouteDesign;

    typedef struct UAV_FLIGHT_STRIP
    {
        std::vector< UAVFlightPoint > __flight_point;
        int __count_exposures;

    public:
        void Clear();
        void AddPoint(const UAVFlightPoint & pt);

        UAV_FLIGHT_STRIP& operator =(const UAV_FLIGHT_STRIP&);


    }UAVRouteStrip;

}

}

#endif // UAVROUTE_H
