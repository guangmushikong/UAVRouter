//! \file
// \brief
// 3D Geometry type
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niGeom3dTypes_H
#define niGeom3dTypes_H

#include <niGeom/niTypes.h>
#include <niGeom/math/niMath.h>
#include <niGeom/math/niVector3d.h>
#include <niGeom/geometry/niTexCoord.h>

namespace ni
{
    namespace geometry
    {
        typedef niVector3d niVert3d;
        typedef niVector3d niNorm3d;

        class niBBox3d;
        class niBvhNode;
        class niLine3d;
        class niTriangle3d;
        class niPolygon3d;

        /**
         * \brief Vert ref
         *
         */
        struct _SVertRef
        {
            _SVertRef               () : vRef(0), nRef(0), tRef(0){}
            _SVertRef               (int v, int n, int u) : vRef(v), nRef(n), tRef(u){}
            int                     vRef;
            int                     nRef;
            int                     tRef;
            bool                    operator <                  (const _SVertRef &other) const
            {
                if (vRef < other.vRef)
                    return true;
                if (vRef > other.vRef)
                    return false;
                if (nRef < other.nRef)
                    return true;
                if (nRef > other.nRef)
                    return false;
                return tRef < other.tRef;
            }
        };

        /**
         * \brief ref tri face
         *
         */
        class niTriFace3d
        {
        public:
            niTriFace3d             (){}

            niTriFace3d             (const niTriFace3d &other)
            {
                aRef = other.aRef;
                bRef = other.bRef;
                cRef = other.cRef;
            }

            void                    Init                        (_SVertRef &_a, _SVertRef &_b, _SVertRef &_c)
            {
                aRef = _a;
                bRef = _b;
                cRef = _c;
            }

            _SVertRef&              operator[]                  (int idx)
            {
                return (&aRef)[idx];
            }

        public:
            _SVertRef               aRef;
            _SVertRef               bRef;
            _SVertRef               cRef;
        };

        /**
         * \brief poly face
         *
         */
        class niPolyFace3d
        {
        public:
            niPolyFace3d            (){}

            niPolyFace3d            (const niPolyFace3d &other)
            {
                m_refs = other.m_refs;
            }

            void                    Append                      (int v, int n, int u)
            {
                m_refs.push_back( _SVertRef(v, n, u) );
            }

            void                    Append                      (const _SVertRef &ref)
            {
                m_refs.push_back( ref );
            }

            _SVertRef&              operator[]                  (int idx)
            {
                return m_refs[idx];
            }

        public:
            niArrayT<_SVertRef>     m_refs;
        };

        typedef niArrayT<niVert3d>      niVert3dArray;
        typedef niArrayT<niNorm3d>      niNorm3dArray;
        typedef niArrayT<niTexCoord>    niTexCoordArray;
        typedef niArrayT<_SVertRef>     niVertRefArray;
        typedef niArrayT<niTriFace3d>   niTriFace3dArray;
        typedef niArrayT<niPolyFace3d>  niPolyFace3dArray;

    }
}

#endif
