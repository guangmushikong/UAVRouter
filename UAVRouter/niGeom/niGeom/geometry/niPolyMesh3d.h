#ifndef niPolyMesh3d_H
#define niPolyMesh3d_H

#include <niGeom/geometry/niGeomDef.h>
#include <niGeom/geometry/niGeom3dTypes.h>
#include <niGeom/geometry/niTexCoord.h>
#include <ostream>
namespace ni
{
    namespace geometry
    {
        /**
         * \brief 3d polygon mesh
         *
         */
        class niPolyMesh3d
        {
        public:
            niPolyMesh3d                () : m_is_valid(false) {}
            niPolyMesh3d                (const niPolyMesh3d &tri_mesh);

            void                        Clear                       ();

            bool                        GetVerts                    (niVert3dArray &points) const;

            bool                        InitFaces                   (const niPolyFace3dArray &faces);

            bool                        InitVerts                   (const niVert3dArray &verts);

            bool                        InitNorms                   (const niNorm3dArray &norms);

            bool                        InitUVs                     (const niTexCoordArray &uvs);

            bool                        IsValid                     () const
            {
                return m_is_valid;
            }

            bool                        MakePolyMesh                (
                                                                    const niVert3dArray &verts,
                                                                    const niNorm3dArray &norms,
                                                                    const niTexCoordArray &uvs,
                                                                    const niPolyFace3dArray &poly_faces);

            int                         NumFaces                    () const
            {
                return int(m_faces.size());
            }


            int                         NumVerts                    () const
            {
                return int(m_verts.size());
            }

            int                         NumNorms                    () const
            {
                return int(m_norms.size());
            }

            int                         NumUVs                      () const
            {
                return int(m_tex_coords.size());
            }

            bool                        Write2OBJ                   (std::ostream &out);

        protected:
            niVert3dArray               m_verts;
            niNorm3dArray               m_norms;
            niTexCoordArray             m_tex_coords;
            niPolyFace3dArray           m_faces;
            bool                        m_is_valid;
        };
    }
}


#endif
