//! \file
// \brief
// Topology of triangle mesh
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niTriMesh2dTopo.h>
#include <niGeom/geometry/niTriMesh2d.h>

#include <algorithm>

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
        */
        niTriMesh2dTopo::niTriMesh2dTopo(const niTriMesh2dTopo &topo_mesh)
        {
            m_valid  = topo_mesh.m_valid;
            m_v2ef_list     = topo_mesh.m_v2ef_list;
            m_e2vf_list     = topo_mesh.m_e2vf_list;
            m_f2ve_list     = topo_mesh.m_f2ve_list;
        }
        //-----------------------------------------------------------------------------
        // FUNCTION DeConstruction
        //-----------------------------------------------------------------------------
        /**
        * DeConstruction
        *
        */
        niTriMesh2dTopo::~niTriMesh2dTopo()
        {
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Copy operation
        //-----------------------------------------------------------------------------
        /**
        * Copy operation
        *
        */
        niTriMesh2dTopo& niTriMesh2dTopo::operator =(const niTriMesh2dTopo &other)
        {
            if (this == &other)
                return *this;

            m_valid     = other.m_valid;
            m_v2ef_list = other.m_v2ef_list;
            m_e2vf_list = other.m_e2vf_list;
            m_f2ve_list = other.m_f2ve_list;
            return *this;
        }
        //-----------------------------------------------------------------------------
        // FUNCTION BuildTopology
        //-----------------------------------------------------------------------------
        /**
        * Build topology of triangle mesh
        *
        * @param       trimesh:     input triangle mesh
        * @param       fBuildVert:  true indicate build vert_topo, or skip vert_topo
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dTopo::BuildTopology(const niTriMesh2d& trimesh, bool fBuildVert)
        {
            Destory();

            bool bStat;

            if (!trimesh.IsValid())
                return false;

            int num_verts = trimesh.NumPoints();
            int num_faces = trimesh.NumFaces();

            try
            {
                m_v2ef_list.resize(num_verts);
                m_f2ve_list.resize(num_faces);
            }
            catch (std::bad_alloc)
            {
                Destory();
                return false;
            }

            bStat = BuildEdge2VF(trimesh);
            if (!bStat)
                return bStat;

            bStat = BuildFace2VE(trimesh);
            if (!bStat)
                return bStat;

            if (fBuildVert)
            {
                bStat = BuildVert2EF(trimesh);
                if (!bStat)
                    return bStat;
            }

            if (fBuildVert)
                m_valid = true;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildEdge2VF
        //-----------------------------------------------------------------------------
        /**
        * Build edge topology of triangle mesh
        *
        * @param       trimesh:     input triangle mesh
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dTopo::BuildEdge2VF(const niTriMesh2d& trimesh)
        {
            int num_faces = trimesh.NumFaces();

            int all_edges = num_faces * 3;

            niEdge2VF * edgebuffs = NULL;

            try
            {
                edgebuffs = new niEdge2VF[all_edges];
                m_e2vf_list.reserve(num_faces * 2);
            }
            catch (std::bad_alloc)
            {
                return false;
            }

            int iter = 0;

            for (int fIdx = 0; fIdx < num_faces; ++fIdx)
            {
                const niTriFace2d &f = trimesh.m_faces[fIdx];
                edgebuffs[iter++].Create(f.aIdx, f.bIdx, fIdx);
                edgebuffs[iter++].Create(f.bIdx, f.cIdx, fIdx);
                edgebuffs[iter++].Create(f.cIdx, f.aIdx, fIdx);
            }

            std::sort(&edgebuffs[0], &edgebuffs[all_edges], niEdge2VF::_sort_);

            iter = 0;
            m_e2vf_list.push_back(edgebuffs[0]);
            for (int i = 1; i < all_edges; ++i)
            {
                if (m_e2vf_list[iter] == edgebuffs[i])
                {
                    if (edgebuffs[i].m_e2f[0] != -1)
                        m_e2vf_list[iter].m_e2f[0] = edgebuffs[i].m_e2f[0];
                    if (edgebuffs[i].m_e2f[1] != -1)
                        m_e2vf_list[iter].m_e2f[1] = edgebuffs[i].m_e2f[1];
                }
                else
                {
                    m_e2vf_list.push_back(edgebuffs[i]);
                    ++iter;
                }
            }

            delete []edgebuffs;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildFace2VE
        //-----------------------------------------------------------------------------
        /**
        * Build face topology of triangle mesh
        *
        * @param       trimesh:     input triangle mesh
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dTopo::BuildFace2VE(const niTriMesh2d& trimesh)
        {
            int num_edges = int(m_e2vf_list.size());
            int num_faces = int(m_f2ve_list.size());

            for (int fIdx = 0; fIdx < num_faces; ++fIdx)
            {
                const niTriFace2d &f = trimesh.m_faces[fIdx];
                m_f2ve_list[fIdx].m_f2v[0] = f.aIdx;
                m_f2ve_list[fIdx].m_f2v[1] = f.bIdx;
                m_f2ve_list[fIdx].m_f2v[2] = f.cIdx;
            }

            for (int eIdx = 0; eIdx < num_edges; ++eIdx)
            {
                niEdge2VF &e = m_e2vf_list[eIdx];
                int fIdx = e.m_e2f[0];
                if (-1 != fIdx)
                {
                    if (-1 == m_f2ve_list[fIdx].m_f2e[0])
                        m_f2ve_list[fIdx].m_f2e[0] = eIdx;
                    else if (-1 == m_f2ve_list[fIdx].m_f2e[1])
                        m_f2ve_list[fIdx].m_f2e[1] = eIdx;
                    else if (-1 == m_f2ve_list[fIdx].m_f2e[2])
                        m_f2ve_list[fIdx].m_f2e[2] = eIdx;
                }

                fIdx = e.m_e2f[1];
                if (-1 != fIdx)
                {
                    if (-1 == m_f2ve_list[fIdx].m_f2e[0])
                        m_f2ve_list[fIdx].m_f2e[0] = eIdx;
                    else if (-1 == m_f2ve_list[fIdx].m_f2e[1])
                        m_f2ve_list[fIdx].m_f2e[1] = eIdx;
                    else if (-1 == m_f2ve_list[fIdx].m_f2e[2])
                        m_f2ve_list[fIdx].m_f2e[2] = eIdx;
                }
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildVert2EF
        //-----------------------------------------------------------------------------
        /**
        * Build vert topology of triangle mesh
        *
        * @param       trimesh:     input triangle mesh
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dTopo::BuildVert2EF(const niTriMesh2d& trimesh)
        {
            int num_edges = int(m_e2vf_list.size());
            int num_faces = int(m_f2ve_list.size());

            for (int fIdx = 0; fIdx < num_faces; ++fIdx)
            {
                const niTriFace2d &f = trimesh.m_faces[fIdx];
                m_v2ef_list[f.aIdx].m_v2f.push_back(fIdx);
                m_v2ef_list[f.bIdx].m_v2f.push_back(fIdx);
                m_v2ef_list[f.cIdx].m_v2f.push_back(fIdx);
            }

            for (int eIdx = 0; eIdx < num_edges; ++eIdx)
            {
                niEdge2VF &e = m_e2vf_list[eIdx];
                m_v2ef_list[e.m_e2v[0]].m_v2e.push_back(eIdx);
                m_v2ef_list[e.m_e2v[1]].m_v2e.push_back(eIdx);
            }
            return true;
        }
        //-----------------------------------------------------------------------------
        // FUNCTION Destory
        //-----------------------------------------------------------------------------
        /**
        * Clear
        */
        bool niTriMesh2dTopo::Destory()
        {
            m_v2ef_list.clear();
            m_e2vf_list.clear();
            m_f2ve_list.clear();
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION FindEdge
        //-----------------------------------------------------------------------------
        /**
        * Get edge index
        *
        * @param       f2ve:        face contains verts' indices and edges' indices
        * @param       v1:          vert id
        * @param       v2:          vert id
        * @return      -1:          not found edge
        *              >=0:         edge id
        */
        int niTriMesh2dTopo::FindEdge(niFace2VE &f2ve, int v1, int v2)
        {
            int eIdx;
            for (int i = 0; i < 3; ++i)
            {
                eIdx = f2ve.m_f2e[i];
                niEdge2VF &e2vf = m_e2vf_list[eIdx];
                if (e2vf.IsEdgeEqual(v1, v2))
                    return eIdx;
            }
            return -1;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION RemoveFace
        //-----------------------------------------------------------------------------
        /**
        * Remove a face from tiangle mesh topology
        *
        * @param       f:           face id need to delete
        * @return      true:        delete success
        *              false:       delete failed
        */
        bool niTriMesh2dTopo::RemoveFace(int f)
        {
            if (f < 0 || f >= int(m_f2ve_list.size()))
                return false;

            niFace2VE &f2ve = m_f2ve_list[f];
            if (f2ve.m_f2v[0] == -1)
                return false;

            int v, e;
            for (int i = 0; i < 3; ++i)
            {
                v = f2ve.m_f2v[i];
                m_v2ef_list[v].RemoveFace(f);
            }

            for (int i = 0; i < 3; ++i)
            {
                e = f2ve.m_f2e[i];
                niEdge2VF &e2vf = m_e2vf_list[e];
                e2vf.RemoveFace(f);
                if (e2vf.NumFaces() == 0)
                {
                    v = e2vf.m_e2v[0];
                    m_v2ef_list[v].RemoveEdge(e);
                    v = e2vf.m_e2v[1];
                    m_v2ef_list[v].RemoveEdge(e);
                    e2vf.m_e2v[0] = e2vf.m_e2v[1] = -1;
                }
            }
            f2ve.Reset();
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION RemoveFaces
        //-----------------------------------------------------------------------------
        /**
        * Remove faces from tiangle mesh topology
        *
        * @param       fs:          faces' id need to delete
        * @return      true:        delete success
        *              false:       delete failed
        */
        bool niTriMesh2dTopo::RemoveFaces(niIntArray fs)
        {
            int num = int(fs.size());
            if (num < 1)
                return false;

            for (int i = 0; i < num; ++i)
            {
                RemoveFace(fs[i]);
            }
            return true;
        }
    }
}
