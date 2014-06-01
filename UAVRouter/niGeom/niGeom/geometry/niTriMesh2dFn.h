//! \file
// \brief
// Operator to handle triangle mesh class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTriMesh2dFn_H
#define niTriMesh2dFn_H

#include <niGeom/geometry/niGeom2dTypes.h>

namespace ni
{
    namespace geometry
    {
        class niTriMesh2d;
        class niTriMesh2dTopo;
        struct _STriMeshFanStrip;

        /**
         * \brief Operator to handle triangle mesh class
         *
         */
        class niTriMesh2dFn
        {
        public:

            niTriMesh2dFn(niTriMesh2d &trimesh) : m_trimesh(trimesh)
            {
            }

            ~niTriMesh2dFn          ()
            {
            }

            bool                    BuildFromPoints             (
                                                                const niPoint2dArray &points,
                                                                niIntArray &convexHullIds);

            bool                    BuildTriangleFan            (
                                                                niTriMesh2dTopo &topology,
                                                                _STriMeshFanStrip &fan,
                                                                int aIdx);

            bool                    BuildTriangleFans           (niArrayT<_STriMeshFanStrip> &fans);

            bool                    BuildTriangleStrip          (
                                                                niTriMesh2dTopo &topology,
                                                                _STriMeshFanStrip &strip,
                                                                niCharArray &used_flags,
                                                                int fIdx,
                                                                int vIdx);

            bool                    BuildTriangleStrips         (niArrayT<_STriMeshFanStrip> &strips);
            
            bool                    OptimizeTriMesh             ();

            bool                    OptimizeTriMesh             (niTriMesh2dTopo &topology);

            bool                    Reverse                     ();

        protected:
            bool                    OptimizeAdjacentTriangles   (niTriMesh2dTopo& topology, int eIdx, int &counter);

        protected:
            niTriMesh2d&            m_trimesh;
        };
    }
}

#endif
