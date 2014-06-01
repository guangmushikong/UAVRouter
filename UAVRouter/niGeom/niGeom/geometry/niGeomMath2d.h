//! \file
// \brief
// Geometry math libaray
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niGeomMath2d_H
#define niGeomMath2d_H

#include <niGeom/geometry/niGeomDef.h>
#include <niGeom/geometry/niGeom2dTypes.h>
#include <niGeom/math/niMatrix22.h>

namespace ni
{
    namespace geometry
    {
        class niBBox2d;
        class niBvhNode;
        class niLine2d;
        class niTriangle2d;
        class niPolygon2d;

        /**
         * \brief 2D Geometry math libaray
         *
         */
        class niGeomMath2d
        {
        public:
            static double               Angle(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niPoint2d &p2);

            static double               Angle(
                const niPoint2d &d0,
                const niPoint2d &d1);

            static void                 AvoidAbnormalData(
                const int num_sign_digital,
                const niBBox2d &bbox,
                niPoint2dArray &points);

            static void                 AvoidAbnormalData(
                const double _SCALE_,
                niPoint2d center,
                niPoint2dArray &points);

            static double               Area(
                const niPoint2dArray &points);

            static bool                 Center(
                const niPoint2dArray &points,
                niPoint2d &center);

            static bool                 CalcConvexHull(
                const niPoint2dArray &points,
                niIntArray &convexHullIds);

            static bool                 CalcPointsOBB(
                const niPoint2dArray &points,
                niPoint2d &center,
                double &angle);

            static bool                 CalcPointsOBB(
                const niPoint2dArray &points,
                niPoint2d &center,
                niMatrix22 &rot);

            static double               DiameterOfBBox2BBox(
                const niBBox2d &bbox1,
                const niBBox2d &bbox2);

            static double               Distance(
                const double& X1,
                const double& Y1,
                const double& X2,
                const double& Y2);

            static double               DistanceOfBBox2BBox(
                const niBBox2d &bbox1,
                const niBBox2d &bbox2);

            static double               DistanceOfPoint2BBox(
                const niPoint2d &p,
                const niBBox2d &bbox);

            static double               HeigthOfParallelLines(
                const double &X1, const double &Y1,
                const double &X2, const double &Y2,
                const double &X3, const double &Y3,
                const double &X4, const double &Y4);

            static double               HeigthOfParallelLines(
                const niPoint2d &P1,
                const niPoint2d &P2,
                const niPoint2d &P3,
                const niPoint2d &P4);

            static double               HeightOfPoint2Line(
                const niPoint2d &P,
                const niPoint2d &P1,
                const niPoint2d &P2);

            static bool                 IsBoxOverlapBox(
                const niBBox2d &bbox1,
                const niBBox2d &bbox2);

            static bool                 IsLineOverlapBox(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niBBox2d &bbox);

            static bool                 IsLineISectBox(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niBBox2d &bbox);

            static bool                 IsPointBetweenAngle(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niPoint2d &p2,
                const niPoint2d &p);

            static bool                 IsPointInBox(
                const niPoint2d &p,
                const niBBox2d &bbox);

            static EPointInGeometry     IsPointInPolygon(
                const niPoint2d &p,
                const niPolygon2d &polygon);

            static EPointInGeometry     IsPointInTriangle(
                const niPoint2d &p,
                const niTriangle2d &tri);

            static EPointInGeometry     IsPointInTriangle(
                const niPoint2d &p,
                const niPoint2d &a,
                const niPoint2d &b,
                const niPoint2d &c);

            static EPointInGeometry     IsPointInTriangle(
                const niPoint2d &p,
                const niPoint2d &a,
                const niPoint2d &b,
                const niPoint2d &c,
                double area);

            static bool                 IsPointOnLine(
                const niPoint2d &p,
                const niPoint2d &p0,
                const niPoint2d &p1);

            static bool                 IsXRayISectBox(
                const niPoint2d &p,
                const niBBox2d &bbox);

            static bool                 IsYRayISectBox(
                const niPoint2d &p,
                const niBBox2d &bbox);

            static EIntersectType       IsXRayISectLine(
                const niPoint2d &p,
                const niPoint2d &p0,
                const niPoint2d &p1);

            static EIntersectType       IsYRayISectLine(
                const niPoint2d &p,
                const niPoint2d &p0,
                const niPoint2d &p1);

            static bool                 LineOverlapLine(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niPoint2d &q0,
                const niPoint2d &q1);

            static bool                 LineOverlapLine(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niPoint2d &q0,
                const niPoint2d &q1,
                double &minHeight);

            static bool                 LineISectLine(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niPoint2d &q0,
                const niPoint2d &q1,
                niPoint2d &isec);

            static EOrientation         Orientation(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const niPoint2d &p2);

            static EOrientation         Orientation(
                const niPoint2dArray &points);
        };
    }
}

#endif