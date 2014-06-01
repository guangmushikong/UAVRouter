//! \file
// \brief
// Polygon class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niPolygon2d_H
#define niPolygon2d_H

#include <niGeom/geometry/niGeom2dTypes.h>
#include <ostream>
namespace ni
{
    namespace geometry
    {
        class niGeomMath2d;
        class niPolygon2dFn;

        /**
         * \brief Polygon 2d class
         *
         */
        class niPolygon2d
        {
            friend class niGeomMath2d;
            friend class niPolygon2dFn;
        public:
            niPolygon2d             () : m_valid(false)    {}

            niPolygon2d             (niPoint2dArray &points)
            {
                MakePolygon(points);
            }

            niPolygon2d             (const niPolygon2d &polygon)
            {
                m_valid = polygon.m_valid;
                m_points = polygon.m_points;
            }

            void                    Append              (const niPoint2d &p);

            void                    Clear               ();

            bool                    GetPoints           (niPoint2dArray &points) const;

            const niPoint2dArray&   GetPoints           () const;

            inline bool             IsValid             () const
            {
                return (m_points.size() > 2);
            }

            bool                    MakePolygon         (niPoint2dArray &points);

            inline int              NumPoints           () const
            {
                return int(m_points.size());
            }
            
            niPoint2d&              operator[]          (int idx)
            {
                return m_points[idx];
            }

            const niPoint2d&        operator[]          (int idx) const
            {
                return m_points[idx];
            }

            niPolygon2d&            operator=           (const niPolygon2d &other)
            {
                if (this == &other)
                    return *this;

                m_valid = other.m_valid;
                m_points = other.m_points;

                return *this;
            }

            bool                    Write2OBJ           (std::ostream &out);

            bool                    Write2Wkt           (std::ostream &out);

        protected:
            niPoint2dArray          m_points;
            bool                    m_valid;

        };
    }
}

#endif
