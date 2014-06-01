//! \file
// \brief
// Convert a polygon into triangle mesh
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niTriangulation2d.h>
#include <niGeom/geometry/niTriMesh2d.h>
#include <niGeom/geometry/niPolygon2d.h>
#include <niGeom/geometry/niGeomMath2d.h>
#include <niGeom/geometry/tree/niKdTree.h>
#include <niGeom/algorithm/niBstTree.h>

#define THRESHOLH_ACCE1     512
#define THRESHOLH_ACCE2     256

#include <algorithm>
#include <stack>
#include <iostream>
namespace ni
{
    namespace geometry
    {
        niTriangulation2d::niTriangulation2d(niPolygon2d &polygon)
            : m_polygon(polygon), m_hBst(NULL), m_hKdt(NULL)
        {
            m_remain_counter = 0;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION FindEarWithMinAngle
        //-----------------------------------------------------------------------------
        /**
        * Find the ear, which has the min angle
        *
        * @return      -1:          can not find an ear
        *              >=0:         the vert index of the ear, which has the min angle
        */
        int niTriangulation2d::FindEarWithMinAngle()
        {
            algorithm::niBstTree<_niTopoVert*> *bst = (algorithm::niBstTree<_niTopoVert*>*)m_hBst;
            ni::algorithm::niBstTree<_niTopoVert*>::_iterator iter;

            bool is_ear = false;
            for (iter = bst->_begin(); iter != bst->_end(); ++iter)
            {
                _niTopoVert *_a = *iter;
                is_ear = IsEar(_a->_index);
                if (is_ear)
                    return _a->_index;
            }
            return -1;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsEar
        //-----------------------------------------------------------------------------
        /**
        * Is the vert is ear
        *
        * @param       _index:      vert index
        * @return      ture:        is ear
        *              false:       is not ear
        */
        bool niTriangulation2d::IsEar(int _index)
        {
            int _l = m_topo_verts[_index].l_link;
            int _r = m_topo_verts[_index].r_link;
            niPoint2d &a = m_inputPoints[_index];
            niPoint2d &b = m_inputPoints[_l];
            niPoint2d &c = m_inputPoints[_r];

            niBBox2d bbox;
            bbox.Append(a);
            bbox.Append(b);
            bbox.Append(c);

            double area = fabs( (b-a).Cross(c-a) );

            EPointInGeometry fInGeometry;

            if (NULL == m_hKdt || m_remain_counter < THRESHOLH_ACCE2)
            {
                for (int _it = m_topo_verts[_r].r_link; _it != _l; _it = m_topo_verts[_it].r_link)
                {
                    niPoint2d &p = m_inputPoints[_it];
                    if (!niGeomMath2d::IsPointInBox(p, bbox))
                        continue;

                    fInGeometry = niGeomMath2d::IsPointInTriangle(p, a, b, c, area);
                    if (fInGeometry == eInGeometry || fInGeometry == eOnBorder)
                        return false;
                    if (fInGeometry == eErrorGeometry)
                        return false;
                }
            }
            else
            {
                niIntArray IdOfPoints;
                IdOfPoints.reserve(16);
                _PointsInBBox(bbox, IdOfPoints);
                int num_points = int(IdOfPoints.size());
                int IdOfPoint;
                for (int i = 0; i < num_points; ++i)
                {
                    IdOfPoint = IdOfPoints[i];
                    if (IdOfPoint == _index)
                        continue;
                    if (IdOfPoint == _l)
                        continue;
                    if (IdOfPoint == _r)
                        continue;

                    const niPoint2d &p = m_inputPoints[ IdOfPoint ];

                    fInGeometry = niGeomMath2d::IsPointInTriangle(p, a, b, c, area);
                    if (fInGeometry == eInGeometry || fInGeometry == eOnBorder)
                        return false;
                }
            }
            return true;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION niKdTree::PointsInBBox
        //-----------------------------------------------------------------------------
        /**
        * Get points in bbox.
        *
        * @param       bbox:        the input points
        * @param       IdOfPoints:  save points' refids in bbox
        * @return      success:     return true
        *              failed:      return false
        */
        bool niTriangulation2d::_PointsInBBox(
            const niBBox2d &bbox,
            niIntArray &IdOfPoints)
        {
            IdOfPoints.clear();
            tree::niKdTree *kdt = (tree::niKdTree *)m_hKdt;
            const tree::niKdTree::niKdNode *root = kdt->Root();

            if (NULL == root)
                return false;

            std::stack<const tree::niKdTree::niKdNode*> stack;
            stack.push(root);

            const tree::niKdTree::niKdNode* iter;
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
                    if (!m_markers[iter->m_refPoint2d.m_IdOfPoint] &&
                        niGeomMath2d::IsPointInBox(*iter->m_refPoint2d.m_pointRef, bbox))
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

        //-----------------------------------------------------------------------------
        // FUNCTION niKdTree::_PointsInBBox
        //-----------------------------------------------------------------------------
        /**
        * Get points in bbox.
        *
        * @param       bbox:        the input points
        * @param       pointRefs:   save points in bbox
        * @return      if success, return true
        *                or return false
        */
        bool niTriangulation2d::_PointsInBBox(
            const niBBox2d &bbox,
            niArrayT<const niPoint2d*> &pointRefs)
        {
            tree::niKdTree *kdt = (tree::niKdTree *)m_hKdt;
            pointRefs.clear();

            const tree::niKdTree::niKdNode *root = kdt->Root();

            if (NULL == root)
                return false;

            std::stack<const tree::niKdTree::niKdNode*> stack;
            stack.push(root);

            const tree::niKdTree::niKdNode* iter;
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
                    if (!m_markers[iter->m_refPoint2d.m_IdOfPoint] &&
                        niGeomMath2d::IsPointInBox(*iter->m_refPoint2d.m_pointRef, bbox))
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
        // FUNCTION Process
        //-----------------------------------------------------------------------------
        /**
        * Convert the polygon intp triangle mesh
        *
        * @param       tri_mesh:    store the triangle mesh
        * @param       error_msg:   store error message
        * @return      true:        convert sucess
        *              false:       convert failed
        */
        bool niTriangulation2d::Process(niTriMesh2d &tri_mesh, String &error_msg)
        {
            tri_mesh.Clear();

            m_polygon.GetPoints(m_inputPoints);

            bool bStat = tri_mesh.InitPoints(m_inputPoints);
            if (!bStat)
            {
                error_msg = "_points error!";
                return false;
            }

            bStat = _Init();
            if (!bStat)
            {
                error_msg = "_Init error!";
                return false;
            }

            niTriFace2dArray faces;

            int num_points = int(m_inputPoints.size());
            faces.resize(num_points-2);
            for (int i = 0; i < num_points-2; ++i)
            {
                int _index = FindEarWithMinAngle();
                if (-1 == _index)
                {
                    error_msg = "FindEarWithMinAngle error!";
                    return false;
                }

                int _l = m_topo_verts[_index].l_link;
                int _r = m_topo_verts[_index].r_link;

                faces[i].aIdx = _index;
                faces[i].bIdx = _r;
                faces[i].cIdx = _l;

                _SplitEar(_index);

                if (NULL != m_hKdt && m_remain_counter >= THRESHOLH_ACCE2)
                {
                    m_markers[_index] = true;
                }
            }

            bStat = tri_mesh.InitFaces(faces);
            if (!bStat)
            {
                error_msg = "MakeTriMesh error!";
                return false;
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _Destory
        //-----------------------------------------------------------------------------
        /**
        * Free memory
        *
        */
        void niTriangulation2d::_Destory()
        {
            if (NULL != m_hBst)
            {
                algorithm::niBstTree<_niTopoVert*> *bst = (algorithm::niBstTree<_niTopoVert*>*)m_hBst;
                delete bst;
                m_hBst = NULL;
            }
            if (NULL != m_hKdt)
            {
                tree::niKdTree *kdt = (tree::niKdTree*)m_hKdt;
                delete kdt;
                m_hKdt = NULL;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _Init
        //-----------------------------------------------------------------------------
        /**
        * Free memory
        *
        */
        bool niTriangulation2d::_Init()
        {
            _Destory();

            algorithm::niBstTree<_niTopoVert*> *bst = 
                new algorithm::niBstTree<_niTopoVert*>(_niTopoVert::_compare_);
            m_hBst = (void*)bst;

            //Avoid abnormal data
            niBBox2d bbox;
            size_t num_points = m_inputPoints.size();
            for (size_t i = 0; i <num_points; ++i)
            {
                bbox.Append(m_inputPoints[i]);
            }
            niGeomMath2d::AvoidAbnormalData(10, bbox, m_inputPoints);

            m_remain_counter = num_points;
            if (m_remain_counter >= THRESHOLH_ACCE1)
            {
                tree::niKdTree *kdt = new tree::niKdTree;
                kdt->Build(m_inputPoints);
                m_hKdt = (void*)kdt;
                m_markers.resize(num_points, false);
            }

            int num_convex_angles = 0;

            m_topo_verts.resize(num_points);

            m_topo_verts[0]._index = 0;
            m_topo_verts[0].l_link = num_points-1;
            m_topo_verts[0].r_link = 1;
            m_topo_verts[0]._angle = niGeomMath2d::Angle(
                m_inputPoints[0],
                m_inputPoints[1],
                m_inputPoints[num_points-1]);
            if (m_topo_verts[0]._angle < _PI_)
                ++num_convex_angles;

            for (size_t i = 1; i < num_points-1; ++i)
            {
                m_topo_verts[i]._index = i;
                m_topo_verts[i].l_link = i-1;
                m_topo_verts[i].r_link = i+1;
                m_topo_verts[i]._angle = niGeomMath2d::Angle(
                    m_inputPoints[i],
                    m_inputPoints[i+1],
                    m_inputPoints[i-1]);
                if (m_topo_verts[i]._angle < _PI_)
                    ++num_convex_angles;
            }

            m_topo_verts[num_points-1]._index = num_points-1;
            m_topo_verts[num_points-1].l_link = num_points-2;
            m_topo_verts[num_points-1].r_link = 0;
            m_topo_verts[num_points-1]._angle = niGeomMath2d::Angle(
                m_inputPoints[num_points-1],
                m_inputPoints[0],
                m_inputPoints[num_points-2]);
            if (m_topo_verts[num_points-1]._angle < _PI_)
                ++num_convex_angles;

            if (0 == num_convex_angles)
                return false;

            _niTopoVert** convex_angles = new _niTopoVert*[num_convex_angles];
            num_convex_angles = 0;

            for (size_t i = 0; i < num_points; ++i)
            {
                if (m_topo_verts[i]._angle < _PI_)
                {
                    convex_angles[num_convex_angles++] = &m_topo_verts[i];
                }
            }

            std::sort(&convex_angles[0], &convex_angles[num_convex_angles], _niTopoVert::_sort_);
            bst->Build(convex_angles, num_convex_angles);

            delete []convex_angles;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _SplitEar
        //-----------------------------------------------------------------------------
        /**
        * Split ear
        *
        * @param       _index:      id of ear need to be splited
        * @return      true:        Split sucess
        *              false:       Split failed
        */
        bool niTriangulation2d::_SplitEar(int _index)
        {
            algorithm::niBstTree<_niTopoVert*> *bst = (algorithm::niBstTree<_niTopoVert*>*)m_hBst;

            int _l = m_topo_verts[_index].l_link;
            int _r = m_topo_verts[_index].r_link;

            bst->Delete(&m_topo_verts[_index]);
            if (m_topo_verts[_r]._angle < _PI_)
                bst->Delete(&m_topo_verts[_r]);
            if (m_topo_verts[_l]._angle < _PI_)
                bst->Delete(&m_topo_verts[_l]);

            m_topo_verts[_l].r_link = _r;
            m_topo_verts[_r].l_link = _l;

            int _ll = m_topo_verts[_l].l_link;
            int _rr = m_topo_verts[_r].r_link;

            niPoint2d &a = m_inputPoints[_ll];
            niPoint2d &b = m_inputPoints[_l];
            niPoint2d &c = m_inputPoints[_r];
            niPoint2d &d = m_inputPoints[_rr];

            m_topo_verts[_l]._angle = niGeomMath2d::Angle(b, c, a);
            m_topo_verts[_r]._angle = niGeomMath2d::Angle(c, d, b);

            if (m_topo_verts[_l]._angle < _PI_)
            {
                bst->Insert(&m_topo_verts[_l]);
            }
            if (m_topo_verts[_r]._angle < _PI_)
            {
                bst->Insert(&m_topo_verts[_r]);
            }
            --m_remain_counter;
            return true;
        }
    }
}