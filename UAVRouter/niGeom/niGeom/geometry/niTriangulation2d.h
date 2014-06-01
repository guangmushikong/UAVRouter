//! \file
// \brief
// Convert a polygon into triangle mesh
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version


#ifndef niTriangulation2d_H
#define niTriangulation2d_H

#include <niGeom/geometry/niGeom2dTypes.h>

#include <niGeom/math/niMath.h>

namespace ni
{
    namespace geometry
    {
        class niBBox2d;
        class niPolygon2d;
        class niTriMesh2d;

        /**
         * \brief used to triangulation 2d polygons
         *
         */
        class niTriangulation2d
        {
        public:
            /**
             * \brief Topology of vert
             *
             * vert angle, vert id, left vert index, right vert index
             */
            class _niTopoVert
            {
            public:
                _niTopoVert     () : _angle(0.0), _index(-1) {}
                _niTopoVert     (double _a, int _i) : _angle(_a), _index(_i) {}

                static int              _compare_                   (const void *p1, const void *p2)
                {
                    const _niTopoVert **a1 = (const _niTopoVert**)p1;
                    const _niTopoVert **a2 = (const _niTopoVert**)p2;
                    if ((*a1)->_angle < (*a2)->_angle)
                        return -1;
                    if ((*a1)->_angle > (*a2)->_angle)
                        return 1;
                    if ((*a1)->_index < (*a2)->_index)
                        return -1;
                    if ((*a1)->_index > (*a2)->_index)
                        return 1;
                    return 0;
                }

                static bool             _sort_                      (const void *p1, const void *p2)
                {
                    const _niTopoVert *a1 = (const _niTopoVert*)p1;
                    const _niTopoVert *a2 = (const _niTopoVert*)p2;
                    if (a1->_angle < a2->_angle)
                        return true;
                    if (a1->_angle > a2->_angle)
                        return false;
                    return a1->_index < a2->_index;
                }

            public:
                double                  _angle;
                int                     _index;
                int                     r_link;
                int                     l_link;
            };

        public:
            niTriangulation2d(niPolygon2d &polygon);

            ~niTriangulation2d()
            {
                _Destory();
            }

            int                         FindEarWithMinAngle ();

            bool                        IsEar                       (int _index);

            bool                        Process                     (niTriMesh2d &tri_mesh, String &error_msg);

        protected:

            void                        _Destory                    ();

            bool                        _Init                       ();
            
            bool                        _PointsInBBox               (const niBBox2d &bbox, niArrayT<const niPoint2d*> &pointRefs);

            bool                        _PointsInBBox               (const niBBox2d &bbox, niIntArray &IdOfPoints);

            bool                        _SplitEar                   (int _index);

        protected:
            niPolygon2d&                m_polygon;
            niPoint2dArray              m_inputPoints;
            niArrayT<_niTopoVert>       m_topo_verts;
            niArrayT<bool>              m_markers;
            void*                       m_hBst;
            void*                       m_hKdt;
            int                         m_remain_counter;
        };
    }
}


#endif
