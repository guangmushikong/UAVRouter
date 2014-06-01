//! \file
// \brief
// The kd-tree class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/tree/niKdTree.h>
#include <niGeom/geometry/niGeomMath2d.h>

#include <stack>

namespace ni
{
    namespace geometry
    {
        namespace tree
        {
            //-----------------------------------------------------------------------------
            // FUNCTION Construction
            //-----------------------------------------------------------------------------
            /**
            * Construction
            *
            */
            niKdTree::niBuilder::niBuilder()
            {
                m_build_nodes   = NULL;
                m_sorted[0]     = NULL;
                m_sorted[1]     = NULL;
            }
            //-----------------------------------------------------------------------------
            // FUNCTION DeConstruction
            //-----------------------------------------------------------------------------
            /**
            * DeConstruction
            *
            */
            niKdTree::niBuilder::~niBuilder()
            {
                Destory();
            }
            //-----------------------------------------------------------------------------
            // FUNCTION Destory
            //-----------------------------------------------------------------------------
            /**
            * Free all memory
            *
            */
            void niKdTree::niBuilder::Destory()
            {
                if (NULL != m_sorted[0])
                {
                    delete []m_sorted[0];
                    m_sorted[0] = NULL;
                }
                if (NULL != m_sorted[1])
                {
                    delete []m_sorted[1];
                    m_sorted[1] = NULL;
                }
                if (NULL != m_build_nodes)
                {
                    delete []m_build_nodes;
                    m_build_nodes = NULL;
                }
            }
            //-----------------------------------------------------------------------------
            // FUNCTION niBuilder::Build
            //-----------------------------------------------------------------------------
            /**
            * Build subtree iteratively.
            *
            * @param       axis1:       0 indicates x axis, 1 incidates y axis
            * @param       _l:          the start index of datas
            * @param       _r:          the finish index of datas
            * @return      if success, return the root of subtree,
            *                or return NULL
            */
            niKdTree::niKdNode* niKdTree::niBuilder::Build(
                int axis1, int _l, int _r)
            {
                int _m = (_l + _r) / 2;

                niKdNode* kd_node = new niKdNode(
                    m_sorted[axis1][_m]->m_refPoint2d,
                    axis1);

                if (_l == _r)
                {
                    return kd_node;
                }

                for (int i = _l; i <= _r; ++i)
                {
                    m_sorted[axis1][i]->m_hit = (i <= _m);
                }

                int axis2 = (axis1+1) % 2;

                // stable partition, maintain the order, so we do not need sort them any more.
                std::stable_partition(&m_sorted[axis2][_l], &m_sorted[axis2][_r+1], IsHit);
                m_sorted[axis1][_m]->m_hit = false; 
                std::stable_partition(&m_sorted[axis2][_l], &m_sorted[axis2][_m+1], IsHit);

                if (_l < _m)
                    kd_node->l_child = Build(axis2, _l, _m-1);
                if (_m < _r)
                    kd_node->r_child = Build(axis2, _m+1, _r);

                return kd_node;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niBuilder::InitBuild
            //-----------------------------------------------------------------------------
            /**
            * Init builder
            *
            * @param       points:      the input points
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::niBuilder::InitBuild(
                const niArrayT<const niPoint2d*> &pointRefs)
            {
                int num_points = int(pointRefs.size());
                if (num_points < 1)
                    return false;

                try
                {
                    m_build_nodes   = new niBuildNode[num_points];
                    m_sorted[0]     = new niBuildNode*[num_points];
                    m_sorted[1]     = new niBuildNode*[num_points];
                }
                catch (std::bad_alloc)
                {
                    Destory();
                    return false;
                }

                for (int i = 0; i < num_points; ++i)
                {
                    m_build_nodes[i].m_refPoint2d.m_pointRef = pointRefs[i];
                    m_build_nodes[i].m_refPoint2d.m_IdOfPoint = i;
                    m_sorted[0][i] = &m_build_nodes[i];
                    m_sorted[1][i] = &m_build_nodes[i];
                }

                std::sort(m_sorted[0], &m_sorted[0][num_points], niBuildNode::_compare_<0>);

                std::sort(m_sorted[1], &m_sorted[1][num_points], niBuildNode::_compare_<1>);

                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niBuilder::InitBuild
            //-----------------------------------------------------------------------------
            /**
            * Init builder
            *
            * @param       points:      the input points
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::niBuilder::InitBuild(
                const niArrayT<niRefPoint2d> &refPoints)
            {
                int num_points = int(refPoints.size());
                if (num_points < 1)
                    return false;

                try
                {
                    m_build_nodes   = new niBuildNode[num_points];
                    m_sorted[0]     = new niBuildNode*[num_points];
                    m_sorted[1]     = new niBuildNode*[num_points];
                }
                catch (std::bad_alloc)
                {
                    Destory();
                    return false;
                }

                for (int i = 0; i < num_points; ++i)
                {
                    m_build_nodes[i].m_refPoint2d = refPoints[i];
                    m_sorted[0][i] = &m_build_nodes[i];
                    m_sorted[1][i] = &m_build_nodes[i];
                }

                std::sort(m_sorted[0], &m_sorted[0][num_points], niBuildNode::_compare_<0>);

                std::sort(m_sorted[1], &m_sorted[1][num_points], niBuildNode::_compare_<1>);

                return true;
            }

            niKdTree::niKdTree(const niArrayT<niRefPoint2d> &refPoints) : m_root(NULL)
            {
                Build(refPoints);
            }

            niKdTree::~niKdTree()            
            {
                delete m_root;
                m_root = NULL;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niKdTree::Build
            //-----------------------------------------------------------------------------
            /**
            * Build subtree iteratively.
            *
            * @param       points:      the input points
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::Build(const niPoint2dArray &points)
            {
                size_t num_points = points.size();
                if (num_points < 1)
                    return false;

                niArrayT<const niPoint2d*> pointRefs;
                pointRefs.resize(num_points);

                for (size_t i = 0; i < num_points; ++i)
                {
                    pointRefs[i] = &points[i];
                }

                return Build(pointRefs);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niKdTree::Build
            //-----------------------------------------------------------------------------
            /**
            * Build subtree iteratively.
            *
            * @param       points:      the input points
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::Build(const niArrayT<const niPoint2d*> &pointRefs)
            {
                if (NULL != m_root)
                {
                    delete m_root;
                    m_root = NULL;
                }

                int num_points = int(pointRefs.size());
                if (num_points < 1)
                    return false;

                niBuilder builder;

                bool bStat = builder.InitBuild(pointRefs);
                if (!bStat)
                    return false;

                m_root = builder.Build(0, 0, num_points-1);

                return m_root != NULL;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niKdTree::Build
            //-----------------------------------------------------------------------------
            /**
            * Build subtree iteratively.
            *
            * @param       points:      the input points
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::Build(const niArrayT<niRefPoint2d> &refPoints)
            {
                if (NULL != m_root)
                {
                    delete m_root;
                    m_root = NULL;
                }

                int num_points = int(refPoints.size());
                if (num_points < 1)
                    return false;

                niBuilder builder;

                bool bStat = builder.InitBuild(refPoints);
                if (!bStat)
                    return false;

                m_root = builder.Build(0, 0, num_points-1);

                return m_root != NULL;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niKdTree::PointsInBBox
            //-----------------------------------------------------------------------------
            /**
            * Get points in bbox.
            *
            * @param       bbox:        the input points
            * @param       pointRefs:   save points in bbox
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::PointsInBBox(
                const niBBox2d &bbox,
                niArrayT<const niPoint2d *> &pointRefs)
            {
                pointRefs.clear();
                if (NULL == m_root)
                    return false;

                std::stack<niKdNode*> stack;
                stack.push(m_root);

                niKdNode* iter;
                double _v;

                while (!stack.empty())
                {
                    iter = stack.top();
                    stack.pop();

                    _v = (*iter->m_refPoint2d.m_pointRef)[iter->m_axis];

                    if (_v < bbox.P1[iter->m_axis])
                    {
                        if (NULL != iter->r_child)
                            stack.push(iter->r_child);
                    }
                    else if (_v > bbox.P2[iter->m_axis])
                    {
                        if (NULL != iter->l_child)
                            stack.push(iter->l_child);
                    }
                    else
                    {
                        if (niGeomMath2d::IsPointInBox(*iter->m_refPoint2d.m_pointRef, bbox))
                        {
                            pointRefs.push_back( iter->m_refPoint2d.m_pointRef );
                        }
                        if (NULL != iter->l_child)
                            stack.push(iter->l_child);
                        if (NULL != iter->r_child)
                            stack.push(iter->r_child);
                    }
                }
                return pointRefs.size() > 0;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION niKdTree::PointsInBBox
            //-----------------------------------------------------------------------------
            /**
            * Get points in bbox.
            *
            * @param       bbox:        the input points
            * @param       IdOfPoints:      save points' refids in bbox
            * @return      if success, return true
            *                or return false
            */
            bool niKdTree::PointsInBBox(
                const niBBox2d &bbox,
                niIntArray &IdOfPoints)
            {
                IdOfPoints.clear();
                if (NULL == m_root)
                    return false;

                std::stack<niKdNode*> stack;
                stack.push(m_root);

                niKdNode* iter;
                double _v;

                while (!stack.empty())
                {
                    iter = stack.top();
                    stack.pop();

                    _v = (*iter->m_refPoint2d.m_pointRef)[iter->m_axis];

                    if (_v < bbox.P1[iter->m_axis])
                    {
                        if (NULL != iter->r_child)
                            stack.push(iter->r_child);
                    }
                    else if (_v > bbox.P2[iter->m_axis])
                    {
                        if (NULL != iter->l_child)
                            stack.push(iter->l_child);
                    }
                    else
                    {
                        if (niGeomMath2d::IsPointInBox(*iter->m_refPoint2d.m_pointRef, bbox))
                        {
                            IdOfPoints.push_back(iter->m_refPoint2d.m_IdOfPoint);
                        }
                        if (NULL != iter->l_child)
                            stack.push(iter->l_child);
                        if (NULL != iter->r_child)
                            stack.push(iter->r_child);
                    }
                }
                return IdOfPoints.size() > 0;
            }
        }
    }
}