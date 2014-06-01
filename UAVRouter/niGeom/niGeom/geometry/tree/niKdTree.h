//! \file
// \brief
// The kd-tree class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niKdTree_H
#define niKdTree_H

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
             * \brief KD-tree node.
             *
             */
            class niKdTree
            {
            public:
                /**
                 * \brief KD-tree node.
                 *
                 */
                class niKdNode
                {
                    friend class niKdTree;
                public:
                    niKdNode            (const niPoint2d &point, int IdOfPoint, int axis)
                        : m_refPoint2d(&point, IdOfPoint),
                        m_axis(axis),
                        l_child(NULL),
                        r_child(NULL)
                    {}

                    niKdNode            (const niPoint2d *pointRef, int IdOfPoint, int axis)
                        : m_refPoint2d(pointRef, IdOfPoint),
                        m_axis(axis),
                        l_child(NULL),
                        r_child(NULL)
                    {}

                    niKdNode            (const niRefPoint2d &refPoint2d, int axis)
                        : m_refPoint2d(refPoint2d),
                        m_axis(axis),
                        l_child(NULL),
                        r_child(NULL)
                    {}

                protected:
                    ~niKdNode           ()
                    {
                        if (NULL != l_child)
                            delete l_child;
                        if (NULL != r_child)
                            delete r_child;
                    }

                public:
                    const niRefPoint2d  m_refPoint2d;
                    int                 m_axis;
                    niKdNode*           l_child;
                    niKdNode*           r_child;
                };

            protected:
                /**
                 * \brief KD-tree build node, used to build banlanced kd tree
                 *
                 */
                class niBuildNode
                {
                public:
                    niBuildNode         () : m_hit(false){}

                    niRefPoint2d        m_refPoint2d;
                    bool                m_hit;

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
                 * \brief Build a banlanced kd tree.
                 *
                 */
                class niBuilder
                {
                public:
                    niBuilder           ();

                    ~niBuilder          ();

                    niKdNode*           Build           (int axis1, int _l, int _r);

                    bool                InitBuild       (const niArrayT<const niPoint2d*> &pointRefs);

                    bool                InitBuild       (const niArrayT<niRefPoint2d> &refPoints);

                    static bool         IsHit           (niBuildNode *node)
                    {
                        return node->m_hit;
                    }

                protected:
                    void                Destory         ();


                protected:
                    niBuildNode*        m_build_nodes;
                    niBuildNode**       m_sorted[2];
                };

            public:
                niKdTree                () : m_root(NULL){}

                niKdTree                (const niArrayT<niRefPoint2d> &refPoints);

                ~niKdTree               ();

                bool                    Build           (const niPoint2dArray &points);

                bool                    Build           (const niArrayT<const niPoint2d*> &pointRefs);

                bool                    Build           (const niArrayT<niRefPoint2d> &refPoints);

                bool                    PointsInBBox    (
                                                        const niBBox2d &bbox,
                                                        niArrayT<const niPoint2d*> &pointRefs);

                bool                    PointsInBBox    (
                                                        const niBBox2d &bbox,
                                                        niIntArray &IdOfPoints);

                const niKdNode*         Root            () const
                {
                    return m_root;
                }

            protected:
                niKdNode*               m_root;
            };
        }
    }
}

#endif