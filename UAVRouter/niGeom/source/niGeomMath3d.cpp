//! \file
// \brief
// Geometry math 3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#include <niGeom/geometry/niGeomMath3d.h>
#include <niGeom/geometry/niBBox3d.h>
#include <niGeom/math/niVector3d.h>
#include <niGeom/geometry/niLine3d.h>
#include <niGeom/geometry/niPolygon3d.h>
#include <niGeom/geometry/niTriangle3d.h>


namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION IsBoxOverlapBox
        //-----------------------------------------------------------------------------
        /**
        * Is the two bbox overlap each other
        *
        * @param       bbox1:       box
        * @param       bbox2:       box
        * @return      if overlap to each other, return true, or return false
        * 
        */
        bool niGeomMath3d::IsBoxOverlapBox(
            const niBBox3d &bbox1,
            const niBBox3d &bbox2)
        {
            if (bbox1.P2.X < bbox2.P1.X)
                return false;
            if (bbox1.P2.Y < bbox2.P1.Y)
                return false;
            if (bbox1.P2.Z < bbox2.P1.Z)
                return false;

            if (bbox1.P1.X > bbox2.P2.X)
                return false;
            if (bbox1.P1.Y > bbox2.P2.Y)
                return false;
            if (bbox1.P1.Z > bbox2.P2.Z)
                return false;

            return false;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInBox
        //-----------------------------------------------------------------------------
        /**
        * Is a point in bbox
        *
        * @param       p:           the corner point
        * @param       bbox:        box
        * @return      if in, return true, or return false
        */

        bool niGeomMath3d::IsPointInBox(
            const niVert3d &p,
            const niBBox3d &bbox)
        {
            if (p.X < bbox.P1.X)
                return false;
            if (p.X > bbox.P2.X)
                return false;
            if (p.Y < bbox.P1.Y)
                return false;
            if (p.Y > bbox.P2.Y)
                return false;
            if (p.Z < bbox.P1.Z)
                return false;
            if (p.Z > bbox.P2.Z)
                return false;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInTriangle
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a triangle
        *
        * @param       p:           point
        * @param       tri:         triangle
        * @return      eOutGeometry:    out of the triangle
        *              eOnBorder:       on the border of the triangle
        *              eOnPoint:        on one point of the triangle
        *              eInGeometry:     in the triangle
        *              eErrorGeometry:  the triangle is error
        */
        EPointInGeometry niGeomMath3d::IsPointInTriangle(
            const niVert3d &p,
            const niTriangle3d &tri)
        {
            return IsPointInTriangle(p, tri.P0, tri.P1, tri.P2);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInTriangle
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a triangle
        *
        * @param       p:           point
        * @param       a:           point of triangle
        * @param       b:           point of triangle
        * @param       c:           point of triangle
        * @return      eOutGeometry:    out of the triangle
        *              eOnBorder:       on the border of the triangle
        *              eOnPoint:        on one point of the triangle
        *              eInGeometry:     in the triangle
        *              eErrorGeometry:  the triangle is error
        */
        EPointInGeometry niGeomMath3d::IsPointInTriangle(
            const niVert3d &p,
            const niVert3d &a,
            const niVert3d &b,
            const niVert3d &c)
        {
            niVert3d pa(a - p);
            niVert3d pb(b - p);
            niVert3d pc(c - p);
            niVert3d cross1 = pa.Cross(pb);
            niVert3d cross2 = pb.Cross(pc);
            if (cross1.Dot(cross2) < -ZEROF)
                return eOutGeometry;
            niVert3d cross3 = pc.Cross(pa);
            if (cross1.Dot(cross3) < -ZEROF)
                return eOutGeometry;
            if (cross3.Dot(cross2) < -ZEROF)
                return eOutGeometry;

            double area = (b-a).Cross(c-a).Length();

            double s1 = cross1.Length();
            double s2 = cross2.Length();
            double s3 = cross3.Length();
            double a2 = s1 + s2 + s3;
            if (0 != niMath::_compare_(area, a2))
                return eOutGeometry;

            int fInGeometry = 1;
            if (0 == s1)
                fInGeometry = fInGeometry << 1;
            if (0 == s2)
                fInGeometry = fInGeometry << 1;
            if (0 == s3)
                fInGeometry = fInGeometry << 1;

            return EPointInGeometry(fInGeometry);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInTriangle
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a triangle
        *
        * @param       p:           point
        * @param       a:           point of triangle
        * @param       b:           point of triangle
        * @param       c:           point of triangle
        * @param       area:        area of triangle
        * @return      eOutGeometry:    out of the triangle
        *              eOnBorder:       on the border of the triangle
        *              eOnPoint:        on one point of the triangle
        *              eInGeometry:     in the triangle
        *              eErrorGeometry:  the triangle is error
        */
        EPointInGeometry niGeomMath3d::IsPointInTriangle(
            const niVert3d &p,
            const niVert3d &a,
            const niVert3d &b,
            const niVert3d &c,
            double area)
        {
            niVert3d pa(a - p);
            niVert3d pb(b - p);
            niVert3d pc(c - p);
            niVert3d cross1 = pa.Cross(pb);
            niVert3d cross2 = pb.Cross(pc);
            double c12 = cross1.Dot(cross2);
            if (c12 < -ZEROF)
                return eOutGeometry;
            niVert3d cross3 = pc.Cross(pa);
            double c23 = cross2.Dot(cross3);
            if (c12 < -ZEROF)
                return eOutGeometry;
            double c31 = cross3.Dot(cross1);
            if (c12 < -ZEROF)
                return eOutGeometry;
            if (c12 >= ZEROF && c23 >= ZEROF && c31 >= ZEROF)
                return eInGeometry;

            double s1 = cross1.Length();
            double s2 = cross2.Length();
            double s3 = cross3.Length();
            double a2 = s1 + s2 + s3;
            if (0 != niMath::_compare_(area, a2))
                return eOutGeometry;

            int fInGeometry = 1;
            if (0 == s1)
                fInGeometry = fInGeometry << 1;
            if (0 == s2)
                fInGeometry = fInGeometry << 1;
            if (0 == s3)
                fInGeometry = fInGeometry << 1;

            return EPointInGeometry(fInGeometry);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointOnLine
        //-----------------------------------------------------------------------------
        /**
        * Is a point on a line
        *
        * @param       p:           point
        * @param       p0:          endpoint of line
        * @param       p1:          endpoint of line
        * @return      true:        on line
        *              false:       not on line
        */
        bool niGeomMath3d::IsPointOnLine(
            const niVert3d &p,
            const niVert3d &p0,
            const niVert3d &p1)
        {
            if (p == p0 || p == p1)
                return true;
            double l1 = p0.DistanceTo(p1);
            double l2 = p.DistanceTo(p0) + p.DistanceTo(p1);
            return 0 == niMath::_compare_(l1, l2);
        }
    }
}