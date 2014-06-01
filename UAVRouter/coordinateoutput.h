#ifndef COORDINATEOUTPUT_H
#define COORDINATEOUTPUT_H

/**
 * \file CoordinateOutput.h
 *
 * \brief Output the Coordinate in packed format using by *.gst file(V1.2 without file header)
 *
 * \history
 *       Date:  2014.04.27 First implementation,tested by unit test
 *
 *
 */

#include <sstream>
using std::ostringstream;

#include <string>
using std::string;

#include <cstdint>

namespace Gomo {

namespace FlightRoute {

    // according to <航摄设计文件文本规范V1.2>
    class CoordinateOutput
    {
    public:
        CoordinateOutput();

        void SetAsDouble(double lat,double lon);
        void SetPointIdForEncryptionStep3A(const std::string strPointId);

        std::string GetAsEncryptString();    // packed coordinates string for normal flight point
        //std::string GetHeaderEncryptString();// packed coordinates string for file header

    protected:
        double m_coordinate_lat,m_coordinate_lon;

        std::string m_coordinate_text_encrypt;

        unsigned char m_pointIdForXor;

        std::string ConvertSingleCoordinateToString(double coord);

        std::string Step1_to16Chars(double lat,double lon);
        void Step2_to8Bytes(const std::string& step1_out, char* step2_out  );
        void Step3_Encrypt8Bytes( const char* step2_out, char* step3_out );
        std::string Step4_16Chars(const char* step3_out);

    };

}

}


#endif // COORDINATEOUTPUT_H
