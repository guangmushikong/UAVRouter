//! \file
// \brief
// Bounding Volume Hierarchies tree
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niBvhTree_H
#define niBvhTree_H

#include <niGeom/niTypes.h>
#include <algorithm>
#include <niGeom/geometry/tree/niTreeTypes.h>

namespace ni
{
    namespace geometry
    {
        namespace tree
        {
            /**
             * \brief BVH tree
             *
             */
            class niBvhTree
            {
            public:
                /**
                 * \brief BVH node
                 *
                 */
                class niBvhNode
                {
                public:
                    niBvhNode           (bool isLeaf = false)
                        : l_child(NULL), r_child(NULL), m_isLeaf(isLeaf)
                    {
                    }

                    virtual ~niBvhNode  ()
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

                    inline bool         IsLeaf                  () const
                    {
                        return m_isLeaf;
                    }

                public:
                    niBBox2d            m_bbox;
                    niBvhNode*          l_child;
                    niBvhNode*          r_child;
                protected:
                    const bool          m_isLeaf;
                };

                /**
                 * \brief BVH Leaf
                 *
                 */
                class niBvhLeaf : public niBvhNode
                {
                public:
                    niBvhLeaf           () : niBvhNode(true)
                    {
                    }

                    ~niBvhLeaf          ()
                    {
                    }

                    niRefGeom2d         m_refGeom;
                };

            protected:
                /**
                 * \brief BVH-tree build node, used to build banlanced BVH tree
                 *
                 */
                class niBuildNode
                {
                public:
                    niBuildNode         () : m_hit(false){}
                    niRefGeom2d         m_refGeom;
                    bool                m_hit;
                    niBBox2d            m_bbox;

                    template<int axis_t>
                    static bool         _compare_               (const niBuildNode* n1, const niBuildNode* n2)
                    {
                        double _a, _b;
                        switch (axis_t)
                        {
                        case 0:
                            _a = n1->m_bbox.P1.X;
                            _b = n2->m_bbox.P1.X;
                            break;
                        case 1:
                            _a = n1->m_bbox.P1.Y;
                            _b = n2->m_bbox.P1.Y;
                            break;
                        }
                        if (_a != _b)
                            return _a < _b;

                        return n1->m_refGeom.m_IdOfGeom < n2->m_refGeom.m_IdOfGeom;
                    }
                };

                /**
                 * \brief Build banlanced bvh tree
                 *
                 */
                class niBuilder
                {
                public:
                    niBuilder           ();

                    ~niBuilder          ();

                    niBvhNode*          Build                   (int _l, int _r);

                    void                CalcBBox                (int _l, int _r, niBBox2d &bbox);

                    void                Destory                 ();

                    bool                FindBestSplit           (int _l, int _r, int &_axis, int &_separator);
                    
                    bool                InitBuild               (
                                                                const niArrayT<niRefGeom2d> &refGeomes,
                                                                const niArrayT<const niBBox2d*> &bboxRefs);

                    bool                InitBuild               (
                                                                const niArrayT<niRefGeom2d> &refGeomes,
                                                                const niArrayT<niBBox2d> &boxes);

                    void                Split                   (int _l, int _r, int _axis, int _separator);

                    static bool         IsHit                   (niBuildNode *node)
                    {
                        if (NULL != node)
                            return node->m_hit;
                        return false;
                    }

                    niBuildNode*        m_build_nodes;
                    niBuildNode**       m_sorted[2];
                    niBBoxCost*         m_cost_from_cache;
                };

            public:
                niBvhTree               () : m_root(NULL){}

                niBvhTree               (
                                        const niArrayT<niRefGeom2d> &refGeomes,
                                        const niArrayT<const niBBox2d*> &bboxRefs);

                niBvhTree               (
                                        const niArrayT<niRefGeom2d> &refGeomes,
                                        const niArrayT<niBBox2d> &boxes);

                ~niBvhTree              ();
                
                bool                    Build                   (
                                                                const niArrayT<niRefGeom2d> &refGeomes,
                                                                const niArrayT<const niBBox2d*> &bboxRefs);

                bool                    Build                   (
                                                                const niArrayT<niRefGeom2d> &refGeomes,
                                                                const niArrayT<niBBox2d> &boxes);

                int                     StatTool                ();

                const niBvhNode*        Root                    () const
                {
                    return m_root;
                }

            protected:
                void                    _Destory                ();

            protected:
                niBvhNode*              m_root;
            };
        }
    }
}

#endif
