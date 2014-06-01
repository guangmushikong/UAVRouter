#include "geomertyconvertor.h"

#include <sstream>
using std::ostringstream;
#include <QDebug>

namespace Gomo {

    namespace Geometry2D {

        GeomertyConvertor::GeomertyConvertor()
        {

        }

        void GeomertyConvertor::OGRGeomery2Point2DArray(const OGRGeometry * geomPtr,  Point2DArray& points2d)
        {
            points2d.clear();
            ostringstream streamdebug;
            streamdebug.str("");
            streamdebug<< "OGRGeomery2Point2DArray: "<<std::endl;


            if ( wkbFlatten(geomPtr->getGeometryType())==wkbPolygon )
            {
                OGRPolygon* pPolygon=dynamic_cast<OGRPolygon*>(geomPtr->clone());

                int ptcount=pPolygon->getExteriorRing()->getNumPoints();

                for(int i=0;i<ptcount;i++)
                {
                    Point2D pt;
                    pt.X=pPolygon->getExteriorRing()->getX(i);
                    pt.Y=pPolygon->getExteriorRing()->getY(i);
                    points2d.push_back(pt);
                    streamdebug<< pt.X<<","<<pt.Y<<std::endl;
                }

            }
            else if ( wkbFlatten(geomPtr->getGeometryType())==wkbLineString )
            {
                OGRLineString* pLineString=dynamic_cast<OGRLineString*>(geomPtr->clone());

                int ptcount=pLineString->getNumPoints();

                for(int i=0;i<ptcount;i++)
                {
                    Point2D pt;
                    pt.X=pLineString->getX(i);
                    pt.Y=pLineString->getY(i);
                    points2d.push_back(pt);
                    streamdebug<< pt.X<<","<<pt.Y<<std::endl;
                }

            }
            else
            {
                throw "not support geometry type in OGRGeomery2Point2DArray";
            }

            qDebug(streamdebug.str().c_str());

        }


        void GeomertyConvertor::OGRPoint2Point2D(const OGRPoint & ogrPoint, Point2D & pt)
        {
            pt.X=ogrPoint.getX();
            pt.Y=ogrPoint.getY();

        }



    }


}
