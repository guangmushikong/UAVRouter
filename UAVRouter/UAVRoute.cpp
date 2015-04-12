
#include "UAVRoute.h"




namespace Gomo {

namespace FlightRoute {

    ///
    /**
     *@param word_obj
     *
     *@return the entrypted word_obj
     */
    WORD16  UAVROUTE_DATA_FRAME::encryptWORD( const WORD16& word_obj)
    {
        // this lookup table indicate the original bit index for each bit of the encrypted word
        int bit_encrypt_index[16]={1,3,5,7,9,11,13,15,
                                   0,2,4,6,8,10,12,14};

        int bitsOfWord=sizeof(WORD16)*8;

        WORD16 * bitmasks= new WORD16[bitsOfWord];

        for(int i=0; i<bitsOfWord;i++)
        {
            WORD16 one=1;
            bitmasks[i]=one << i;
        }

        WORD16 * srcBitsInWord= new WORD16[bitsOfWord];
        for(int i=0; i<bitsOfWord;i++)
        {
            srcBitsInWord[i]= bitmasks[i] & word_obj;
        }

        // here use bit_encrypt_index to find the original bit index(src_bit_index)
        //  and its value in WORD:srcBitsInWord[src_bit_index],
        //  then use right bit shift and left shift to get the encrypted value of this bit
        WORD16 * destBitsInWord= new WORD16[bitsOfWord];
        for(int i=0; i<bitsOfWord;i++)
        {
            int src_bit_index=bit_encrypt_index[i];
            destBitsInWord[i]=(srcBitsInWord[src_bit_index] >> src_bit_index) <<i;
        }

        delete [] srcBitsInWord;
        srcBitsInWord=NULL;

        // use bitwise OR to combine the value of all the bits value in WORD
        WORD16 dest=destBitsInWord[0];
        for(int i=1;i<bitsOfWord;i++)
        {
            dest= dest | destBitsInWord[i];
        }

        delete [] bitmasks;
        bitmasks=NULL;
        delete [] destBitsInWord;
        destBitsInWord=NULL;

        return dest;
    }


    void UAVROUTE_DATA_FRAME::SequentialXor(BYTE8* bytes,int length)
    {
        //ostringstream streamdebug;

//        if(length<3)
//        {
//            throw "SequentialXor length<3";
//        }

        bytes[0]= 0XEA ^ bytes[0];

        for(int i=1;i<=length-1;i++)
        {
            bytes[i]= bytes[i-1] ^ bytes[i];
        }

    }

    void UAVROUTE_DATA_FRAME::OutputBinary(std::ostream & out_stream, bool encrypt)
    {
        if(encrypt==true)
        {

            OutputBinaryV2(out_stream, true);
        }
        else
        {
//            ostringstream streamdebug;
//            streamdebug.str("");
//            streamdebug<< "start OutputBinary here! "<<std::endl;
//            qDebug(streamdebug.str().c_str());

            //check sum of the frame
            checksum = checksum_data_XOR();

            size_t size_frame= sizeof(frameHeader)+FRAME_DATA_LENGTH_IN_BYTE8;

            char * data_output= new char[size_frame];

            memcpy_s(data_output,
                     sizeof(frameHeader),
                     &frameHeader,
                     sizeof(frameHeader));
            memcpy_s(data_output+sizeof(frameHeader),
                     FRAME_DATA_LENGTH_IN_BYTE8-1,
                     data,
                     FRAME_DATA_LENGTH_IN_BYTE8-1);

            memcpy_s(data_output+sizeof(frameHeader)+FRAME_DATA_LENGTH_IN_BYTE8-1,
                     sizeof(checksum),
                     &checksum,
                     1);


            out_stream.write(data_output,size_frame);

            delete [] data_output;
            data_output= NULL;
        }


    }

