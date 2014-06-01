//! \file
// \brief
// Define enum types of geometry
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niGeomDef_H
#define niGeomDef_H

#include <niGeom/niTypes.h>

namespace ni
{
    namespace geometry
    {
        enum EOrientation{
            eUndefined              = 0,
            eClockwise              = 1 << 0,
            eCounterClockwise       = 1 << 1
        };

        enum ETriangleType{
            eTriangleFan            = 1 << 0,
            eTriangleStrip          = 1 << 1,
            eTriangleSimple         = 1 << 2
        };

        enum EIntersectType{
            eNotIntersect           = 0,
            eIntersectOverlap       = 1 << 0,
            eIntersectConnect       = 1 << 1,
            eIntersectLeftHand      = 1 << 2,
            eIntersectRightHand     = 1 << 3,
            eIntersect              = eIntersectLeftHand ^ eIntersectRightHand
        };

        enum EPointInGeometry{
            eOutGeometry            = 0,
            eInGeometry             = 1 << 0,
            eOnBorder               = 1 << 1,
            eOnPoint                = 1 << 2,
            eErrorGeometry          = 1 << 3
        };

        /**
         * \brief Topology of vert
         *
         */
        struct niVert2EF
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
                niIntArray::iterator iter;
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
                niIntArray::iterator iter;
                for (iter = m_v2f.begin(); iter != m_v2f.end(); ++iter)
                {
                    if (*iter == f)
                    {
                        m_v2f.erase(iter);
                        break;
                    }
                }
            }
            niIntArray    m_v2e;
            niIntArray    m_v2f;
        };

        /**
         * \brief Topology of edge
         *
         */
        struct niEdge2VF
        {
            niEdge2VF           ()
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
            inline bool         operator ==         (const niEdge2VF &e2vf)
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

            static bool         _sort_              (const niEdge2VF &e1, const niEdge2VF &e2)
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
        struct niFace2VE
        {
            niFace2VE           ()
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
        typedef niArrayT<niVert2EF> niVert2efArray;
        typedef niArrayT<niEdge2VF> niEdge2vfArray;
        typedef niArrayT<niFace2VE> niFace2veArray;
    }
}
#endif
