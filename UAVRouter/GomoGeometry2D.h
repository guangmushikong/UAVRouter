#ifndef GOMOGEOMETRY2D_H
#define GOMOGEOMETRY2D_H

#include <vector>
using std::vector;

#include "niGeom/niGeom/math/niVector2d.h"

namespace Gomo {

namespace Geometry2D {

    typedef niVector2d Point2D;

    typedef vector<Point2D> Point2DArray;

    // 2D rotate, in anticlockwise
    inline Point2D Rotate2D(const Point2D & pt,double angle_to_x)
    {
        Point2D ptRotated;

        ptRotated.X = pt.X*cos(angle_to_x)- pt.Y*sin(angle_to_x);

        ptRotated.Y = pt.X*sin(angle_to_x)+ pt.Y*cos(angle_to_x);

        return ptRotated;

    };

    inline void MBR2D(const Point2DArray & pt_array, Point2D& left_top, Point2D& right_bottom)
    {
        double minX = pt_array[0].X;
        double maxX = pt_array[0].X;
        double minY = pt_array[0].Y;
        double maxY = pt_array[0].Y;

        Point2DArray::const_iterator it= pt_array.begin()+1;

        for( ; it!= pt_array.end();it++ )
        {
            minX = (minX > it->X) ? it->X : minX;
            maxX = (maxX < it->X) ? it->X : maxX;
            minY = (minY > it->Y) ? it->Y : minY;
            maxY = (maxY < it->Y) ? it->Y : maxY;
        }

        left_top.X = minX;
        left_top.Y = maxY;
        right_bottom.X = maxX;
        right_bottom.Y = minY;
    };

}





}


#endif // GOMOGEOMETRY2D_H
