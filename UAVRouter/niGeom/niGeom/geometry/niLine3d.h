//! \file
// \brief
// Line class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niLine3d_H
#define niLine3d_H

#include <niGeom/geometry/niGeom3dTypes.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief Line 3d class
         *
         */
        class niLine3d
        {
        public:
            niLine3d            ()    {}
            niLine3d            (const niVert3d &p1, const niVert3d &p2)
            {
                P1 = p1, P2 = p2;
            }

            niLine3d            (const niLine3d &l)
            {
                P1 = l.P1;
                P2 = l.P2;
            }

            inline void         Init                    (const niVert3d &p1, const niVert3d &p2)
            {
                P1 = p1, P2 = p2;
            }

            inline double       Length                  () const
            {
                return (P2-P1).Length();
            }

            inline niLine3d&    operator=               (const niLine3d &other)
            {
                P1 = other.P1;
                P2 = other.P2;
                return *this;
            }

            inline niVert3d    operator[]              (int idx) const
            {
                return (&P1)[idx%2];
            }

            inline bool         operator==              (const niLine3d &l)
            {
                return (P1 == l.P1) && (P2 == l.P2);
            }

            inline bool         operator!=              (const niLine3d &l)
            {
                if (P1 != l.P1)
                    return true;
                return P2 != l.P2;
            }

            inline bool         operator>               (const niLine3d &l)
            {
                if (P1 > l.P1)
                    return true;
                if (P1 < l.P1)
                    return false;
                return P2 > l.P2;
            }

            inline bool         operator<               (const niLine3d &l)
            {
                if (P1 < l.P1)
                    return true;
                if (P1 > l.P1)
                    return false;
                return P2 < l.P2;
            }

        protected:
            niVert3d           P1;
            niVert3d           P2;
        };
    }
}


#endif