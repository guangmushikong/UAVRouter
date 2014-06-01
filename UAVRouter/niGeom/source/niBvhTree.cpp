//! \file
// \brief
// Bounding Volume Hierarchies tree
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#include <niGeom/geometry/tree/niBvhTree.h>

#include <iostream>
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
            niBvhTree::niBuilder::niBuilder()
            {
                m_build_nodes       = NULL;
                m_sorted[0]         = NULL;
                m_sorted[1]         = NULL;
                m_cost_from_cache   = NULL;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION DeConstruction
            //-----------------------------------------------------------------------------
            /**
            * DeConstruction
            *
            */
            niBvhTree::niBuilder::~niBuilder()
            {
                Destory();
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build bsp 
            * @param            _l:                 start position
            * @param            _r:                 end position
            * @return           bsp node
            *
            */
            niBvhTree::niBvhNode* niBvhTree::niBuilder::Build(int _l, int _r)
            {
                if (_l == _r)
                {
                    niBvhLeaf *leaf = new niBvhLeaf;
                    leaf->m_bbox    = m_sorted[0][_l]->m_bbox;
                    leaf->m_refGeom = m_sorted[0][_l]->m_refGeom;
                    return leaf;
                }
                else if (_l < _r)
                {
                    niBvhNode *node = new niBvhNode;
                    CalcBBox(_l, _r, node->m_bbox);
                    int _axis, _separator;
                    bool bStat = FindBestSplit(_l, _r, _axis, _separator);
                    if (!bStat)
                    {
                        std::cout<<"dsBvhTree::niBuilder::Build FindBestSplit Error1!"<<std::endl;
                    }
                    Split(_l, _r, _axis, _separator);

                    node->l_child = Build(_l, _separator);
                    node->r_child = Build(_separator+1, _r);
                    return node;
                }
                else
                {
                    std::cout<<"dsBvhTree::niBuilder::Build Error1!"<<std::endl;
                    return NULL;
                }
            }

            //-----------------------------------------------------------------------------
            // FUNCTION CalcBBox
            //-----------------------------------------------------------------------------
            /**
            * CalcBBox of elements between [_l, _r]
            * @param            _l:                 start position
            * @param            _r:                 end position
            * @param            bbox:               bbox
            *
            */
            void niBvhTree::niBuilder::CalcBBox(int _l, int _r, niBBox2d &bbox)
            {
                bbox = m_sorted[0][_l]->m_bbox;
                for (int i = _l+1; i <= _r; ++i)
                {
                    bbox.Append(m_sorted[0][i]->m_bbox);
                }
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Destory
            //-----------------------------------------------------------------------------
            /**
            * Clear memory
            *
            */
            void niBvhTree::niBuilder::Destory()
            {
                if (NULL != m_build_nodes)
                {
                    delete []m_build_nodes;
                    m_build_nodes = NULL;
                }
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
                if (NULL != m_cost_from_cache)
                {
                    delete []m_cost_from_cache;
                    m_cost_from_cache = NULL;
                }
            }

            //-----------------------------------------------------------------------------
            // FUNCTION FindBestSplit
            //-----------------------------------------------------------------------------
            /**
            * Find best split line to split element into two nodes.
            * First, minize the area of the two splited nodes
            * Second, balance the depth of children nodes.
            *
            * @param            _l:                 start position
            * @param            _r:                 end position
            * @param            _axis:              split axis
            * @param            _separator:         split position
            * @return           true:               success
            *                   false:              failed
            *
            */
            bool niBvhTree::niBuilder::FindBestSplit(int _l, int _r, int &_axis, int &_separator)
            {
                _axis = -1;
                _separator = -1;

                const int num_nodes = _r - _l + 1;
                if (num_nodes < 2)
                    return false;

                if (_l+1 == _r)
                {
                    _axis = 0;
                    _separator = _l;
                    return true;
                }

                double coef = 1;
                double min_cost = (double)1e300;
                double max_cost = (double)-1e300;

                _axis = -1, _separator = (_l + _r) / 2;

                for (int axis = 0; axis < 2; ++axis)
                {
                    niBBoxCost cost_from_left;
                    cost_from_left.m_bbox = m_sorted[axis][_l]->m_bbox;
                    cost_from_left.m_cost = 1;

                    m_cost_from_cache[_r].m_bbox = m_sorted[axis][_r]->m_bbox;
                    m_cost_from_cache[_r].m_cost = 1;

                    for (int i = _r-1; i > _l; --i)
                    {
                        niBBoxCost &curr = m_cost_from_cache[i];
                        niBBoxCost &last = m_cost_from_cache[i+1];

                        curr.m_bbox = last.m_bbox;
                        curr.m_bbox.Append( m_sorted[axis][i]->m_bbox );

                        curr.m_cost = last.m_cost + 1;
                    }

                    for (int i = _l+1; i < _r; ++i)
                    {
                        cost_from_left.m_bbox.Append( m_sorted[axis][i]->m_bbox );
                        cost_from_left.m_cost += 1;

                        double cost_left, cost_right, cost;
                        cost_left = cost_from_left.m_bbox.Area() * (cost_from_left.m_cost + log((double)(i-_l)) * coef);

                        niBBoxCost& cost_from_right =m_cost_from_cache[i];
                        cost_right = cost_from_right.m_bbox.Area() * (cost_from_right.m_cost + log((double)(_r-i+1)) * coef);
                        
                        cost = cost_left + cost_right;

                        if (cost < min_cost)
                        {
                            min_cost = cost;
                            _axis = axis;
                            _separator = i;
                        }
                        if (max_cost < cost)
                        {
                            max_cost = cost;
                        }
                    }
                    if (-1 == _axis)
                    {
                        _axis = 0;
                        _separator = (_l + _r) / 2;
                    }
                }
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION InitBuild
            //-----------------------------------------------------------------------------
            /**
            * Init build
            *
            * @param            refGeomes:          geometry references
            * @param            bboxRefs:           boxes references
            * @return           true:               success
            *                   false:              failed
            *
            */
            bool niBvhTree::niBuilder::InitBuild(
                const niArrayT<niRefGeom2d> &refGeomes,
                const niArrayT<const niBBox2d*> &bboxRefs)
            {
                size_t numGeoms = refGeomes.size();
                try
                {
                    m_build_nodes       = new niBuildNode[numGeoms];
                    m_sorted[0]         = new niBuildNode*[numGeoms];
                    m_sorted[1]         = new niBuildNode*[numGeoms];
                    m_cost_from_cache   = new niBBoxCost[numGeoms];
                }
                catch (std::bad_alloc)
                {
                    Destory();
                    return false;
                }

                for (size_t i = 0; i < numGeoms; ++i)
                {
                    niBuildNode &buildNode  = m_build_nodes[i];
                    buildNode.m_refGeom     = refGeomes[i];
                    buildNode.m_bbox        = *bboxRefs[i];
                    buildNode.m_hit         = false;

                    m_sorted[0][i]          = &buildNode;
                    m_sorted[1][i]          = &buildNode;
                }

                std::sort(m_sorted[0], &m_sorted[0][numGeoms], niBuildNode::_compare_<0>);
                std::sort(m_sorted[1], &m_sorted[1][numGeoms], niBuildNode::_compare_<1>);
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION InitBuild
            //-----------------------------------------------------------------------------
            /**
            * Init build
            *
            * @param            refGeomes:          geometry references
            * @param            bboxRefs:           boxes references
            * @return           true:               success
            *                   false:              failed
            *
            */
            bool niBvhTree::niBuilder::InitBuild(
                const niArrayT<niRefGeom2d> &refGeomes,
                const niArrayT<niBBox2d> &bboxes)
            {
                size_t numGeoms = refGeomes.size();
                try
                {
                    m_build_nodes       = new niBuildNode[numGeoms];
                    m_sorted[0]         = new niBuildNode*[numGeoms];
                    m_sorted[1]         = new niBuildNode*[numGeoms];
                    m_cost_from_cache   = new niBBoxCost[numGeoms];
                }
                catch (std::bad_alloc)
                {
                    Destory();
                    return false;
                }

                for (size_t i = 0; i < numGeoms; ++i)
                {
                    niBuildNode &buildNode  = m_build_nodes[i];
                    buildNode.m_refGeom     = refGeomes[i];
                    buildNode.m_bbox        = bboxes[i];
                    buildNode.m_hit         = false;

                    m_sorted[0][i]          = &buildNode;
                    m_sorted[1][i]          = &buildNode;
                }

                std::sort(m_sorted[0], &m_sorted[0][numGeoms], niBuildNode::_compare_<0>);
                std::sort(m_sorted[1], &m_sorted[1][numGeoms], niBuildNode::_compare_<1>);
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Split
            //-----------------------------------------------------------------------------
            /**
            * Split
            *
            * @param            _l:                 start position
            * @param            _r:                 end position
            * @param            _axis:              split axis
            * @param            _separator:         split position
            *
            */
            void niBvhTree::niBuilder::Split(int _l, int _r, int _axis, int _separator)
            {
                for (int i = _l; i <= _r; ++i)
                {
                    m_sorted[_axis][i]->m_hit = (i <= _separator);
                }

                _axis = (_axis+1) % 2;

                std::stable_partition(&m_sorted[_axis][_l], &m_sorted[_axis][_r+1], IsHit);
            }
            
            //-----------------------------------------------------------------------------
            // FUNCTION Construction
            //-----------------------------------------------------------------------------
            /**
            * Construction
            *
            * @param            refGeomes:          geometry references
            * @param            bboxRefs:           boxes references
            *
            */
            niBvhTree::niBvhTree(
                const niArrayT<niRefGeom2d> &refGeomes,
                const niArrayT<const niBBox2d*> &bboxRefs) : m_root(NULL)
            {
                Build(refGeomes, bboxRefs);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Construction
            //-----------------------------------------------------------------------------
            /**
            * Construction
            *
            * @param            refGeomes:          geometry references
            * @param            bboxRefs:           boxes references
            *
            */
            niBvhTree::niBvhTree(
                const niArrayT<niRefGeom2d> &refGeomes,
                const niArrayT<niBBox2d> &boxes) : m_root(NULL)
            {
                Build(refGeomes, boxes);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION DeConstruction
            //-----------------------------------------------------------------------------
            /**
            * DeConstruction
            *
            */
            niBvhTree::~niBvhTree()
            {
                _Destory();
            }
            
            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build
            *
            * @param            refGeomes:          geometry references
            * @param            bboxRefs:           boxes references
            * @return           true:               success
            *                   false:              failed
            */
            bool niBvhTree::Build(
                const niArrayT<niRefGeom2d> &refGeomes,
                const niArrayT<const niBBox2d*> &bboxRefs)
            {
                _Destory();

                size_t numGeomes = refGeomes.size();
                if (numGeomes < 1)
                    return false;

                niBuilder build;

                bool bStat = build.InitBuild(refGeomes, bboxRefs);
                if (!bStat)
                    return false;

                m_root = build.Build(0, numGeomes-1);

                return NULL != m_root;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build
            *
            * @param            refGeomes:          geometry references
            * @param            bboxRefs:           boxes references
            * @return           true:               success
            *                   false:              failed
            */
            bool niBvhTree::Build(
                const niArrayT<niRefGeom2d> &refGeomes,
                const niArrayT<niBBox2d> &bboxes)
            {
                _Destory();

                size_t numGeomes = refGeomes.size();
                if (numGeomes < 1)
                    return false;

                niBuilder build;

                bool bStat = build.InitBuild(refGeomes, bboxes);
                if (!bStat)
                    return false;

                m_root = build.Build(0, numGeomes-1);

                return NULL != m_root;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION StatTool
            //-----------------------------------------------------------------------------
            /**
            * Used to check efficiency of the bsp tree.
            * Compare the real depth and theoretical depth of the bsp tree.
            *
            */
            int niBvhTree::StatTool()
            {
                int maxDepth    = 0;
                int minDepth    = 0;
                int counter     = 0;

                std::stack<niBvhNode*>  ns;
                std::stack<int>         ds;

                niBvhNode* iter = m_root;
                int deep        = 0;

                while (iter)
                {
                    ns.push(iter);
                    ds.push(deep++);
                    iter = iter->l_child;
                }

                while (!ns.empty())
                {
                    iter = ns.top();    ns.pop();   
                    deep = ds.top();    ds.pop();

                    maxDepth = maxDepth > deep ? maxDepth : deep;
                    ++counter;
                    if (iter->r_child)
                    {
                        deep++;
                        iter = iter->r_child;
                        while (iter)
                        {
                            ns.push(iter);
                            ds.push(deep++);
                            iter = iter->l_child;
                        }
                    }
                }

                double depth = log( (double)counter ) / log(2.0);
                minDepth = (int)depth;

                return maxDepth - minDepth;
            }
            void niBvhTree::_Destory()
            {
                if (NULL != m_root)
                {
                    delete m_root;
                    m_root = NULL;
                }
            }
        }
    }
}