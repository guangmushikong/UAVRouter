//! \file
// \brief
// Define types of 2d geometry
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niGeom2dTypes_H
#define niGeom2dTypes_H

#include <niGeom/niTypes.h>
#include <niGeom/math/niMath.h>
#include <niGeom/math/niVector2d.h>

namespace ni
{
    namespace geometry
    {
        typedef niVector2d niPoint2d;

        class niPolygon2d;
        /**
         * \brief triface 2d
         *
         */
        class niTriFace2d
        {
        public:
            niTriFace2d          () : aIdx(-1), bIdx(-1), cIdx(-1)    {}

            niTriFace2d          (const niTriFace2d &tf)
            {
                aIdx = tf.aIdx;
                bIdx = tf.bIdx;
                cIdx = tf.cIdx;
            }

            void                Init                        (int _a, int _b, int _c)
            {
                aIdx = _a;
                bIdx = _b;
                cIdx = _c;
            }

            int&                operator[]                  (int idx)
            {
                return (&aIdx)[idx];
            }

        public:
            int                 aIdx;
            int                 bIdx;
            int                 cIdx;
        };

        /**
         * \brief geometry reference
         *
         */
        struct niRefGeom2d
        {
            niRefGeom2d()
                : m_geomRef(NULL), m_IdOfGeom(-1)
            {
            }

            niRefGeom2d(const void* geomRef, int IdOfGeom)
                : m_geomRef(geomRef), m_IdOfGeom(IdOfGeom)
            {
            }

            const void*         m_geomRef;
            int                 m_IdOfGeom;
        };

        /**
         * \brief point reference
         *
         */
        struct niRefPoint2d
        {
            niRefPoint2d()
                : m_pointRef(NULL)
                , m_IdOfPoint(-1)
                , m_IdOfPoly(-1)
            {
            }

            niRefPoint2d(const niPoint2d* pointRef, int pointId)
                : m_pointRef(pointRef)
                , m_IdOfPoint(pointId)
                , m_IdOfPoly(-1)
            {
            }

            niRefPoint2d(const niPoint2d* pointRef, int pointId, int polyId)
                : m_pointRef(pointRef)
                , m_IdOfPoint(pointId)
                , m_IdOfPoly(polyId)
            {
            }

            niRefPoint2d(const niRefPoint2d &other)
                : m_pointRef(other.m_pointRef)
                , m_IdOfPoint(other.m_IdOfPoint)
                , m_IdOfPoly(other.m_IdOfPoly)
            {

            }

            inline bool         operator ==(const niRefPoint2d &other) const
            {
                return m_pointRef == other.m_pointRef;
            }

            inline bool         operator < (const niRefPoint2d &other) const
            {
                return m_pointRef < other.m_pointRef;
            }

            const niPoint2d*    m_pointRef;
            int                 m_IdOfPoint;
            int                 m_IdOfPoly;
        };

        typedef niArrayT<niPoint2d>     niPoint2dArray;
        typedef niArrayT<niTriFace2d>   niTriFace2dArray;
        typedef niArrayT<niPolygon2d>   niPolygon2dArray;
    }
}

#endif