    /// this is for output the frame as encrypted binary format
    /**
     *  @date 2013.11.09
     *
     */
    void UAVROUTE_DATA_FRAME::OutputBinaryV2(std::ostream & out_stream, bool encrypt)
    {
        BYTE8   data_for_encrypt[FRAME_DATA_LENGTH_IN_BYTE8];

        //check sum of the frame
        checksum = checksum_data_XOR();

        //copy data[], checksum to data_for_encrypt[]
        memcpy_s(data_for_encrypt,
                     FRAME_DATA_LENGTH_IN_BYTE8-1,
                     data,
                     FRAME_DATA_LENGTH_IN_BYTE8-1);

        memcpy_s(data_for_encrypt+FRAME_DATA_LENGTH_IN_BYTE8-1,
                     sizeof(checksum),
                     &checksum,
                     1);

        // encrypt 'data_for_encrypt[]' here
        if ( encrypt==true)
        {
//            ostringstream streamdebug;
//            streamdebug.str("");
//            streamdebug<< "encrypt 'data_for_encrypt[]' here! "<<std::endl;
//            qDebug(streamdebug.str().c_str());

            //process1, change the seq of bits in each WORD16 element
            WORD16* frame_data_word= (WORD16*)data_for_encrypt;
            for(int i=0; i<FRAME_DATA_LENGTH_IN_BYTE8/2;i++)
            {
                WORD16 curWord=frame_data_word[i];
                WORD16 encWorkd= encryptWORD(curWord);
                frame_data_word[i]=encWorkd;
            }
//            streamdebug.str("");
//            streamdebug<< "memcpy_s(data_for_encrypt"<<std::endl;
//            qDebug(streamdebug.str().c_str());

            memcpy_s(data_for_encrypt,
                         FRAME_DATA_LENGTH_IN_BYTE8,
                         frame_data_word,
                         FRAME_DATA_LENGTH_IN_BYTE8);

//            streamdebug.str("");
//            streamdebug<< "BEFORE SequentialXor"<<std::endl;
//            qDebug(streamdebug.str().c_str());

            //process2,sequential Xor in bytes
            SequentialXor(data_for_encrypt,FRAME_DATA_LENGTH_IN_BYTE8);

        }

        size_t size_frame= sizeof(frameHeader)+FRAME_DATA_LENGTH_IN_BYTE8;
        char * data_output= new char[size_frame];

        //copy frameHeader,data_for_encrypt[] to data_output[]
        memcpy_s(data_output,
                 sizeof(frameHeader),
                 &frameHeader,
                 sizeof(frameHeader));
        memcpy_s(data_output+sizeof(frameHeader),
                 FRAME_DATA_LENGTH_IN_BYTE8,
                 data_for_encrypt,
                 FRAME_DATA_LENGTH_IN_BYTE8);

        // output data_output[] to file
        out_stream.write(data_output,size_frame);

        delete [] data_output;
        data_output= NULL;

    }

    void UAVROUTE_HEADER::OutputBinary(std::ostream & out_stream) const
    {
//        ostringstream streamdebug;
//        streamdebug.str("");
//        streamdebug<< "UAVROUTE_HEADER::OutputBinary here! "<<std::endl;
//        qDebug(streamdebug.str().c_str());

        //------------
        //first frame
        //------------
        UAVRouteDataFrame frame_header_region;

        frame_header_region.data[0]=0x0A;

        UINT4 reserved = 0x0000;
        memcpy_s(frame_header_region.data+1,4,&reserved,4);

        UINT4 min_lat= CoordinatePack(min_latitude);
        memcpy_s(frame_header_region.data+5,4,&min_lat,4);

        UINT4 max_lat= CoordinatePack(max_latitude);
        memcpy_s(frame_header_region.data+9,4,&max_lat,4);

        UINT4 min_lon= CoordinatePack(min_longitude);
        memcpy_s(frame_header_region.data+13,4,&min_lon,4);

        UINT4 max_lon= CoordinatePack(max_longitude);
        memcpy_s(frame_header_region.data+17,4,&max_lon,4);

        frame_header_region.OutputBinary(out_stream);

        //------------
        //second frame
        //------------
        UAVRouteDataFrame frame_header_airport;
        frame_header_airport.data[0]=0x0B;

        //airport name
        std::string airport_upper(airport_name);
        std::transform(airport_name.begin(), airport_name.end(), airport_upper.begin(), toupper);
        char airport_name_array[10]={0x00,0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,0x00};
        for(int i=0;i<airport_upper.length();i++)
        {
            airport_name_array[10-1-i]=airport_upper[airport_upper.length()-i-1];
        }
        memcpy_s(frame_header_airport.data+1,10,airport_name_array,10);

        UINT4 airport_center_lat_4= CoordinatePack(airport_latitude);
        memcpy_s(frame_header_airport.data+11,4,&airport_center_lat_4,4);
        UINT4 airport_center_lon_4= CoordinatePack(airport_longitude);
        memcpy_s(frame_header_airport.data+15,4,&airport_center_lon_4,4);
        WORD16 height_2 = HeightPack(airport_height);
        memcpy_s(frame_header_airport.data+19,2,&height_2,2);

//        streamdebug.str("");
//        streamdebug<< "airport_name="<<airport_name<<std::endl;
//        qDebug(streamdebug.str().c_str());

        frame_header_airport.OutputBinary(out_stream);

    }

