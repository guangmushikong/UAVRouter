//! \file
// \brief
// niTriMesh3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#include <niGeom/geometry/niTriMesh3d.h>
#include <niGeom/geometry/niBBox3d.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION Copy Construction
        //-----------------------------------------------------------------------------
        /**
        * Copy Construction
        *
        * @param       other:           tri_mesh
        */
        niTriMesh3d::niTriMesh3d(const niTriMesh3d &tri_mesh)
        {
            m_verts = tri_mesh.m_verts;
            m_faces = tri_mesh.m_faces;
            m_is_valid = tri_mesh.m_is_valid;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Clear
        //-----------------------------------------------------------------------------
        /**
        * Clear memory
        *
        * @param       other:           tri_mesh
        */
        void niTriMesh3d::Clear()
        {
            m_verts.clear();
            m_norms.clear();
            m_tex_coords.clear();
            m_faces.clear();
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetVerts
        //-----------------------------------------------------------------------------
        /**
        * GetVerts
        *
        * @param        verts:              used to store return verts
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::GetVerts(niVert3dArray &verts) const
        {
            verts.clear();
            if (m_verts.size() < 3)
                return false;

            verts = m_verts;
            return true;
        }
        
        //-----------------------------------------------------------------------------
        // FUNCTION GetNorms
        //-----------------------------------------------------------------------------
        /**
        * GetNorms
        *
        * @param        norms:              used to store return norms
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::GetNorms(niNorm3dArray &norms) const
        {
            norms.clear();
            if (m_norms.size() < 3)
                return false;

            norms = m_norms;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetTexCoords
        //-----------------------------------------------------------------------------
        /**
        * GetTexCoords
        *
        * @param        tex_coords:         used to store return tex_coords
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::GetTexCoords(niTexCoordArray &tex_coords) const
        {
            tex_coords.clear();
            if (m_tex_coords.size() < 3)
                return false;

            tex_coords = m_tex_coords;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetFaces
        //-----------------------------------------------------------------------------
        /**
        * GetTexCoords
        *
        * @param        faces:              used to store return faces
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::GetFaces(niTriFace3dArray &faces) const
        {
            faces.clear();
            if (m_faces.size() < 1)
                return false;

            faces = m_faces;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION InitFaces
        //-----------------------------------------------------------------------------
        /**
        * InitFaces
        *
        * @param        faces:              faces
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::InitFaces(const niTriFace3dArray &faces)
        {
            m_is_valid = false;
            m_faces.clear();
            int num_faces = int(faces.size());
            if (num_faces < 1)
                return false;

            int num_verts = int(m_verts.size());
            if (num_verts < 3)
                return false;

            niCharArray valid_buffs;

            //validate verts
            valid_buffs.resize(num_verts, 0);
            for (int i = 0; i < num_faces; ++i)
            {
                const niTriFace3d &f = faces[i];
                if (f.aRef.vRef >= num_verts || f.aRef.vRef < 0)
                    continue;
                if (f.bRef.vRef >= num_verts || f.bRef.vRef < 0)
                    continue;
                if (f.cRef.vRef >= num_verts || f.cRef.vRef < 0)
                    continue;
                valid_buffs[f.aRef.vRef] = 1;
                valid_buffs[f.bRef.vRef] = 1;
                valid_buffs[f.cRef.vRef] = 1;
                m_faces.push_back(f);
            }

            for (int i = 0; i < num_verts; ++i)
            {
                if (0 == valid_buffs[i])
                    return m_is_valid;
            }


            num_faces = int(m_faces.size());
            if (num_faces < 1)
                return m_is_valid;

            //validate texcoords
            int num_texcoords = int(m_tex_coords.size());
            if (num_texcoords > 0)
            {
                valid_buffs.resize(num_texcoords, 0);
                for (int i = 0; i < num_faces; ++i)
                {
                    const niTriFace3d &f = m_faces[i];
                    if (f.aRef.tRef >= num_texcoords || f.aRef.tRef < 0)
                        return m_is_valid;
                    if (f.bRef.tRef >= num_texcoords || f.bRef.tRef < 0)
                        return m_is_valid;
                    if (f.cRef.tRef >= num_texcoords || f.cRef.tRef < 0)
                        return m_is_valid;
                    valid_buffs[f.aRef.tRef] = 1;
                    valid_buffs[f.bRef.tRef] = 1;
                    valid_buffs[f.cRef.tRef] = 1;
                }
            }
            for (int i = 0; i < num_texcoords; ++i)
            {
                if (0 == valid_buffs[i])
                    return m_is_valid;
            }


            //validate normals
            int num_norms = int(m_norms.size());
            if (num_norms > 0)
            {
                valid_buffs.resize(num_norms, 0);
                for (int i = 0; i < num_faces; ++i)
                {
                    const niTriFace3d &f = m_faces[i];
                    if (f.aRef.nRef >= num_norms || f.aRef.nRef < 0)
                        return m_is_valid;
                    if (f.bRef.nRef >= num_norms || f.bRef.nRef < 0)
                        return m_is_valid;
                    if (f.cRef.nRef >= num_norms || f.cRef.nRef < 0)
                        return m_is_valid;
                    valid_buffs[f.aRef.nRef] = 1;
                    valid_buffs[f.bRef.nRef] = 1;
                    valid_buffs[f.cRef.nRef] = 1;
                }
            }
            for (int i = 0; i < num_norms; ++i)
            {
                if (0 == valid_buffs[i])
                    return m_is_valid;
            }

            m_is_valid = true;
            return m_is_valid;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION InitVerts
        //-----------------------------------------------------------------------------
        /**
        * InitVerts
        *
        * @param        verts:              verts
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::InitVerts(const niVert3dArray &verts)
        {
            m_is_valid = false;
            m_verts = verts;
            return (m_verts.size() >= 3);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION InitNorms
        //-----------------------------------------------------------------------------
        /**
        * InitNorms
        *
        * @param        norms:              norms
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::InitNorms(const niNorm3dArray &norms)
        {
            m_is_valid = false;
            m_norms = norms;
            return (m_norms.size() >= 1);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION InitTexCoords
        //-----------------------------------------------------------------------------
        /**
        * InitTexCoords
        *
        * @param        tex_coords:         tex_coords
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::InitTexCoords(const niTexCoordArray &tex_coords)
        {
            m_is_valid = false;
            m_tex_coords = tex_coords;
            return (m_tex_coords.size() >= 1);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeTriMesh
        //-----------------------------------------------------------------------------
        /**
        * MakeTriMesh
        *
        * @param        verts:              verts
        * @param        norms:              norms
        * @param        tex_coords:         tex_coords
        * @param        tri_faces:          tri_faces
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::MakeTriMesh(
            const niVert3dArray &verts,
            const niNorm3dArray &norms,
            const niTexCoordArray &tex_coords,
            const niTriFace3dArray &tri_faces)
        {
            bool bStat = InitVerts(verts);
            if (!bStat)
                return false;

            InitNorms(norms);

            InitTexCoords(tex_coords);

            bStat = InitFaces(tri_faces);

            return bStat;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeTriMesh
        //-----------------------------------------------------------------------------
        /**
        * MakeTriMesh
        *
        * @param        verts:              verts
        * @param        tri_faces:          tri_faces
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::MakeTriMesh(
            const niVert3dArray &verts,
            const niTriFace3dArray &tri_faces)
        {
            bool bStat = InitVerts(verts);
            if (!bStat)
                return false;

            bStat = InitFaces(tri_faces);
            return bStat;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION Write2OBJ
        //-----------------------------------------------------------------------------
        /**
        * Dump to wavefront obj
        *
        * @param        out:                ostream
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::Write2OBJ(std::ostream &out)
        {
            if (!m_is_valid)
                return false;

            int num_verts = int(m_verts.size());
            for (int i = 0; i < num_verts; ++i)
            {
                niVert3d &pnt = m_verts[i];
                out <<std::setprecision(18) <<"v "
                    <<pnt.X<<" "
                    <<pnt.Y<<" "
                    <<pnt.Z<<std::endl;
            }
            int num_texcoords = int(m_tex_coords.size());
            bool hasTexCoord = (num_texcoords > 0);
            for (int i = 0; i < num_texcoords; ++i)
            {
                niTexCoord &texcoord = m_tex_coords[i];
                out <<std::setprecision(18) <<"vt "<<texcoord.S<<" "<<texcoord.T<<std::endl;
            }
            int num_norms = int(m_norms.size());
            bool hasNorm = (num_norms > 0);
            for (int i = 0; i < num_norms; ++i)
            {
                niNorm3d norm = m_norms[i];
                out <<std::setprecision(18) <<"vn "
                    <<norm.X<<" "
                    <<norm.Y<<" "
                    <<norm.Z<<std::endl;
            }

            out<<"g "<<m_mesh_name<<std::endl;
            int num_faces = int(m_faces.size());
            for (int i = 0; i < num_faces; ++i)
            {
                niTriFace3d &f = m_faces[i];
                out <<'f';

                out<<' '<<f.aRef.vRef-num_verts;
                if (hasTexCoord && hasNorm)
                    out<<'/'<<f.aRef.tRef-num_texcoords<<'/'<<f.aRef.nRef-num_norms;
                else if (hasTexCoord && !hasNorm)
                    out<<'/'<<f.aRef.tRef-num_texcoords;
                else if (!hasTexCoord && hasNorm)
                    out<<"//"<<f.aRef.nRef-num_norms;
                out<<' ';
                
                out<<' '<<f.bRef.vRef-num_verts;
                if (hasTexCoord && hasNorm)
                    out<<'/'<<f.bRef.tRef-num_texcoords<<'/'<<f.bRef.nRef-num_norms;
                else if (hasTexCoord && !hasNorm)
                    out<<'/'<<f.bRef.tRef-num_texcoords;
                else if (!hasTexCoord && hasNorm)
                    out<<"//"<<f.bRef.nRef-num_norms;
                out<<' ';

                out<<' '<<f.cRef.vRef-num_verts;
                if (hasTexCoord && hasNorm)
                    out<<'/'<<f.cRef.tRef-num_texcoords<<'/'<<f.cRef.nRef-num_norms;
                else if (hasTexCoord && !hasNorm)
                    out<<'/'<<f.cRef.tRef-num_texcoords;
                else if (!hasTexCoord && hasNorm)
                    out<<"//"<<f.cRef.nRef-num_norms;
                out<<std::endl;
            }
            out<<std::endl<<std::endl;

            out.flush();

            return true;
        }
        

        //-----------------------------------------------------------------------------
        // FUNCTION MakeSubTriMesh
        //-----------------------------------------------------------------------------
        /**
        * make a sub trimesh by trifaces.
        *
        * @param        verts:              verts
        * @param        norms:              norms
        * @param        tex_coords:         tex_coords
        * @param        tri_faces:          tri_faces
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::MakeSubTriMesh(
            const niVert3dArray &verts,
            const niNorm3dArray &norms,
            const niTexCoordArray &tex_coords,
            const niTriFace3dArray &tri_faces)
        {
            int numVerts        = int(verts.size());
            int numNorms        = int(norms.size());
            int numTexcoords    = int(tex_coords.size());
            int numFaces        = int(tri_faces.size());


            if (numVerts < 1 || numFaces < 1)
            {
                return false;
            }

            bool hasNorm        = numNorms > 0;
            bool hasTexcoord    = numTexcoords > 0;


            niIntArray    vertHitCache;
            niIntArray    normHitCache;
            niIntArray    texcoordHitCache;

            vertHitCache.resize(numVerts, -1);
            normHitCache.resize(numNorms, -1);
            texcoordHitCache.resize(numTexcoords, -1);

            for (int i = 0; i < numFaces; ++i)
            {
                const niTriFace3d &triFace = tri_faces[i];
                vertHitCache[ triFace.aRef.vRef ] = 1;
                vertHitCache[ triFace.bRef.vRef ] = 1;
                vertHitCache[ triFace.cRef.vRef ] = 1;

                if (hasNorm)
                {
                    normHitCache[ triFace.aRef.nRef ] = 1;
                    normHitCache[ triFace.bRef.nRef ] = 1;
                    normHitCache[ triFace.cRef.nRef ] = 1;
                }

                if (hasTexcoord)
                {
                    texcoordHitCache[ triFace.aRef.tRef ] = 1;
                    texcoordHitCache[ triFace.bRef.tRef ] = 1;
                    texcoordHitCache[ triFace.cRef.tRef ] = 1;
                }
            }

            niVert3dArray   sub_verts;
            niNorm3dArray   sub_norms;
            niTexCoordArray sub_texcoords;
            niTriFace3dArray  sub_trifaces;

            int vCounter = 0;
            for (int i = 0; i < numVerts; ++i)
            {
                if (1 == vertHitCache[i])
                {
                    vertHitCache[i] = vCounter++;
                    sub_verts.push_back( verts[i] );
                }
            }
            int nCounter = 0;
            for (int i = 0; i < numNorms; ++i)
            {
                if (1 == normHitCache[i])
                {
                    normHitCache[i] = nCounter++;
                    sub_norms.push_back( norms[i] );
                }
            }
            int tCounter = 0;
            for (int i = 0; i < numTexcoords; ++i)
            {
                if (1 == texcoordHitCache[i])
                {
                    texcoordHitCache[i] = tCounter++;
                    sub_texcoords.push_back( tex_coords[i] );
                }
            }

            for (int i = 0; i < numFaces; ++i)
            {
                niTriFace3d triface = tri_faces[i];
                triface.aRef.vRef = vertHitCache[ triface.aRef.vRef ];
                triface.bRef.vRef = vertHitCache[ triface.bRef.vRef ];
                triface.cRef.vRef = vertHitCache[ triface.cRef.vRef ];

                if (hasNorm)
                {
                    triface.aRef.nRef = normHitCache[ triface.aRef.nRef ];
                    triface.bRef.nRef = normHitCache[ triface.bRef.nRef ];
                    triface.cRef.nRef = normHitCache[ triface.cRef.nRef ];
                }

                if (hasTexcoord)
                {
                    triface.aRef.tRef = texcoordHitCache[ triface.aRef.tRef ];
                    triface.bRef.tRef = texcoordHitCache[ triface.bRef.tRef ];
                    triface.cRef.tRef = texcoordHitCache[ triface.cRef.tRef ];
                }

                sub_trifaces.push_back( triface );
            }

            return MakeTriMesh(sub_verts, sub_norms, sub_texcoords, sub_trifaces);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeTriMeshFromBlob
        //-----------------------------------------------------------------------------
        /**
        * make a trimesh from blob
        *
        * @param        vertBlobSize:       vert list blob size
        * @param        vertBlob:           vert data
        * @param        normBlobSize:       norm list blob size
        * @param        normBlob:           norm data
        * @param        texcoordBlobSize:   texcoord list blob size
        * @param        texcoordBlob:       texcoord data
        * @param        triangleBlobSize:   triangle list blob size
        * @param        triangleBlob:       triangle data
        * return        true:               success
        *               false:              failed
        */
        bool niTriMesh3d::MakeTriMeshFromBlob(
            int                     vertBlobSize,
            const Uint8*    vertBlob,
            int                     normBlobSize,
            const Uint8*    normBlob,
            int                     texcoordBlobSize,
            const Uint8*    texcoordBlob,
            int                     triangleBlobSize,
            const Uint8*    triangleBlob)
        {
            int numVerts        = vertBlobSize / sizeof(niVert3d);
            int numNorms        = normBlobSize / sizeof(niNorm3d);
            int numTexcoords    = texcoordBlobSize / sizeof(niTexCoord);
            int numFaces        = triangleBlobSize / sizeof(niTriFace3d);

            if (numVerts < 1 || numFaces < 1)
            {
                return false;
            }

            bool hasNorm        = numNorms > 0;
            bool hasTexcoord    = numTexcoords > 0;

            const niTriFace3d* trifaces_ptr  = (niTriFace3d*)(triangleBlob);

            niIntArray    vertHitCache;
            niIntArray    normHitCache;
            niIntArray    texcoordHitCache;

            vertHitCache.resize(numVerts, -1);
            normHitCache.resize(numNorms, -1);
            texcoordHitCache.resize(numTexcoords, -1);

            for (int i = 0; i < numFaces; ++i)
            {
                const niTriFace3d &triFace = trifaces_ptr[i];
                vertHitCache[ triFace.aRef.vRef ] = 1;
                vertHitCache[ triFace.bRef.vRef ] = 1;
                vertHitCache[ triFace.cRef.vRef ] = 1;

                if (hasNorm)
                {
                    normHitCache[ triFace.aRef.nRef ] = 1;
                    normHitCache[ triFace.bRef.nRef ] = 1;
                    normHitCache[ triFace.cRef.nRef ] = 1;
                }

                if (hasTexcoord)
                {
                    texcoordHitCache[ triFace.aRef.tRef ] = 1;
                    texcoordHitCache[ triFace.bRef.tRef ] = 1;
                    texcoordHitCache[ triFace.cRef.tRef ] = 1;
                }
            }

            const niVert3d* verts_ptr      = (niVert3d*)(vertBlob);
            const niNorm3d* norms_ptr      = (niNorm3d*)(normBlob);
            const niTexCoord* texcoords_ptr= (niTexCoord*)(texcoordBlob);

            niVert3dArray   verts;
            niNorm3dArray   norms;
            niTexCoordArray texcoords;
            niTriFace3dArray  trifaces;

            int vCounter = 0;
            for (int i = 0; i < numVerts; ++i)
            {
                if (1 == vertHitCache[i])
                {
                    vertHitCache[i] = vCounter++;
                    verts.push_back( verts_ptr[i] );
                }
            }
            int nCounter = 0;
            for (int i = 0; i < numNorms; ++i)
            {
                if (1 == normHitCache[i])
                {
                    normHitCache[i] = nCounter++;
                    norms.push_back( norms_ptr[i] );
                }
            }
            int tCounter = 0;
            for (int i = 0; i < numTexcoords; ++i)
            {
                if (1 == texcoordHitCache[i])
                {
                    texcoordHitCache[i] = tCounter++;
                    texcoords.push_back( texcoords_ptr[i] );
                }
            }

            for (int i = 0; i < numFaces; ++i)
            {
                niTriFace3d triface = trifaces_ptr[i];
                triface.aRef.vRef = vertHitCache[ triface.aRef.vRef ];
                triface.bRef.vRef = vertHitCache[ triface.bRef.vRef ];
                triface.cRef.vRef = vertHitCache[ triface.cRef.vRef ];

                if (hasNorm)
                {
                    triface.aRef.nRef = normHitCache[ triface.aRef.nRef ];
                    triface.bRef.nRef = normHitCache[ triface.bRef.nRef ];
                    triface.cRef.nRef = normHitCache[ triface.cRef.nRef ];
                }

                if (hasTexcoord)
                {
                    triface.aRef.tRef = texcoordHitCache[ triface.aRef.tRef ];
                    triface.bRef.tRef = texcoordHitCache[ triface.bRef.tRef ];
                    triface.cRef.tRef = texcoordHitCache[ triface.cRef.tRef ];
                }

                trifaces.push_back( triface );
            }

            return MakeTriMesh(verts, norms, texcoords, trifaces);
        }

    }
}