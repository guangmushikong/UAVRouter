#ifndef dsPolyMesh3dTopo_H
#define dsPolyMesh3dTopo_H

#include <geometry/dsDef.h>
#include <vector>

namespace ds
{
    namespace geometry
    {
        class dsPolyMesh3d;
        /**
         * \brief Topology of 3d polygon mesh
         *
         */
        class dsPolyMesh3dTopo
        {
        public:
            /**
             * \brief Topology of vert
             *
             */
            struct dsVert2EF
            {
                inline void         AddEdge             (int e)
                {
                    m_v2e.push_back(e);
                }
                inline void         AddFace             (int f)
                {
                    m_v2f.push_back(f);
                }

                inline void         RemoveEdge          (int e)
                {
                    std::vector<int>::iterator iter;
                    for (iter = m_v2e.begin(); iter != m_v2e.end(); ++iter)
                    {
                        if (*iter == e)
                        {
                            m_v2e.erase(iter);
                            break;
                        }
                    }
                }
                inline void         RemoveFace          (int f)
                {
                    std::vector<int>::iterator iter;
                    for (iter = m_v2f.begin(); iter != m_v2f.end(); ++iter)
                    {
                        if (*iter == f)
                        {
                            m_v2f.erase(iter);
                            break;
                        }
                    }
                }
                std::vector<int>    m_v2e;
                std::vector<int>    m_v2f;
            };

            /**
             * \brief Topology of edge
             *
             */
            struct dsEdge2VF
            {
                dsEdge2VF           ()
                {
                    m_e2v[0] = m_e2v[1] = -1;
                    m_e2f[0] = m_e2f[1] = -1;
                }
                inline void         Create              (int v1, int v2, int f)
                {
                    if (v1 < v2)
                    {
                        m_e2v[0] = v1;
                        m_e2v[1] = v2;
                        m_e2f[0] = f;
                    }
                    else
                    {
                        m_e2v[0] = v2;
                        m_e2v[1] = v1;
                        m_e2f[1] = f;
                    }
                }
                inline int          GetFace             (int v1, int v2)
                {
                    if (v1 < v2)
                    {
                        if (v1 == m_e2v[0] && v2 == m_e2v[1])
                            return m_e2f[0];
                    }
                    else
                    {
                        if (v1 == m_e2v[1] && v2 == m_e2v[0])
                            return m_e2f[1];
                    }
                    return -1;
                }

                inline bool         SetFace             (int v1, int v2, int f)
                {
                    if (v1 < v2)
                    {
                        if (v1 == m_e2v[0] && v2 == m_e2v[1])
                        {
                            m_e2f[0] = f;
                            return true;
                        }
                    }
                    else
                    {
                        if (v1 == m_e2v[1] && v2 == m_e2v[0])
                        {
                            m_e2f[1] = f;
                            return true;
                        }
                    }
                    return false;
                }
                inline bool         IsEdgeEqual         (int v1, int v2)
                {
                    if (v1 < v2)
                    {
                        if (v1 != m_e2v[0])
                            return false;
                        return v2 == m_e2v[1];
                    }
                    else
                    {
                        if (v2 != m_e2v[0])
                            return false;
                        return v1 == m_e2v[1];
                    }
                }
                inline int          NumFaces            ()
                {
                    int n = 0;
                    if (m_e2f[0] >= 0)
                        ++n;
                    if (m_e2f[1] >= 0)
                        ++n;
                    return n;
                }
                inline bool         operator ==         (const dsEdge2VF &e2vf)
                {
                    if (e2vf.m_e2v[0] !=  m_e2v[0])
                        return false;
                    return e2vf.m_e2v[1] ==  m_e2v[1];
                }
                inline void         RemoveFace          (int f)
                {
                    if (m_e2f[0] == f)
                        m_e2f[0] = -1;
                    else if (m_e2f[1] == f)
                        m_e2f[1] = -1;
                }

                static bool         _sort_              (const dsEdge2VF &e1, const dsEdge2VF &e2)
                {
                    if (e1.m_e2v[0] < e2.m_e2v[0])
                        return true;
                    if (e1.m_e2v[0] > e2.m_e2v[0])
                        return false;
                    return e1.m_e2v[1] < e2.m_e2v[1];
                }

                int                 m_e2v[2];
                int                 m_e2f[2];
            };
            /**
             * \brief Topology of face
             *
             */
            struct dsFace2VE
            {
                dsFace2VE           ()
                {
                    m_f2v[0] = m_f2v[1] = m_f2v[2] = -1;
                    m_f2e[0] = m_f2e[1] = m_f2e[2] = -1;
                }
                inline bool         GetVerts            (int aIdx, int &bIdx, int &cIdx)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        if (aIdx == m_f2v[i])
                        {
                            bIdx = m_f2v[(i+1)%3];
                            cIdx = m_f2v[(i+2)%3];
                            return true;
                        }
                    }
                    return false;
                }

                inline void         Reset               ()
                {
                    m_f2v[0] = m_f2v[1] = m_f2v[2] = -1;
                    m_f2e[0] = m_f2e[1] = m_f2e[2] = -1;
                }

                int                 m_f2v[3];
                int                 m_f2e[3];
            };

        public:
            dsPolyMesh3dTopo         (){}

            dsPolyMesh3dTopo         (const dsPolyMesh3dTopo &topo_mesh);

            ~dsPolyMesh3dTopo        ();

            dsPolyMesh3dTopo&        operator=           (const dsPolyMesh3dTopo &topo_mesh);

            bool                    BuildTopology       (const dsPolyMesh3d& trimesh, bool fBuildVert);

            bool                    IsValid             () const
            {
                return m_editor_valid;
            }

            int                     FindEdge            (dsFace2VE &f2ve, int v1, int v2);

            bool                    RemoveFace          (int f);

            bool                    RemoveFaces         (std::vector<int> fs);

        protected:

            bool                    BuildEdge2VF        (const dsPolyMesh3d& trimesh);
            bool                    BuildFace2VE        (const dsPolyMesh3d& trimesh);
            bool                    BuildVert2EF        (const dsPolyMesh3d& trimesh);

            bool                    Destory             ();

            bool                    m_editor_valid;
        public:
            std::vector<dsVert2EF>  m_v2ef_list;
            std::vector<dsEdge2VF>  m_e2vf_list;
            std::vector<dsFace2VE>  m_f2ve_list;

        };
    }
}
#endif
