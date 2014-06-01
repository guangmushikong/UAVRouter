//! \file
// \brief
// Line class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niLine2d_H
#define niLine2d_H

#include <niGeom/geometry/niGeom2dTypes.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief Line 2d class
         *
         */
        class niLine2d
        {
        public:
            niLine2d            ()    {}
            niLine2d            (const niPoint2d &p1, const niPoint2d &p2)
            {
                P1 = p1, P2 = p2;
            }

            niLine2d            (const niLine2d &l)
            {
                P1 = l.P1;
                P2 = l.P2;
            }

            inline void         Init                    (const niPoint2d &p1, const niPoint2d &p2)
            {
                P1 = p1, P2 = p2;
            }

            inline double       Length                  () const
            {
                return (P2-P1).Length();
            }

            inline niLine2d&    operator=               (const niLine2d &other)
            {
                P1 = other.P1;
                P2 = other.P2;
                return *this;
            }

            inline niPoint2d    operator[]              (int idx) const
            {
                return (&P1)[idx%2];
            }

            inline bool         operator==              (const niLine2d &l)
            {
                return (P1 == l.P1) && (P2 == l.P2);
            }

            inline bool         operator!=              (const niLine2d &l)
            {
                if (P1 != l.P1)
                    return true;
                return P2 != l.P2;
            }

            inline bool         operator>               (const niLine2d &l)
            {
                if (P1 > l.P1)
                    return true;
                if (P1 < l.P1)
                    return false;
                return P2 > l.P2;
            }

            inline bool         operator<               (const niLine2d &l)
            {
                if (P1 < l.P1)
                    return true;
                if (P1 > l.P1)
                    return false;
                return P2 < l.P2;
            }

        protected:
            niPoint2d           P1;
            niPoint2d           P2;
        };
    }
}


#endif