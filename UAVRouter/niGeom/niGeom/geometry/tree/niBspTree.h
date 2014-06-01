//! \file
// \brief
// bsp tree
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niBspTree_H
#define niBspTree_H

#include <algorithm>

#include <niGeom/geometry/niGeom2dTypes.h>
#include <niGeom/geometry/niBBox2d.h>
#include <niGeom/geometry/tree/niTreeTypes.h>

namespace ni
{
    namespace geometry
    {
        namespace tree
        {
            /**
             * \brief bsp tree
             *
             */
            class niBspTree
            {
            public:
                /**
                 * \brief node of bsp tree
                 *
                 */
                class niBspNode
                {
                public:
                    niBspNode           (bool isLeaf = false)
                        : l_child(NULL), r_child(NULL), m_isLeaf(isLeaf)
                    {
                    }

                    niBspNode           (const niBBox2d &bbox, bool isLeaf = false)
                        : m_bbox(bbox), l_child(NULL), r_child(NULL), m_isLeaf(isLeaf)
                    {
                    }

                    virtual ~niBspNode  ()
                    {
                        if (NULL != l_child)
                        {
                            delete l_child;
                            l_child = NULL;
                        }
                        if (NULL != r_child)
                        {
                            delete r_child;
                            r_child = NULL;
                        }
                    }

                    niBBox2d            m_bbox;
                    niBspNode*          l_child;
                    niBspNode*          r_child;

                    inline bool         IsLeaf                  () const
                    {
                        return m_isLeaf;
                    }

                protected:
                    const bool          m_isLeaf;
                };

                /**
                 * \brief leaf of bsp tree
                 *
                 */
                class niBspLeaf : public niBspNode
                {
                public:
                    niBspLeaf           () : niBspNode(true)
                    {
                    }
                    niBspLeaf           (const niBBox2d &bbox) : niBspNode(bbox, true)
                    {
                    }
                    ~niBspLeaf          ()
                    {
                    }

                    niArrayT<niRefPoint2d>  m_refPoints;
                };

            protected:
                /**
                 * \brief node used to build bsp tree
                 *
                 */
                class niBuildNode
                {
                public:
                    niBuildNode         () : m_hit(false){}

                    niRefPoint2d        m_refPoint2d;
                    bool                m_hit;
                    niBBox2d            m_bbox;

                    template<int axis_t>
                    static bool         _compare_           (const niBuildNode* n1, const niBuildNode *n2)
                    {
                        double _a, _b;
                        switch (axis_t)
                        {
                        case 0:
                            _a = n1->m_refPoint2d.m_pointRef->X;
                            _b = n2->m_refPoint2d.m_pointRef->X;
                            break;
                        default:
                            _a = n1->m_refPoint2d.m_pointRef->Y;
                            _b = n2->m_refPoint2d.m_pointRef->Y;
                            break;
                        }
                        if (_a < _b)
                            return true;
                        if (_a > _b)
                            return false;
                        return n1->m_refPoint2d.m_IdOfPoint < n2->m_refPoint2d.m_IdOfPoint;
                    }
                };

                /**
                 * \brief bsp tree builder
                 *
                 */
                class niBuilder
                {
                public:
                    niBuilder           ();

                    ~niBuilder          ();

                    bool                InitBuild               (const niArrayT<const niPoint2d*> &pointRefs);

                    bool                InitBuild               (const niArrayT<niRefPoint2d> &refPoints);

                    niBspNode*          Build                   (int threshold);

                protected:

                    niBspNode*          Build                   (int _l, int _r, int threshold);

                    void                CalcBBox                (int _l, int _r, niBBox2d &bbox);

                    void                Destory                 ();
                    
                    bool                FindBestSplit           (int _l, int _r, int _axis, int &_separator);

                    void                Split                   (int _l, int _r, int _axis, int _separator);

                    static bool         IsHit                   (niBuildNode *node)
                    {
                        if (NULL != node)
                            return node->m_hit;
                        return false;
                    }

                    int                 m_size;
                    niBuildNode*        m_build_nodes;
                    niBuildNode**       m_sorted[2];
                    niBBoxCost*         m_cost_from_cache;
                };

            public:
                niBspTree               (int threshold = 16);

                niBspTree               (const niArrayT<niRefPoint2d> &refPoints, int threshold = 16);

                ~niBspTree              ();
                
                bool                    Build                   (const niArrayT<const niPoint2d*> &pointRefs);

                bool                    Build                   (const niPoint2dArray &points);

                bool                    Build                   (const niArrayT<niRefPoint2d> &refPoints);

                void                    DumpToMIF               (const String &file, int color);

                const niBspNode*        Root                    () const
                {
                    return m_root;
                }

                int                     StatTool                ();

            protected:
                void                    _Destory                ();

            protected:
                niBspNode*              m_root;
                const int               m_cThreshold;
            };
        }
    }
}

#endif
