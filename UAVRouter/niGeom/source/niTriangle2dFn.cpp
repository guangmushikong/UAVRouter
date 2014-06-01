//! \file
// \brief
// Operator to handle triangle  class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niTriangle2dFn.h>
#include <niGeom/geometry/niTriangle2d.h>

#include <niGeom/geometry/niGeomMath2d.h>


#include <algorithm>

namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION Orientation
        //-----------------------------------------------------------------------------
        /**
        * Calc orientation of the triangle
        *
        * return        EOrientation
        */
        EOrientation niTriangle2dFn::Orientation() const
        {
            if (!m_triangle.IsValid())
                return eUndefined;
            return niGeomMath2d::Orientation(m_triangle.P0, m_triangle.P1, m_triangle.P2);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Reverse
        //-----------------------------------------------------------------------------
        /**
        * Reverse the triangle, for example, change the orientation from CW to CCW
        *
        * return        true:           success
        *               false:          failed
        */
        bool niTriangle2dFn::Reverse()
        {
            if (!m_triangle.IsValid())
                return false;
            niPoint2d swap(m_triangle.P0);
            m_triangle.P0 = m_triangle.P2;
            m_triangle.P2 = swap;
            return true;
        }


    }
}