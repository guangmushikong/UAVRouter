//! \file
// \brief
// Polygon class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niPolygon2d.h>

#include <fstream>
#include <iomanip>

namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION Append
        //-----------------------------------------------------------------------------
        /**
        * Add a point into polygon.
        *
        * @param       other:           point
        */
        void niPolygon2d::Append(const niPoint2d &other)
        {
            int num = int(m_points.size());
            switch(num)
            {
            case 0:
                break;
            case 1:
                if (other == m_points[0])
                    return;
            default:
                if (other == m_points[0])
                    return;
                if (other == m_points[num-1])
                    return;
            }
            m_points.push_back(other);
            m_valid = (m_points.size() > 2);
        }

        void niPolygon2d::Clear()
        {
            m_points.clear();
            m_valid = false;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetPoints
        //-----------------------------------------------------------------------------
        /**
        * Get a copy of points in polygon
        *
        * @param       points:      store return points
        * @return      true:        success
        *              false:       failed
        */
        bool niPolygon2d::GetPoints(
            niPoint2dArray &points) const
        {
            points.clear();
            if (!m_valid)
                return false;
            points = m_points;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetPoints
        //-----------------------------------------------------------------------------
        /**
        * Get a copy of points in polygon
        *
        * @param       points:      store return points
        * @return      true:        success
        *              false:       failed
        */
        const niPoint2dArray& niPolygon2d::GetPoints() const
        {
            return m_points;
        }



        //-----------------------------------------------------------------------------
        // FUNCTION MakePolygon
        //-----------------------------------------------------------------------------
        /**
        * Make a polygon from input points
        *
        * @param       points:      input points
        * @return      true:        success to create a polygon
        *              false:       failed to create a polygon
        */
        bool niPolygon2d::MakePolygon(niPoint2dArray &points)
        {
            m_points.clear();
            m_valid = false;
            size_t num = points.size();
            if (num < 3)
                return false;

            m_points.reserve(num);

            m_points.push_back(points[0]);

            niPoint2d lastPoint = points[0];
            for (size_t i = 1; i < num; ++i)
            {
                niPoint2d &point = points[i];
                if (point != lastPoint)
                {
                    m_points.push_back(point);
                    lastPoint = point;
                }
            }
            if (lastPoint == points[0])
                m_points.pop_back();

            m_valid = (m_points.size() >= 3);

            return m_valid;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Write2OBJ
        //-----------------------------------------------------------------------------
        /**
        * Dump to wavefront format
        *
        * @param       out:         ostream
        * @return      true:        success
        *              false:       failed
        */
        bool niPolygon2d::Write2OBJ(std::ostream &out)
        {
            if (!m_valid)
                return false;

            int num_points = int(m_points.size());

            double max = -1e30;
            for (int i = 0; i < num_points; ++i)
            {
                niPoint2d &p = m_points[i];
                if (max < fabs(p.X))
                    max = fabs(p.X);
                if (max < fabs(p.Y))
                    max = fabs(p.Y);
            }

            int exponent = (int)log10(max) - 2;
            double _SCALE_ = 1.0 / pow(10.0, exponent);

            for (int i = 0; i < num_points; ++i)
            {
                niPoint2d &p = m_points[i];
                out<<std::setprecision(9)<<"v "<<p.X*_SCALE_<<" "<<p.Y*_SCALE_<<" 0"<<std::endl;
            }
            out<<"g"<<std::endl;
            out<<"f ";
            for (int i = 0; i < num_points; ++i)
            {
                out<<i-num_points<<" ";
            }
            out<<std::endl<<std::endl;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Write2Wkt
        //-----------------------------------------------------------------------------
        /**
        * Dump to wkt format
        *
        * @param       out:         ostream
        * @return      true:        success
        *              false:       failed
        */
        bool niPolygon2d::Write2Wkt(std::ostream &out)
        {
            if (!m_valid)
                return false;

            out<<"POLYGON((";
            out<<std::setprecision(16)<<m_points[0].X<<" "<<m_points[0].Y;

            int num_points = int(m_points.size());
            for (int i = 1; i < num_points; ++i)
            {
                niPoint2d &p = m_points[i];
                out<<std::setprecision(16)<<", "<<p.X<<" "<<p.Y;
            }
            out<<"))"<<std::endl;

            return true;
        }
    }
}