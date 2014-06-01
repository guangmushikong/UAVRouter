//! \file
// \brief
// bsp tree
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version


#include <niGeom/geometry/tree/niBspTree.h>

#include <iostream>
#include <fstream>
#include <stack>

#include <iomanip>

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
            niBspTree::niBuilder::niBuilder()
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
            niBspTree::niBuilder::~niBuilder()
            {
                Destory();
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build bsp 
            * @param            threshold:          If the number of elements is more than
            *                                       threshold, split it into two nodes.
            * @return           bsp node
            *
            */
            niBspTree::niBspNode* niBspTree::niBuilder::Build(int threshold)
            {
                int _l = 0, _r = m_size-1;
                niBspNode *node = new niBspNode;
                CalcBBox(_l, _r, node->m_bbox);
                int _axis = node->m_bbox.MaxAxis();
                int  _separator;
                bool bStat = FindBestSplit(_l, _r, _axis, _separator);
                if (!bStat)
                {
                    std::cout<<"niBspTree::niBuilder::Build FindBestSplit Error1!"<<std::endl;
                }
                Split(_l, _r, _axis, _separator);

                node->l_child = Build(_l, _separator, threshold);
                node->r_child = Build(_separator+1, _r, threshold);
                return node;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build bsp 
            * @param            _l:                 start position
            * @param            _r:                 end position
            * @param            threshold:          If the number of elements is more than
            *                                       threshold, split it into two nodes.
            * @return           bsp node
            *
            */
            niBspTree::niBspNode* niBspTree::niBuilder::Build(int _l, int _r, int threshold)
            {
                if (_r - _l <= threshold)
                {
                    niBspLeaf* leaf = new niBspLeaf;
                    leaf->m_refPoints.reserve(_r - _l + 1);
                    for (int i = _l; i <= _r; ++i)
                    {
                        niBuildNode *buildNode = m_sorted[0][i];
                        leaf->m_refPoints.push_back(buildNode->m_refPoint2d);
                        leaf->m_bbox.Append( buildNode->m_refPoint2d.m_pointRef );
                    }
                    return leaf;
                }
                else if (_l < _r)
                {
                    niBspNode *node = new niBspNode;
                    CalcBBox(_l, _r, node->m_bbox);
                    int _axis = node->m_bbox.MaxAxis();
                    int  _separator;
                    bool bStat = FindBestSplit(_l, _r, _axis, _separator);
                    if (!bStat)
                    {
                        std::cout<<"niBspTree::niBuilder::Build FindBestSplit Error2!"<<std::endl;
                    }
                    Split(_l, _r, _axis, _separator);

                    node->l_child = Build(_l, _separator, threshold);
                    node->r_child = Build(_separator+1, _r, threshold);
                    return node;
                }
                else
                {
                    std::cout<<"niBspTree::niBuilder::Build Error!"<<std::endl;
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
            void niBspTree::niBuilder::CalcBBox(int _l, int _r, niBBox2d &bbox)
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
            void niBspTree::niBuilder::Destory()
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
            * First, split along the longest axis
            * Second, minize the area of the two splited nodes
            * Third, balance the depth of children nodes.
            *
            * @param            _l:                 start position
            * @param            _r:                 end position
            * @param            _axis:              longest axis
            * @param            _separator:         split position
            * @return           true:               success
            *                   false:              failed
            *
            */
            bool niBspTree::niBuilder::FindBestSplit(int _l, int _r, int _axis, int &_separator)
            {
                _separator = -1;

                const int num_nodes = _r - _l + 1;
                if (num_nodes <= 1)
                    return false;

                if (_l+1 == _r)
                {
                    _separator = _l;
                    return true;
                }

                double coef = 1;
                double min_cost = (double)1e300;
                double max_cost = (double)-1e300;

                niBBoxCost cost_from_left;
                cost_from_left.m_bbox = m_sorted[_axis][_l]->m_bbox;
                cost_from_left.m_cost = 1;

                m_cost_from_cache[_r].m_bbox = m_sorted[_axis][_r]->m_bbox;
                m_cost_from_cache[_r].m_cost = 1;

                for (int i = _r-1; i > _l; --i)
                {
                    niBBoxCost &curr = m_cost_from_cache[i];
                    niBBoxCost &last = m_cost_from_cache[i+1];

                    curr.m_bbox = last.m_bbox;
                    curr.m_bbox.Append(m_sorted[_axis][i]->m_bbox);

                    curr.m_cost = last.m_cost + 1;
                }

                for (int i = _l + 1; i < _r; ++i)
                {
                    cost_from_left.m_bbox.Append(m_sorted[_axis][i]->m_bbox);
                    cost_from_left.m_cost += 1;

                    double cost_left, cost_right, cost;
                    cost_left = cost_from_left.m_bbox.Area() * (cost_from_left.m_cost + log((double)(i-_l)) * coef);

                    niBBoxCost& cost_from_right = m_cost_from_cache[i];
                    cost_right = cost_from_right.m_bbox.Area() * (cost_from_right.m_cost + log((double)(_r-i+1)) * coef);

                    cost = cost_left + cost_right;
                    if (cost < min_cost)
                    {
                        min_cost = cost;
                        _separator = i;
                    }
                    if (max_cost < cost)
                    {
                        max_cost = cost;
                    }
                }
                if (-1 == _separator)
                {
                    _separator = (_l + _r) / 2;
                }

                return -1 != _separator;
            }
            
            //-----------------------------------------------------------------------------
            // FUNCTION InitBuild
            //-----------------------------------------------------------------------------
            /**
            * Init build
            *
            * @param            pointRefs:          point references
            * @return           true:               success
            *                   false:              failed
            *
            */
            bool niBspTree::niBuilder::InitBuild(
                const niArrayT<const niPoint2d*> &pointRefs)
            {
                m_size = int(pointRefs.size());
                try
                {
                    m_build_nodes       = new niBuildNode[m_size];
                    m_sorted[0]         = new niBuildNode*[m_size];
                    m_sorted[1]         = new niBuildNode*[m_size];
                    m_cost_from_cache   = new niBBoxCost[m_size];
                }
                catch (std::bad_alloc)
                {
                    Destory();
                    return false;
                }

                for (int i = 0; i < m_size; ++i)
                {
                    niBuildNode &buildNode              = m_build_nodes[i];
                    buildNode.m_refPoint2d.m_pointRef   = pointRefs[i];
                    buildNode.m_refPoint2d.m_IdOfPoint    = i;
                    buildNode.m_hit                     = false;
                    buildNode.m_bbox.Append( buildNode.m_refPoint2d.m_pointRef );

                    m_sorted[0][i]    = &buildNode;
                    m_sorted[1][i]    = &buildNode;
                }

                std::sort(m_sorted[0], &m_sorted[0][m_size], niBuildNode::_compare_<0>);
                std::sort(m_sorted[1], &m_sorted[1][m_size], niBuildNode::_compare_<1>);
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION InitBuild
            //-----------------------------------------------------------------------------
            /**
            * Init build
            *
            * @param            pointRefs:          point references
            * @return           true:               success
            *                   false:              failed
            *
            */
            bool niBspTree::niBuilder::InitBuild(
                const niArrayT<niRefPoint2d> &refPoints)
            {
                m_size = int(refPoints.size());
                try
                {
                    m_build_nodes       = new niBuildNode[m_size];
                    m_sorted[0]         = new niBuildNode*[m_size];
                    m_sorted[1]         = new niBuildNode*[m_size];
                    m_cost_from_cache   = new niBBoxCost[m_size];
                }
                catch (std::bad_alloc)
                {
                    Destory();
                    return false;
                }

                for (int i = 0; i < m_size; ++i)
                {
                    niBuildNode &buildNode  = m_build_nodes[i];
                    buildNode.m_refPoint2d  = refPoints[i];
                    buildNode.m_hit         = false;
                    buildNode.m_bbox.Append( buildNode.m_refPoint2d.m_pointRef );

                    m_sorted[0][i]    = &buildNode;
                    m_sorted[1][i]    = &buildNode;
                }

                std::sort(m_sorted[0], &m_sorted[0][m_size], niBuildNode::_compare_<0>);
                std::sort(m_sorted[1], &m_sorted[1][m_size], niBuildNode::_compare_<1>);
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
            void niBspTree::niBuilder::Split(int _l, int _r, int _axis, int _separator)
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
            * @param            threshold:          If the number of elements is more than
            *                                       threshold, split it into two nodes.
            *
            */
            niBspTree::niBspTree(int threshold)
                : m_root(NULL),
                m_cThreshold(threshold)
            {
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Construction
            //-----------------------------------------------------------------------------
            /**
            * Construction
            *
            * @param            refPoints:          points
            * @param            threshold:          If the number of elements is more than
            *                                       threshold, split it into two nodes.
            *
            */
            niBspTree::niBspTree(
                const niArrayT<niRefPoint2d> &refPoints,
                int threshold)
                : m_root(NULL)
                , m_cThreshold(threshold)
            {
                Build(refPoints);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION DeConstruction
            //-----------------------------------------------------------------------------
            /**
            * DeConstruction
            *
            */
            niBspTree::~niBspTree()
            {
                _Destory();
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build
            *
            * @param            points:             points
            * @return           true:               success
            *                   false:              failed
            */
            bool niBspTree::Build(
                const niPoint2dArray &points)
            {
                size_t numPoints = points.size();
                if (numPoints < 1)
                    return false;

                niArrayT<const niPoint2d*> pointRefs;
                pointRefs.resize(numPoints);
                for (size_t i = 0; i < numPoints; ++i)
                {
                    pointRefs[i] = &points[i];
                }

                return Build(pointRefs);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build
            *
            * @param            pointRefs:          points
            * @return           true:               success
            *                   false:              failed
            */
            bool niBspTree::Build(
                const niArrayT<const niPoint2d*> &pointRefs)
            {
                int numPoints = int(pointRefs.size());
                if (numPoints < 1)
                    return false;

                _Destory();

                niBuilder build;

                bool bStat = build.InitBuild(pointRefs);
                if (!bStat)
                    return false;

                m_root = build.Build(m_cThreshold);

                return NULL != m_root;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Build
            //-----------------------------------------------------------------------------
            /**
            * Build
            *
            * @param            pointRefs:          points
            * @return           true:               success
            *                   false:              failed
            */
            bool niBspTree::Build(
                const niArrayT<niRefPoint2d> &refPoints)
            {
                int numPoints = int(refPoints.size());
                if (numPoints < 1)
                    return false;

                _Destory();

                niBuilder build;

                bool bStat = build.InitBuild(refPoints);
                if (!bStat)
                    return false;

                m_root = build.Build(m_cThreshold);

                return NULL != m_root;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION DumpToMIF
            //-----------------------------------------------------------------------------
            /**
            * Dump bsp tree into MIF format, then we can check it in QGis
            *
            * @param            file:               destination
            * @param            color:              color of pen in MIF format
            */
            void niBspTree::DumpToMIF(
                const ni::String &file,
                int color)
            {
                std::ofstream writer;
                writer.open(file.c_str());
                writer
                    <<"Version 450"<<std::endl
                    <<"Charset \"WindowsSimpChinese\""<<std::endl
                    <<"Delimiter \",\""<<std::endl
                    <<std::setprecision(16)<<"CoordSys NonEarth Units \"m\" Bounds ("
                    <<m_root->m_bbox.P1.X<<","<<m_root->m_bbox.P1.Y<<") ("
                    <<m_root->m_bbox.P2.X<<", "<<m_root->m_bbox.P2.Y<<")"<<std::endl
                    <<"Columns 1"<<std::endl
                    <<"  id Char(10)"<<std::endl
                    <<"Data"<<std::endl;

                std::stack<const niBspTree::niBspNode*> stack;

                stack.push(m_root);

                const niBspTree::niBspNode *iter;

                while (!stack.empty())
                {
                    iter = stack.top();     stack.pop();

                    while (iter)
                    {
                        writer<<"Pline 5"<<std::endl
                            <<iter->m_bbox.P1.X<<" "<<iter->m_bbox.P1.Y<<" "<<std::endl
                            <<iter->m_bbox.P2.X<<" "<<iter->m_bbox.P1.Y<<" "<<std::endl
                            <<iter->m_bbox.P2.X<<" "<<iter->m_bbox.P2.Y<<" "<<std::endl
                            <<iter->m_bbox.P1.X<<" "<<iter->m_bbox.P2.Y<<" "<<std::endl
                            <<iter->m_bbox.P1.X<<" "<<iter->m_bbox.P1.Y<<" "<<std::endl;
                        writer<<"    Pen (1,2,"<<color<<") "<<std::endl;

                        if (iter->IsLeaf())
                        {
                            const niBspTree::niBspLeaf* leaf =
                                dynamic_cast<const niBspTree::niBspLeaf*>(iter);
                            size_t numPoints = leaf->m_refPoints.size();
                            for (size_t i = 0; i < numPoints; ++i)
                            {
                                writer<<"Point "
                                    <<leaf->m_refPoints[i].m_pointRef->X<<" "
                                    <<leaf->m_refPoints[i].m_pointRef->Y<<std::endl;
                            }
                            //writer<<"   Symbol (30, 255, 12)"<<std::endl;
                        }
                        if (iter->r_child)
                        {
                            stack.push(iter->r_child);
                        }
                        iter = iter->l_child;
                    }
                }
            }

            //-----------------------------------------------------------------------------
            // FUNCTION StatTool
            //-----------------------------------------------------------------------------
            /**
            * Used to check efficiency of the bsp tree.
            * Compare the real depth and theoretical depth of the bsp tree.
            *
            */
            int niBspTree::StatTool()
            {
                int maxDepth    = 0;
                int minDepth    = 0;
                int counter     = 0;

                std::stack<niBspNode*>  ns;
                std::stack<int>         ds;

                niBspNode* iter = m_root;
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

            //-----------------------------------------------------------------------------
            // FUNCTION Destory
            //-----------------------------------------------------------------------------
            /**
            * Destory
            *
            */
            void niBspTree::_Destory()
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
