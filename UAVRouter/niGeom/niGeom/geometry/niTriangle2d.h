//! \file
// \brief
// Triangle  class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTriangle2d_H
#define niTriangle2d_H

#include <niGeom/geometry/niGeom2dTypes.h>

namespace ni
{
    namespace geometry
    {
        class niGeomMath2d;
        class niTriangle2dFn;

        /**
         * \brief 2D Triangle  class
         *
         */
        class niTriangle2d
        {
            friend class niGeomMath2d;
            friend class niTriangle2dFn;
        public:
            niTriangle2d            () : m_valid(false)    {}

            niTriangle2d            (const niPoint2d &p0, const niPoint2d &p1, const niPoint2d &p2)
            {
                MakeTriangle(p0, p1, p2);
            }

            niTriangle2d            (const niTriangle2d& t)
            {
                m_valid = t.m_valid;
                P0 = t.P0;
                P1 = t.P1;
                P2 = t.P2;
            }

            inline bool             IsValid                 () const
            {
                return m_valid;
            }

            bool                    MakeTriangle            (
                                                            const niPoint2d &p0,
                                                            const niPoint2d &p1,
                                                            const niPoint2d &p2)
            {
                m_valid = false;
                if (p0 == p1)
                    return false;
                if (p1 == p2)
                    return false;
                if (p2 == p0)
                    return false;
                P0 = p0;
                P1 = p1;
                P2 = p2;
                m_valid = true;
                return true;
            }

            inline niPoint2d        operator[]              (int idx) const
            {
                return (&P0)[idx%3];
            }

            inline niTriangle2d&    operator=               (const niTriangle2d& other)
            {
                m_valid = other.m_valid;
                P0 = other.P0;
                P1 = other.P1;
                P2 = other.P2;
            return *this;
            }

        protected:
            bool                    m_valid;
            niPoint2d               P0;
            niPoint2d               P1;
            niPoint2d               P2;
        };
    }
}



#endif
