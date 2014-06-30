#include "uavrouteoutputer.h"

#include <fstream>
using std::ofstream;




UAVRouteOutputer::UAVRouteOutputer()
{
}


    void UAVRouteOutputer::OutputRouteDesignFileAsText(const UAVRouteDesign & route_design
                                                       ,const std::string & output_file)
    {

        try
        {
            std::ofstream output_route_file;
            output_route_file.open(output_file);

            //output_route_file<< route_design.__header.ToStdString();
            route_design.__header.Output(output_route_file);

            std::vector< UAVFlightPoint >::const_iterator it_pt= route_design.__flight_point.begin();

            for( ; it_pt!= route_design.__flight_point.end(); it_pt++ )
            {
                (*it_pt).Output(output_route_file);
            }

            output_route_file<<"END"<<"\n";

            route_design.__flight_statistic.Output(output_route_file);

            output_route_file.close();
        }
        catch(std::string& e)
        {
            throw e+"Exception in PolygonAreaFlightRouteDesign::OutputRouteFile() ";
        }

    }

    void UAVRouteOutputer::OutputRouteDesignFileAsBinary(const UAVRouteDesign & route_design
                                                         ,const std::string & output_file)
    {
        try
        {
            ostringstream streamdebug;
            streamdebug.str("");
            streamdebug<< "UAVRouteOutputer::OutputRouteDesignFileAsBinary here! "<<std::endl;
            qDebug(streamdebug.str().c_str());

            std::ofstream output_route_file;
            output_route_file.open(output_file,ios::binary | ios::out);

            route_design.__header.OutputBinary(output_route_file);

            std::vector< UAVFlightPoint >::const_iterator it_pt= route_design.__flight_point.begin();

            for( ; it_pt!= route_design.__flight_point.end(); it_pt++ )
            {
                (*it_pt).OutputBinary(output_route_file);
            }

            route_design.__flight_statistic.OutputBinary(output_route_file);

            output_route_file.close();
        }
        catch(std::string& e)
        {
            throw e+"Exception in PolygonAreaFlightRouteDesign::OutputRouteFile() ";
        }


    }


    void UAVRouteOutputer::OutputRouteDesignFileAsTextEncrypted(const UAVRouteDesign & route_design
                                            ,const std::string & output_file)
    {
        try
        {
            std::ofstream output_route_file;
            output_route_file.open(output_file);

            route_design.__header.Output(output_route_file,true);

            std::vector< UAVFlightPoint >::const_iterator it_pt= route_design.__flight_point.begin();

            for( ; it_pt!= route_design.__flight_point.end(); it_pt++ )
            {
                (*it_pt).Output(output_route_file,true);
            }

            output_route_file<<"END"<<"\n";

            route_design.__flight_statistic.Output(output_route_file);

            output_route_file.close();
        }
        catch(std::string& e)
        {
            throw e+"Exception in UAVRouteOutputer::OutputRouteDesignFileAsTextEncrypted ";
        }


    }

    void UAVRouteOutputer::OutputRouteDesignFileAsKML(const UAVRouteDesign & route_design
                                              ,const std::string & output_file )
    {
        ostringstream streamdebug;
        streamdebug.str("");
        streamdebug<< "UAVRouteOutputer::OutputRouteDesignFileAsKML here! "<<std::endl;
        qDebug(streamdebug.str().c_str());

        const char *pszDriverName = "KML";
        OGRSFDriver *poDriver;

        OGRRegisterAll();

        poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
                    pszDriverName );
        if( poDriver == NULL )
        {
            return;
        }
        OGRDataSource *poDS;

        poDS = poDriver->CreateDataSource( output_file.c_str(), NULL );
        if( poDS == NULL )
        {
            return;
        }

        OGRLayer *poFlightPtLayer;
        poFlightPtLayer = poDS->CreateLayer( "flight_points", NULL, wkbPoint25D, NULL );

        OGRFieldDefn oField( "Id", OFTString );
        oField.SetWidth(10);

        if( poFlightPtLayer->CreateField( &oField ) != OGRERR_NONE )
        {
            return;
        }

        OGRLineString striplines;

        // 1. iterate the flight points and create the flight point layer
        std::vector< UAVFlightPoint >::const_iterator it_pt= route_design.__flight_point.begin();

        for( ; it_pt!= route_design.__flight_point.end(); it_pt++ )
        {
            OGRFeature * pOFeature;
            (*it_pt).ToOGRFeature(poFlightPtLayer,&pOFeature);

            if( poFlightPtLayer->CreateFeature( pOFeature ) != OGRERR_NONE )
            {
               return;
            }

            OGRFeature::DestroyFeature( pOFeature );

            OGRPoint ptLine= (*it_pt).ToOGRPoint();
            striplines.addPoint(&ptLine);
        }

        // 2. create the strip feature
        OGRLayer *poFlightStripLayer;
        poFlightStripLayer = poDS->CreateLayer( "flight_points", NULL, wkbLineString25D, NULL );

        if( poFlightStripLayer->CreateField( &oField ) != OGRERR_NONE )
        {
            return;
        }

        OGRFeature * pStripFeature;

        pStripFeature = OGRFeature::CreateFeature( poFlightStripLayer->GetLayerDefn() );
        pStripFeature->SetField( "Id", "1" );

        pStripFeature->SetGeometry( &striplines );
        if( poFlightStripLayer->CreateFeature( pStripFeature ) != OGRERR_NONE )
        {
           return;
        }
        OGRFeature::DestroyFeature( pStripFeature );


        OGRDataSource::DestroyDataSource( poDS );

    }


