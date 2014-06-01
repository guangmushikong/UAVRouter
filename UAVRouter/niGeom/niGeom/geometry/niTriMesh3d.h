//! \file
// \brief
// niTriMesh3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niTriMesh3d_H
#define niTriMesh3d_H

#include <niGeom/geometry/niGeomDef.h>
#include <niGeom/geometry/niGeom3dTypes.h>
#include <niGeom/geometry/niTexCoord.h>
#include <ostream>

#include <niGeom/niRefCounter.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief The 3d triangle mesh
         *
         */
        class niTriMesh3d : public niRefCounter
        {
            friend class niTriMesh3dFn;
            friend class niTriMesh3dTopo;

        public:
            niTriMesh3d             () : m_is_valid(false) {}
            niTriMesh3d             (const niTriMesh3d &tri_mesh);
            
            void                    Clear                   ();

            bool                    GetFaceVerts           (int fIdx, niVert3d &P1, niVert3d &P2, niVert3d &P3) const
            {
                if (fIdx < 0 || fIdx >= int(m_faces.size()))
                    return false;

                const niTriFace3d &tf = m_faces[fIdx];
                P1 = m_verts[tf.aRef.vRef];
                P2 = m_verts[tf.bRef.vRef];
                P3 = m_verts[tf.cRef.vRef];
                return true;
            }

            bool                    GetFaceVertRefs        (int fIdx, int &_a, int &_b, int &_c) const
            {
                if (fIdx < 0 || fIdx >= int(m_faces.size()))
                    return false;

                const niTriFace3d &tf = m_faces[fIdx];
                _a = tf.aRef.vRef;
                _b = tf.bRef.vRef;
                _c = tf.cRef.vRef;
                return true;
            }

            void                    GetVert                 (int vIdx, niVert3d &V) const
            {
                V = m_verts[vIdx];
            }

            bool                    GetVerts                (niVert3dArray &verts) const;
                        
            bool                    GetNorms                (niNorm3dArray &norms) const;

            bool                    GetTexCoords            (niTexCoordArray &tex_coords) const;

            bool                    GetFaces                (niTriFace3dArray &faces) const;

            bool                    InitFaces               (const niTriFace3dArray &faces);

            bool                    InitVerts               (const niVert3dArray &verts);

            bool                    InitNorms               (const niNorm3dArray &verts);

            bool                    InitTexCoords           (const niTexCoordArray &tex_coords);

            bool                    IsValid                 () const
            {
                return m_is_valid;
            }

            bool                    MakeTriMesh             (
                                                            const niVert3dArray &verts,
                                                            const niNorm3dArray &norms,
                                                            const niTexCoordArray &tex_coords,
                                                            const niTriFace3dArray &tri_faces);

            bool                    MakeTriMesh             (
                                                            const niVert3dArray &verts,
                                                            const niTriFace3dArray &tri_faces);

            bool                    MakeSubTriMesh          (
                                                            const niVert3dArray &verts,
                                                            const niNorm3dArray &norms,
                                                            const niTexCoordArray &tex_coords,
                                                            const niTriFace3dArray &tri_faces);

            bool                    MakeTriMeshFromBlob     (
                                                            int                     vertBlobSize,
                                                            const Uint8*    vertBlob,
                                                            int                     normBlobSize,
                                                            const Uint8*    normBlob,
                                                            int                     texcoordBlobSize,
                                                            const Uint8*    texcoordBlob,
                                                            int                     triangleBlobSize,
                                                            const Uint8*    triangleBlob);

            int                     NumFaces                () const
            {
                return int(m_faces.size());
            }


            int                     NumVerts               () const
            {
                return int(m_verts.size());
            }

            void                    SetName                 (const String &name)
            {
                m_mesh_name = name;
            }

            bool                    Write2OBJ               (std::ostream &out);

        protected:
            String                  m_mesh_name;
            niVert3dArray           m_verts;
            niNorm3dArray           m_norms;
            niTexCoordArray         m_tex_coords;
            niTriFace3dArray        m_faces;
            bool                    m_is_valid;
        };
    }
}

#endif
