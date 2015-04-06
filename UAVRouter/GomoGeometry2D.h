#ifndef GOMOGEOMETRY2D_H
#define GOMOGEOMETRY2D_H

#define GETVARIABLESTR(a) (#a)

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "niGeom/niGeom/math/niVector2d.h"
#include "./niGeom/geometry/niPolygon2d.h"
#include "./niGeom/geometry/niGeom2dTypes.h"
#include "./niGeom/geometry/niPolygon2dFn.h"
#include "./niGeom/geometry/niGeomMath2d.h"
#include "./niGeom/niGeom/niTypes.h"

#include <sstream>
using std::ostringstream;

namespace Gomo {

namespace Geometry2D {

    typedef niVector2d Point2D;

    //typedef vector<Point2D> Point2DArray;

    typedef ni::geometry::niPoint2dArray Point2DArray;

	/**
	* Calc the angle of line(ptS->ptE) to X axis
	* @param       ptS:          Start point of the line
	* @param       ptE:          End point of the line
	* @return      the angle of the line to X axis
	*             ptE
    *            /
    *           /
    *         ptS-----> X axis
	*/
	inline double LineOrientation(const Point2D & ptS,const Point2D & ptE)
	{

		Point2D ptX= ptS;

		ptX.X = ptS.X+ 1.0e15;

		return ni::geometry::niGeomMath2d::Angle( ptS,ptX,ptE);

		/**
        * calc the angle of a corner
        *
        * @param       p0:          the corner point
        * @param       p1:          endpoint of the first border
        * @param       p2:          endpoint of the second border
        * @return      the angle of the corner
        *             p2
        *            /
        *           /
        *         p0-----p1
        * 
        *
        double niGeomMath2d::Angle(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &p2)
			*/



	};

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


    inline bool CalculatePolygonOrientaion2D(const Point2DArray&  polygon_2d,Point2D& center,double& angle)
    {
            size_t numPoints = polygon_2d.size();

            if (numPoints < 1)
            {
                angle = 0.0;
                return false;
            }

            center = polygon_2d[0];

            for (size_t i = 1; i < numPoints; ++i)
            {
                center += polygon_2d[i];
            }

            center /= double(numPoints);

            double varX = 0.0, varY = 0.0, covXY = 0.0;

            for (size_t i = 0; i < numPoints; ++i)
            {
                Point2D pnt = polygon_2d[i] - center;//cetralize
                varX    += pnt.X * pnt.X;
                varY    += pnt.Y * pnt.Y;
                covXY   += pnt.X * pnt.Y;
            }

            if (varX < ZEROF || varY < ZEROF)
            {
                angle = 0.0;
                return true;
            }

            if (abs(varY - varX) < 0.0001)
                angle = 45.0 * _PI_ / 180.0;
            else
            {
                angle = atan(2.0 * covXY / (varY - varX)) / 2.0;

                if (covXY > 0.0)
                {
                    if (angle < 0.0)
                        angle = angle + _PI_ / 2.0;

                    angle -= _PI_ / 4.0;//added by wangmiao
                }

                if (covXY < 0.0)
                {
                    if (angle < 0.0)
                        angle = angle + _PI_;
                    else
                        angle = angle + _PI_ / 2.0;

                    angle += _PI_ / 4.0 ;//added by wangmiao
                }



            }

            // make sure the angle be smaller than 90 degree
            if( angle> _PI_ / 2.0 )
            {
                angle -= _PI_ ;
            }

        return true;
    }

    class PolygonOrientation2D
    {
        // candidate methodologies
        enum enmOrientationModel
        {
            LINE_APPROXIMATION=0,
            CONVEXHULL=1,
            SKELETON=2
        };

        struct OptimalOrientationInfo
        {
            double __angle;
            double __rotated_mbr_area;
            enmOrientationModel __model;

        public:

            inline void Output(std::ostream & out_stream) const
            {
                std::map<enmOrientationModel,std::string> model_string;
                model_string[LINE_APPROXIMATION]=GETVARIABLESTR(LINE_APPROXIMATION);
                model_string[CONVEXHULL]=GETVARIABLESTR(CONVEXHULL);
                model_string[SKELETON]=GETVARIABLESTR(SKELETON);

                out_stream<<"Orientation Angle (in rad): "<<__angle<<"\n";
                out_stream<<"MBR Area after roation(m2):"<< __rotated_mbr_area <<"\n";
                out_stream<<"OrientationModel:"<<model_string[__model]<<"\n";
            };
        };

    public:
        PolygonOrientation2D(const Point2DArray pts);
        ~PolygonOrientation2D();

        Point2D GetCenter();
        bool GetOptimalOrientation(double & optimal_angle);

        void SetExtendBaseLineLength(const double & baseline)
        {
            m_extended_baseline = baseline;
        };

    protected:
        bool Centralization();
        bool GetOrientation_ConvexHull(OptimalOrientationInfo& info);
        bool GetOrientation_Skeleton(OptimalOrientationInfo& info);
        bool GetOrientation_LineApprox( OptimalOrientationInfo& info);

        bool PolygonRoationExperiment(const double & angle,
                                      double& mbr_area);

    protected:
        Point2D m_center;
        double m_orientation_angle;

        double m_orignal_area;

        Point2DArray m_polygon_pts;
        Point2DArray m_polygon_centralized;

        std::map<double /* difference of __rotated_mbr_area and m_orignal_area */,OptimalOrientationInfo> m_model_orienation;

        double m_extended_baseline;

    };


}





}


#endif // GOMOGEOMETRY2D_H
