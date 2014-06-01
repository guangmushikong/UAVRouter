//! \file
// \brief
// Triangle mesh class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTriMesh2d_H
#define niTriMesh2d_H

#include <niGeom/geometry/niGeomDef.h>
#include <niGeom/geometry/niGeom2dTypes.h>
#include <ostream>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief trimesh fan or strip
         *
         */
        struct _STriMeshFanStrip
        {
            niIntArray    m_IdOfPoints;
            niIntArray    m_IdOfFaces;
        };

        /**
         * \brief 2d Triangle mesh class
         *
         */
        class niTriMesh2d
        {
            friend class niTriMesh2dFn;
            friend class niTriMesh2dTopo;

        public:
            niTriMesh2d             () : m_is_valid(false) {}
            niTriMesh2d             (const niTriMesh2d &tri_mesh);

            void                    Clear                       ();

            bool                    GetPoints                   (niPoint2dArray &points) const;

            const niPoint2dArray&   GetPoints                   () const
            {
                return m_points;
            }

            const niTriFace2dArray& GetFaces                    () const
            {
                return m_faces;
            }

            bool                    InitFaces                   (const niTriFace2dArray &faces);

            bool                    InitPoints                  (const niPoint2dArray &points);

            bool                    IsValid                     () const
            {
                return m_is_valid;
            }

            bool                    MakeTriMesh                 (
                                                                const niPoint2dArray &points,
                                                                const niTriFace2dArray &tri_faces);

            int                     NumFaces                    () const
            {
                return int(m_faces.size());
            }


            int                     NumPoints                   () const
            {
                return int(m_points.size());
            }

            bool                    Write2OBJ                   (std::ostream &out);

            bool                    Write2OBJ                   (
                                                                std::ostream &out,
                                                                niArrayT<_STriMeshFanStrip> &mesh_fan_strip);

        protected:
            niPoint2dArray          m_points;
            niTriFace2dArray        m_faces;
            bool                    m_is_valid;
        };
    }
}


#endif
