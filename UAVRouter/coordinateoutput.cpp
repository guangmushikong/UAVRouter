#include "coordinateoutput.h"

#include <iomanip>
using std::ios;


namespace Gomo {

namespace FlightRoute {

CoordinateOutput::CoordinateOutput()
{
    m_coordinate_lat=m_coordinate_lon=0;
    m_coordinate_text_encrypt="";
    m_pointIdForXor =0;
}


void CoordinateOutput::SetAsDouble(double lat,double lon)
{
    m_coordinate_lat=lat;
    m_coordinate_lon=lon;

}

void CoordinateOutput::SetPointIdForEncryptionStep3A(const std::string strPointId)
{
    int len = strPointId.length();

	unsigned char last		=0;      
    unsigned char prelast	=0;   

    

	std::string binary_values;
	for(int i=0; i< len; i++)
	{
		unsigned char cha = strPointId[i];
		if (cha>='0' && cha<='9' )
		{
				cha -= '0';
		}
		else if(cha>='A' && cha<='F')
		{
				cha = cha - 'A' + 0xA;
		}

		binary_values.push_back(cha);
	}

	if(len ==1)
    {
        last	= binary_values[0] ;
    }
    else 
    {
		last	= binary_values[len-1];
        prelast	= binary_values[len-2];
    }


    m_pointIdForXor  = ( (prelast<<4) & 0xF0 ) | (last & 0x0F);


}

std::string CoordinateOutput::ConvertSingleCoordinateToString(double coord)
{
    double lat_long_in_degree = coord*1e5;

    long tolong = lat_long_in_degree+0.5;//for round

    ostringstream stream_coord;
    stream_coord.str("");

    stream_coord << std::setw(8)
         << std::setfill('0')
         << tolong;     // the actual value we wanted to print out

    std::string str8chars= stream_coord.str();

    if(tolong <0)
    {
        str8chars[0]='F';
    }

    return str8chars;
}

std::string CoordinateOutput::Step1_to16Chars(double lat,double lon)
{
    std::string step1=ConvertSingleCoordinateToString(lat)+ConvertSingleCoordinateToString(lon);

    if(step1.length()!=16)
    {
        throw "step1.length()!=16";
    }

    return step1;

}


void CoordinateOutput::Step2_to8Bytes(const std::string& step1_out, char * step2_out )
{

    int step1_length=step1_out.length();

    if(step1_length!=16 )
    {
        throw "step1_length!=16 ";
    }

    unsigned char pre_char=0;
    for(int i=0;i<16;i++)
    {
        unsigned char cha= (unsigned char)step1_out[i];

		if (cha>='0' && cha<='9' )
		{
			cha -= '0';
		}
		else if(cha>='A' && cha<='F')
		{
			cha = cha - 'A' + 0xA;
		}

        if( (i/2) * 2 ==i) // i is even, but in sequence is odd
        {
            pre_char=(cha<<4) & 0xF0; // just reserve its lowest 4 bits for filling the current output char's highest 4 bits
        }
        else
        {
            step2_out[i/2] = pre_char | (cha & 0x0F);
        }
    }

}


void CoordinateOutput::Step3_Encrypt8Bytes( const char* step2_out, char* step3_out )
{


    unsigned char first_sub_step[8];
    //1. first with 0XEA
    for(int i=0 ; i<8 ; i++)
    {
        first_sub_step[i] = step2_out[i] ^ 0XEA;
    }

    //2. then with new previous byte
    step3_out[0] = first_sub_step[0] ^ m_pointIdForXor;

    for (int j=1 ; j<8 ; j++)
    {
        step3_out[j] = first_sub_step[j] ^ step3_out[j-1];
    }


}

std::string CoordinateOutput::Step4_16Chars(const char* step3_out)
{
    char step4_out[17];

    for(int i=0 ; i<8 ; i++)
    {
        char packed = step3_out[i];

        step4_out[2*i]      =  (packed & 0xF0)>>4;
        step4_out[2*i+1]    =  packed & 0x0F;
    }

    for(int j=0 ; j<16 ; j++)
    {
        if ( step4_out[j] <=9  )
        {
            step4_out[j] = step4_out[j] + '0';
        }
        else
        {
            step4_out[j] = (step4_out[j]-0xA) + 'A' ;
        }
    }

    step4_out[16] = '\0';
    std::string result_str = step4_out;

    return result_str;

}


std::string CoordinateOutput::GetAsEncryptString()
{
    std::string step1=Step1_to16Chars(m_coordinate_lat,m_coordinate_lon);

    char step2_out[8];
    Step2_to8Bytes(step1, step2_out);

    /*uint64_t byte8test= *(uint64_t *)step2_out;
    ostringstream stream_8byte;
    stream_8byte.str("");
    stream_8byte << byte8test;*/

    char step3_out[8];
    Step3_Encrypt8Bytes(step2_out, step3_out);

    std::string step4= Step4_16Chars(step3_out);

    //std::string test = step1 +'_'+Step4_16Chars(step2_out);;//unit test pass: 02-004 0399142411651468_0399142411651468 300 3

    return step4;

}

//std::string CoordinateOutput::GetHeaderEncryptString()
//{
//    std::string step1=Step1_to16Chars(m_coordinate_lat,m_coordinate_lon);

//    char step2_out[8];
//    Step2_to8Bytes(step1, step2_out);

//    char step3_out[8];
//    Step3_Encrypt8Bytes(step2_out, step3_out);

//    std::string step4= Step4_16Chars(step3_out);

//    return step4;
//}


}

}