    Point2D UAVROUTE_FLIGHT_POINT::ToGomoPoint2D()const
    {
        Point2D pt2d;

        pt2d.X = this->__longitude;
        pt2d.Y = this->__latitude;

        return pt2d;

    }

    OGRPoint UAVROUTE_FLIGHT_POINT::ToOGRPoint() const
    {

        OGRPoint pt;
        pt.setX( __longitude );
        pt.setY( __latitude );
        pt.setZ( __height);

        return pt;
    }

    void UAVROUTE_FLIGHT_POINT::ToOGRFeature(OGRLayer *poLayer,OGRFeature ** ppOFeature) const
    {

        ostringstream streamid;
        streamid.str("");
        streamid<<__strip_id <<"."<< __id_in_strip<<std::endl;

        *ppOFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        (*ppOFeature)->SetField( "Id", streamid.str().c_str() );

        OGRPoint pt;
        pt.setX( __longitude );
        pt.setY( __latitude );
        pt.setZ(__height);

        (*ppOFeature)->SetGeometry( &pt );
    }

    void UAVROUTE_FLIGHT_POINT::Output(std::ostream & out_stream,bool encrypt) const
    {
        ostringstream stream_pointid;
        stream_pointid.str("");

        out_stream<<setiosflags(ios::fixed)<<setiosflags(ios::showpoint);
        out_stream.precision(COORDINATE_DEGREE_PRECISION);


        // output strip id
        unsigned int strip_id= __strip_id;
        ios state(nullptr);
        state.copyfmt(out_stream); // save current formatting
        out_stream << std::setw(2)
             << std::setfill('0')
             << strip_id<<"-";          // the actual value we wanted to print out
        out_stream.copyfmt(state); // restore previous formatting

        std::string pt_flag_A_B;

        if( __flight_point_type & FLIGTH_POINT_TYPE_A_POINT_MASK )
        {
            pt_flag_A_B = "0A";
        }
        else if(__flight_point_type & FLIGTH_POINT_TYPE_B_POINT_MASK)
        {
            pt_flag_A_B = "0B";
        }
        else
        {
            pt_flag_A_B="";
        }

        int pt_class;

         // output point id
        if( __flight_point_type == FLIGTH_POINT_TYPE_EXPOSURE)
        {
            state.copyfmt(out_stream); // save current formatting
            out_stream << std::setw(3)
                 << std::setfill('0')
                 << __id_in_strip;     // the actual value we wanted to print out
            out_stream.copyfmt(state); // restore previous formatting

            //out_stream<<__id_in_strip;
            pt_class= FLIGTH_POINT_TYPE_EXPOSURE;

            stream_pointid<<__id_in_strip;// for encrypt
        }
        else if ( __flight_point_type & FLIGTH_POINT_TYPE_GUIDE)
        {
            out_stream<<pt_flag_A_B<<FLIGTH_POINT_TYPE_GUIDE ;
            pt_class= FLIGTH_POINT_TYPE_GUIDE;

            stream_pointid <<pt_flag_A_B<<FLIGTH_POINT_TYPE_GUIDE ; // for encrypt
        }
        else if ( __flight_point_type & FLIGTH_POINT_TYPE_ETRANCE_EXIT)
        {
            out_stream<<pt_flag_A_B<<FLIGTH_POINT_TYPE_ETRANCE_EXIT ;
            pt_class= FLIGTH_POINT_TYPE_ETRANCE_EXIT;

            stream_pointid <<pt_flag_A_B<<FLIGTH_POINT_TYPE_ETRANCE_EXIT ; // for encrypt
        }

        int height_int = __height +0.5;

        out_stream.precision(COORDINATE_DEGREE_PRECISION);

        if(encrypt==false)
        {
            out_stream<<SPACE<<__latitude<<SPACE;

            //output the height and pt_class
            state.copyfmt(out_stream); // save current formatting
            out_stream << std::setw(COORDINATE_DEGREE_PRECISION+4)
                      << std::setfill('0')
                      <<__longitude;
            out_stream.copyfmt(state); // restore previous formatting
        }
        else
        {
            CoordinateOutput coordoutput;

            coordoutput.SetPointIdForEncryptionStep3A(stream_pointid.str());
            coordoutput.SetAsDouble(__latitude,__longitude);

            out_stream<<SPACE<<coordoutput.GetAsEncryptString();
        }


        //output the height and pt_class
        state.copyfmt(out_stream); // save current formatting
        out_stream <<SPACE
                  << std::setw(4)
                  << std::setfill('0')
                  <<height_int;
        out_stream.copyfmt(state); // restore previous formatting

        //point class
        out_stream <<SPACE<<pt_class<<"\n";


    }



