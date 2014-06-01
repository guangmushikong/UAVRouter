//! \file
// \brief
// Polygon 3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niPolygon3d_H
#define niPolygon3d_H

#include <niGeom/geometry/niGeom3dTypes.h>
#include <ostream>

namespace ni
{
    namespace geometry
    {
        class niGeomMath3d;
        class niPolygon3dFn;

        /**
         * \brief Polygon 3d class
         *
         */
        class niPolygon3d
        {
            friend class niGeomMath3d;
            friend class niPolygon3dFn;
        public:
            niPolygon3d             () : m_valid(false)    {}

            niPolygon3d             (niVert3dArray &verts)
            {
                MakePolygon(verts);
            }

            niPolygon3d             (const niPolygon3d &polygon)
            {
                m_valid = polygon.m_valid;
                m_verts = polygon.m_verts;
            }

            void                    Append              (const niVert3d &p);

            void                    Clear               ();

            bool                    GetVerts            (niVert3dArray &verts) const;

            inline bool             IsValid             () const
            {
                return (m_verts.size() > 2);
            }

            bool                    MakePolygon         (niVert3dArray &verts);

            inline int              NumVerts            () const
            {
                return int(m_verts.size());
            }

            niVert3d                operator[]          (int idx) const
            {
                return m_verts[idx];
            }

            niPolygon3d&            operator=           (const niPolygon3d &other)
            {
                if (this == &other)
                    return *this;

                m_valid = other.m_valid;
                m_verts = other.m_verts;

                return *this;
            }

            bool                    Write2OBJ           (std::ostream &out);

            bool                    Write2Wkt           (std::ostream &out);

        protected:
            niVert3dArray   m_verts;
            bool                    m_valid;

        };
    }
}

#endif