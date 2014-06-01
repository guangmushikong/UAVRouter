#ifndef GEOMERTYCONVERTOR_H
#define GEOMERTYCONVERTOR_H

#include "GomoGeometry2D.h"
using namespace Gomo::Geometry2D;


#include <ogrsf_frmts.h>

namespace Gomo {

    namespace Geometry2D {

        class GeomertyConvertor
        {
        public:
            GeomertyConvertor();
        public:
            static void OGRGeomery2Point2DArray(const OGRGeometry *,  Point2DArray& points2d);
            static void OGRPoint2Point2D(const OGRPoint &, Point2D & pt);

        };
    }

}


#endif // GEOMERTYCONVERTOR_H