    void UAVROUTE_FLIGHT_POINT::OutputBinary(std::ostream & out_stream) const
    {

        UAVRouteDataFrame frame_point;

        WORD16 point_id_16;
        BYTE8  pt_class;

        if( __flight_point_type & FLIGTH_POINT_TYPE_A_POINT_MASK )
        {

            if(__flight_point_type & FLIGTH_POINT_TYPE_GUIDE)
            {
                //A1
                point_id_16=0xA11F;
                pt_class =FLIGTH_POINT_TYPE_GUIDE;
            }
            else if ( __flight_point_type & FLIGTH_POINT_TYPE_ETRANCE_EXIT)
            {
                //A2
                point_id_16=0xA21F;
                pt_class= FLIGTH_POINT_TYPE_ETRANCE_EXIT;
            }
        }
        else if(__flight_point_type & FLIGTH_POINT_TYPE_B_POINT_MASK)
        {
            if(__flight_point_type & FLIGTH_POINT_TYPE_GUIDE)
            {
                //B1
                point_id_16=0xB11F;
                pt_class =FLIGTH_POINT_TYPE_GUIDE;
            }
            else if ( __flight_point_type & FLIGTH_POINT_TYPE_ETRANCE_EXIT)
            {
                //B2
                point_id_16=0xB21F;
                pt_class= FLIGTH_POINT_TYPE_ETRANCE_EXIT;
            }
        }
        else if (__flight_point_type == FLIGTH_POINT_TYPE_EXPOSURE)
        {
            WORD16 id_litte=__id_in_strip;
            WORD16 id_litte_pack = (id_litte & 0x0FFF) + 0x1000;
            point_id_16 = SHORT_little_endian_TO_big_endian(id_litte_pack);
            pt_class= FLIGTH_POINT_TYPE_EXPOSURE;
        }

        //16 bits point id for the first two bytes
        memcpy_s(frame_point.data,2,&point_id_16,2);

        char reserved[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        memcpy_s(frame_point.data+2,7,reserved,7);

        BYTE8 strip_id_packed = __strip_id;
        memcpy_s(frame_point.data+9,1,&strip_id_packed,1);

        UINT4 lat_pack = CoordinatePack(__latitude);
        UINT4 lon_pack = CoordinatePack(__longitude);
        WORD16 h_pack  = HeightPack(__height);

        memcpy_s(frame_point.data+10,4,&lat_pack,4);
        memcpy_s(frame_point.data+14,4,&lon_pack,4);
        memcpy_s(frame_point.data+18,2,&h_pack,2);

        memcpy_s(frame_point.data+20,1,&pt_class,1);

        frame_point.OutputBinary(out_stream);

    }


    void UAVFLIGHT_STATISTIC_INFO::OutputBinary(std::ostream & out_stream) const
    {

        UAVRouteDataFrame frame_statis;
        frame_statis.data[0]=0x20;

        BYTE8 reserved =0;
        frame_statis.data[1]=reserved;

        ULong64 mbr_area = __MBR_Area*1e3;
        ULong64 area_region= __flight_region_area*1e3;
        WORD16 count_expos = __count_exposures;
        BYTE8  count_strip = __count_strips;

        BYTE8 mbr6[6];
        BYTE8 area6[6];
        ULong64Pack(mbr_area,mbr6,6);
        ULong64Pack(area_region,area6,6);

        if ( __photo_flight_course_chainage*1e3> 4294967295)
        {
            throw "__photo_flight_course_chainage overflow 4 byte";
        }
        UINT4 sum_lengh = DoublePackUnsigned(__photo_flight_course_chainage*1e3);

        memcpy_s(frame_statis.data+2,6,mbr6,6);
        memcpy_s(frame_statis.data+8,6,area6,6);
        memcpy_s(frame_statis.data+14,2,&count_expos,2);
        memcpy_s(frame_statis.data+16,1,&count_strip,1);
        memcpy_s(frame_statis.data+17,4,&sum_lengh,4);

        frame_statis.OutputBinary(out_stream);

    }



    void UAVRouteStrip::Clear()
    {
        __flight_point.clear();
        __count_exposures=0;
    }

    // add point to strip and update the max id of the strip
    void UAVRouteStrip::AddPoint(const UAVFlightPoint & pt)
    {
        __flight_point.push_back(pt);

        __count_exposures= __count_exposures>pt.__id_in_strip ? __count_exposures:pt.__id_in_strip;
    }

    UAV_FLIGHT_STRIP& UAVRouteStrip::operator =(const UAV_FLIGHT_STRIP& rhs)
    {
        this->Clear();
        this->__count_exposures=rhs.__count_exposures;

        for(std::vector<UAVFlightPoint>::const_iterator it=rhs.__flight_point.begin();
            it!=rhs.__flight_point.end();
            it++)
        {
           this-> __flight_point.push_back(*it);

        }
        return *this;
    }


    UAVFLIGHT_ROUTE_DESIGN& UAVFLIGHT_ROUTE_DESIGN::
            operator=(const UAVFLIGHT_ROUTE_DESIGN & rs)
    {

        //1.UAVROUTE_HEADER
        __header.airport_height     = rs.__header.airport_height;
        __header.airport_latitude   = rs.__header.airport_latitude;
        __header.airport_longitude  = rs.__header.airport_longitude;
        __header.airport_name       = rs.__header.airport_name;

        __header.max_latitude       = rs.__header.max_latitude;
        __header.max_longitude      = rs.__header.max_longitude;
        __header.min_latitude       = rs.__header.min_latitude;
        __header.min_longitude      = rs.__header.min_longitude;


        //2. std::vector< UAVFlightPoint > __flight_point;
        __flight_point = rs.__flight_point;

        //3. UAVFlightStatisticInfo __flight_statistic;
        __flight_statistic.__count_exposures    = rs.__flight_statistic.__count_exposures;
        __flight_statistic.__count_strips       = rs.__flight_statistic.__count_strips;
        __flight_statistic.__flight_region_area = rs.__flight_statistic.__flight_region_area;
        __flight_statistic.__MBR_Area           = rs.__flight_statistic.__MBR_Area;
        __flight_statistic.__photo_flight_course_chainage = rs.__flight_statistic.__photo_flight_course_chainage;

        return *this;

    }


    UAVFLIGHT_ROUTE_DESIGN& UAVFLIGHT_ROUTE_DESIGN::
            operator+=(const UAVFLIGHT_ROUTE_DESIGN & rs)
    {
        //1.expand the bounding box of UAVROUTE_HEADER
        __header.max_latitude       = (rs.__header.max_latitude     > __header.max_latitude)    ? rs.__header.max_latitude  : __header.max_latitude     ;
        __header.max_longitude      = (rs.__header.max_longitude    > __header.max_longitude)   ? rs.__header.max_longitude : __header.max_longitude    ;
        __header.min_latitude       = (rs.__header.min_latitude     < __header.min_latitude )   ? rs.__header.min_latitude  : __header.min_latitude     ;
        __header.min_longitude      = (rs.__header.min_longitude    < __header.min_longitude)   ? rs.__header.min_longitude : __header.min_longitude    ;

        //2. std::vector< UAVFlightPoint > __flight_point;
        // first update the strip id of the second flight region
        std::vector<UAVFlightPoint>::const_iterator it_rs_point = rs.__flight_point.begin();

        std::vector<UAVFlightPoint> second_region_points;
        for ( ;it_rs_point!= rs.__flight_point.end();it_rs_point++)
        {
            UAVFlightPoint pt = *it_rs_point;
            pt.__strip_id +=  __flight_statistic.__count_strips;

            second_region_points.push_back(pt);
        }

        __flight_point.insert(__flight_point.end(),second_region_points.begin(),second_region_points.end());

        //3. expand the UAVFlightStatisticInfo __flight_statistic;
        __flight_statistic.__count_exposures    += rs.__flight_statistic.__count_exposures;
        __flight_statistic.__count_strips       += rs.__flight_statistic.__count_strips;
        __flight_statistic.__flight_region_area += rs.__flight_statistic.__flight_region_area;
        __flight_statistic.__MBR_Area           += rs.__flight_statistic.__MBR_Area;
        __flight_statistic.__photo_flight_course_chainage += rs.__flight_statistic.__photo_flight_course_chainage;

        return *this;


    }

}

}
