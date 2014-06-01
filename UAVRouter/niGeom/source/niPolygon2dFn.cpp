//! \file
// \brief
// Operator to handle polygon  class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niPolygon2dFn.h>
#include <niGeom/geometry/niPolygon2d.h>
#include <niGeom/geometry/niBBox2d.h>
#include <niGeom/geometry/niGeomMath2d.h>

#include <algorithm>

namespace ni
{
    namespace geometry
    {


        double niPolygon2dFn::Area() const
        {
            return niGeomMath2d::Area(m_polygon.GetPoints());
        }
        //-----------------------------------------------------------------------------
        // FUNCTION Orientation
        //-----------------------------------------------------------------------------
        /**
        * Calc polygon orientation
        *
        * @return      eCounterClockwise / eClockwise
        */
        EOrientation niPolygon2dFn::Orientation() const
        {
            return niGeomMath2d::Orientation(m_polygon.GetPoints());
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Reverse
        //-----------------------------------------------------------------------------
        /**
        * Reverse polygon
        *
        */
        bool niPolygon2dFn::Reverse()
        {
            if (!m_polygon.IsValid())
                return false;

            std::reverse(m_polygon.m_points.begin(), m_polygon.m_points.end());
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Scale
        //-----------------------------------------------------------------------------
        /**
        * Scale the polygon
        *
        * @param       _scale_:     scale value
        * @return      true:        success
        *              false:       failed
        */
        bool niPolygon2dFn::Scale(double _scale_)
        {
            if (!m_polygon.IsValid())
                return false;
            int num_points = int(m_polygon.m_points.size());
            for (int i = 0; i < num_points; ++i)
            {
                niPoint2d &p = m_polygon.m_points[i];
                p *= _scale_;
            }
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION TransToCenter
        //-----------------------------------------------------------------------------
        /**
        * TransToCenter the polygon
        *
        * @return      true:        success
        *              false:       failed
        */
        bool niPolygon2dFn::TransToCenter()
        {
            if (!m_polygon.IsValid())
                return false;
            int num_points = int(m_polygon.m_points.size());

            niBBox2d bbox;
            for (int i = 0; i < num_points; ++i)
            {
                bbox.Append(m_polygon.m_points[i]);
            }

            niPoint2d center = (bbox.P1 + bbox.P2) * 0.5;
            for (int i = 0; i < num_points; ++i)
            {
                niPoint2d &p = m_polygon.m_points[i];
                p -= center;
            }
            return true;
        }
    }
}