#include <niGeom/geometry/niPolyMesh3d.h>
#include <niGeom/geometry/niBBox3d.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace ni
{
    namespace geometry
    {

        niPolyMesh3d::niPolyMesh3d(const niPolyMesh3d &tri_mesh)
        {
            m_verts = tri_mesh.m_verts;
            m_faces = tri_mesh.m_faces;
            m_is_valid = tri_mesh.m_is_valid;
        }

        void niPolyMesh3d::Clear()
        {
            m_verts.clear();
            m_faces.clear();
        }

        bool niPolyMesh3d::GetVerts(niVert3dArray &verts) const
        {
            verts.clear();
            if (m_verts.size() < 3)
                return false;

            verts = m_verts;
            return true;
        }

        bool niPolyMesh3d::InitFaces(const niPolyFace3dArray &faces)
        {
            m_is_valid = false;
            m_faces.clear();

            int num_verts = int(m_verts.size());
            if (num_verts < 3)
                return false;

            int num_faces = int(faces.size());
            if (num_faces < 1)
                return false;

            m_faces = faces;

            int vRef;
            niCharArray valid_uerts;
            valid_uerts.resize(num_verts, 0);

            for (int i = 0; i < num_faces; ++i)
            {
                const niPolyFace3d &poly_face = faces[i];
                int num_verts = int(poly_face.m_refs.size());
                if (num_verts < 3)
                    return false;

                for (int k = 0; k < num_verts; ++k)
                {
                    vRef = poly_face.m_refs[k].vRef;
                    valid_uerts[vRef] = 1;
                }

                m_faces.push_back( poly_face );
            }

            m_is_valid = true;
            for (int i = 0; i < num_verts; ++i)
            {
                if (0 == valid_uerts[i])
                {
                    m_is_valid = false;
                    break;
                }
            }
            return m_is_valid;
        }

        bool niPolyMesh3d::InitVerts(const niVert3dArray &verts)
        {
            m_is_valid = false;
            m_verts = verts;
            return (m_verts.size() >= 3);
        }

        bool niPolyMesh3d::InitNorms(const niNorm3dArray &norms)
        {
            m_is_valid = false;
            m_norms = norms;
            return (m_norms.size() >= 3);
        }

        bool niPolyMesh3d::InitUVs(const niTexCoordArray &uvs)
        {
            m_is_valid = false;
            m_tex_coords = uvs;
            return (m_tex_coords.size() >= 3);
        }

        bool niPolyMesh3d::MakePolyMesh(
            const niVert3dArray &verts,
            const niVert3dArray &norms,
            const niTexCoordArray &uvs,
            const niPolyFace3dArray &poly_faces)
        {
            bool bStat = InitVerts(verts);
            if (!bStat)
                return false;

            InitNorms(norms);
            InitUVs(uvs);

            bStat = InitFaces(poly_faces);

            return bStat;
        }

        bool niPolyMesh3d::Write2OBJ(std::ostream &out)
        {
            if (!m_is_valid)
                return false;

            int num_verts = int(m_verts.size());

            niBBox3d bbox;
            for (int i = 0; i < num_verts; ++i)
            {
                bbox.Append(m_verts[i]);
            }
            niVert3d center;
            center = (bbox.P1 + bbox.P2) * 0.5;

            double maxBorder = bbox.Xlength() * 0.5;


            int exponent = (int)log10(maxBorder) - 2;
            double _SCALE_ = 1.0 / pow(10.0, exponent);

            for (int i = 0; i < num_verts; ++i)
            {
                niVert3d p = m_verts[i] - center;
                out <<std::setprecision(9)
                    <<"v "<<p.X*_SCALE_
                    <<" "<<p.Y*_SCALE_<<" 0"<<std::endl;
            }
            out<<"g"<<std::endl;
            int num_faces = int(m_faces.size());
            for (int i = 0; i < num_faces; ++i)
            {
                niPolyFace3d &poly_face = m_faces[i];
                int vRef;
                int nv = int(poly_face.m_refs.size());
                out <<"f";
                for (int k = 0; k < nv; ++k)
                {
                    vRef = poly_face.m_refs[k].vRef;
                    out<<" "<<vRef - num_verts;
                }
                out<<std::endl;
            }
            out<<std::endl<<std::endl;

            out.flush();

            return true;
        }
    }
}