//! \file
// \brief
// Triangle mesh class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niTriMesh2d.h>
#include <niGeom/geometry/niBBox2d.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION Copy construction
        //-----------------------------------------------------------------------------
        /**
        * Copy construction
        *
        * @param       tri_mesh:    other trimesh
        */
        niTriMesh2d::niTriMesh2d(const niTriMesh2d &tri_mesh)
        {
            m_points = tri_mesh.m_points;
            m_faces = tri_mesh.m_faces;
            m_is_valid = tri_mesh.m_is_valid;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Clear
        //-----------------------------------------------------------------------------
        /**
        * Clear memory
        *
        */
        void niTriMesh2d::Clear()
        {
            m_points.clear();
            m_faces.clear();
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetPoints
        //-----------------------------------------------------------------------------
        /**
        * Get points
        *
        * @param        points:         store return points
        * return        true:           success
        *               false:          failed
        */
        bool niTriMesh2d::GetPoints(niPoint2dArray &points) const
        {
            points.clear();
            if (m_points.size() < 3)
                return false;

            points = m_points;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION InitFaces
        //-----------------------------------------------------------------------------
        /**
        * Init trifaces
        *
        * @param        &faces:         trifaces.
        * return        true:           success
        *               false:          failed
        */
        bool niTriMesh2d::InitFaces(const niTriFace2dArray &faces)
        {
            m_is_valid = false;
            m_faces.clear();
            int num_points = int(m_points.size());
            if (num_points < 3)
                return false;

            niCharArray valid_buffs;
            valid_buffs.resize(num_points, 0);

            size_t num_faces = faces.size();

            m_faces.reserve(num_faces);

            for (size_t i = 0; i < num_faces; ++i)
            {
                const niTriFace2d &f = faces[i];
                if (f.aIdx >= num_points || f.aIdx < 0)
                    continue;
                if (f.bIdx >= num_points || f.bIdx < 0)
                    continue;
                if (f.cIdx >= num_points || f.cIdx < 0)
                    continue;
                valid_buffs[f.aIdx] = 1;
                valid_buffs[f.bIdx] = 1;
                valid_buffs[f.cIdx] = 1;
                m_faces.push_back(f);
            }

            m_is_valid = true;
            for (int i = 0; i < num_points; ++i)
            {
                if (0 == valid_buffs[i])
                {
                    m_is_valid = false;
                    break;
                }
            }
            return m_is_valid;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION InitPoints
        //-----------------------------------------------------------------------------
        /**
        * Create triangle points
        *
        * @param       points:      points
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2d::InitPoints(const niPoint2dArray &points)
        {
            m_is_valid = false;
            m_faces.clear();
            m_points.clear();
            size_t num = points.size();
            if (num < 3)
                return false;

            m_points.reserve(num);

            m_points.push_back(points[0]);

            niPoint2d lastPoint = points[0];
            for (size_t i = 1; i < num; ++i)
            {
                const niPoint2d &point = points[i];
                if (point != lastPoint)
                {
                    m_points.push_back(point);
                    lastPoint = point;
                }
            }
            if (lastPoint == points[0])
                m_points.pop_back();

            return (m_points.size() >= 3);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeTriMesh
        //-----------------------------------------------------------------------------
        /**
        * Create triangle mesh
        *
        * @param       points:      points
        * @param       tri_faces:   tri_faces
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2d::MakeTriMesh(
            const niPoint2dArray &points,
            const niTriFace2dArray &tri_faces)
        {
            bool bStat = InitPoints(points);
            if (!bStat)
                return false;

            bStat = InitFaces(tri_faces);

            return bStat;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Write2OBJ
        //-----------------------------------------------------------------------------
        /**
        * Dump to wavefront obj format
        *
        * @param       out:         ostream
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2d::Write2OBJ(std::ostream &out)
        {
            if (!m_is_valid)
                return false;

            int num_points = int(m_points.size());

            for (int i = 0; i < num_points; ++i)
            {
                niPoint2d &p = m_points[i];
                out <<std::setprecision(16)
                    <<"v "<<p.X<<" "<<p.Y<<" 0"<<std::endl;
            }
            out<<"g"<<std::endl;
            int num_faces = int(m_faces.size());
            for (int i = 0; i < num_faces; ++i)
            {
                niTriFace2d &f = m_faces[i];
                out <<"f "
                    <<f.aIdx-num_points<<" "
                    <<f.bIdx-num_points<<" "
                    <<f.cIdx-num_points<<std::endl;
            }
            out<<std::endl<<std::endl;

            out.flush();

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Write2OBJ
        //-----------------------------------------------------------------------------
        /**
        * Dump to wavefront obj format, and use different smooth group to mark different
        *   fan/strip  group.
        *
        * @param        out:            ostream
        * @param        mesh_fan_strip: fan / strip
        * @return       true:           success
        *               false:          failed
        */
        bool niTriMesh2d::Write2OBJ(std::ostream &out, niArrayT<_STriMeshFanStrip> &mesh_fan_strip)
        {
            if (!m_is_valid)
                return false;

            int num_points = int(m_points.size());


            for (int i = 0; i < num_points; ++i)
            {
                niPoint2d &p = m_points[i];
                out <<std::setprecision(16)
                    <<"v "<<p.X<<" "<<p.Y<<" 0"<<std::endl;
            }
            out<<"g"<<std::endl;

            int num_fans = int(mesh_fan_strip.size());
            for (int i = 0; i < num_fans; ++i)
            {
                //sprintf_s(mtl_name, 255, "usemtl MAT_FAN_%05d", i);
                //out <<mtl_name<<std::endl;
                out <<"s  "<<i+1<<std::endl;

                _STriMeshFanStrip &fan_strip = mesh_fan_strip[i];
                int num = int(fan_strip.m_IdOfFaces.size());
                for (int j = 0; j < num; ++j)
                {
                    niTriFace2d &f = m_faces[ fan_strip.m_IdOfFaces[j] ];
                    out <<"f "
                        <<f.aIdx-num_points<<" "
                        <<f.bIdx-num_points<<" "
                        <<f.cIdx-num_points<<std::endl;
                }
            }

            out<<std::endl<<std::endl;

            out.flush();
            return true;
        }
    }
}