//! \file
// \brief
// TexCoord class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niTexCoord_H
#define niTexCoord_H

#include <niGeom/math/niMath.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief TexCoord class
         *
         */
        class niTexCoord
        {
        public:
            niTexCoord          () : S(0.0), T(0.0)    {}

            niTexCoord          (double x, double y) : S(x), T(y)    {}

            niTexCoord          (const niTexCoord &other)
            {
                S = other.S;
                T = other.T;
            }

            inline void         Init                (double x, double y)
            {
                S = x;
                T = y;
            }

            inline double&      operator[]          (int idx)
            {
                return idx == 0 ? S : T;
            }

            inline double       operator[]          (int idx) const
            {
                return idx == 0 ? S : T;
            }

            inline niTexCoord&  operator=           (const niTexCoord &other)
            {
                S = other.S;
                T = other.T;
                return *this;
            }

            inline niTexCoord&  operator+=          (const niTexCoord &other)
            {
                S += other.S;
                T += other.T;
                return *this;
            }

            inline niTexCoord&  operator-=          (const niTexCoord &other)
            {
                S -= other.S;
                T -= other.T;
                return *this;
            }

            inline niTexCoord   operator+           (const niTexCoord &other) const
            {
                return niTexCoord(S+other.S, T+other.T);
            }

            inline niTexCoord   operator-           (const niTexCoord &other) const
            {
                return niTexCoord(S-other.S, T-other.T);
            }

            inline niTexCoord&  operator*=          (double v)
            {
                S *= v;
                T *= v;
                return *this;
            }

            inline niTexCoord&  operator/=          (double v)
            {
                S /= v;
                T /= v;
                return *this;
            }

            inline niTexCoord   operator*           (double v) const
            {
                return niTexCoord(S*v, T*v);
            }

            inline niTexCoord   operator/           (double v) const
            {
                return niTexCoord(S/v, T/v);
            }

            inline bool         operator==          (const niTexCoord &other) const
            {
                if (0 != niMath::_compare_(S, other.S))
                    return false;
                return niMath::_compare_(T, other.T) == 0;
            }

            inline bool         operator!=          (const niTexCoord &other) const
            {
                if (0 != niMath::_compare_(S, other.S))
                    return true;
                return 0 != niMath::_compare_(T, other.T);
            }

            inline bool         operator<           (const niTexCoord &other) const
            {
                int cmp = niMath::_compare_(S, other.S);
                if (-1 == cmp)
                    return true;
                if (1 == cmp)
                    return false;
                return niMath::_compare_(T, other.T) == -1;
            }

            inline bool         operator>           (const niTexCoord &other) const
            {
                int cmp = niMath::_compare_(S, other.S);
                if (1 == cmp)
                    return true;
                if (-1 == cmp)
                    return false;
                return niMath::_compare_(T, other.T) == 1;
            }

        public:
            double              S;
            double              T;
        };
    }
}

#endif
