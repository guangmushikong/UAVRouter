//! \file
// \brief
// 3D Geometry math libaray
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niGeomMath3d_H
#define niGeomMath3d_H

#include <niGeom/geometry/niGeomDef.h>
#include <niGeom/geometry/niGeom3dTypes.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief 3D Geometry math libaray
         *
         */
        class niGeomMath3d
        {
        public:
            static bool                 IsBoxOverlapBox(
                const niBBox3d &bbox1,
                const niBBox3d &bbox2);

            static bool                 IsLineISectBox(
                const niVert3d &p0,
                const niVert3d &p1,
                const niBBox3d &bbox);

            static bool                 IsPointInBox(
                const niVert3d &p,
                const niBBox3d &bbox);

            static EPointInGeometry     IsPointInTriangle(
                const niVert3d &p,
                const niTriangle3d &tri);

            static EPointInGeometry     IsPointInTriangle(
                const niVert3d &p,
                const niVert3d &a,
                const niVert3d &b,
                const niVert3d &c);

            static EPointInGeometry     IsPointInTriangle(
                const niVert3d &p,
                const niVert3d &a,
                const niVert3d &b,
                const niVert3d &c,
                double area);

            static bool                 IsPointOnLine(
                const niVert3d &p,
                const niVert3d &p0,
                const niVert3d &p1);

            static bool                 LineISectLine(
                const niVert3d &p0,
                const niVert3d &p1,
                const niVert3d &q0,
                const niVert3d &q1,
                niVert3d &isect);

        };
    }
}

#endif