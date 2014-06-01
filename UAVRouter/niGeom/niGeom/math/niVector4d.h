//! \file
// \brief
// Vector 4d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niVector4d_H
#define niVector4d_H

#include <niGeom/math/niMath.h>

/**
 * \brief niVector4d class
 *
 */
class niVector4d
{
public:
    niVector4d          () : A(0.0), B(0.0), C(0.0), D(0.0)    {}

    niVector4d          (double a, double b, double c, double d) : A(a), B(b), C(c), D(d){}

    niVector4d          (const niVector4d &other)
    {
        A = other.A;
        B = other.B;
        C = other.C;
        D = other.D;
    }

    inline double       Dot                 (const niVector4d &other) const
    {
        return A * other.A + B * other.B + C * other.C + D * other.D;
    }

    inline double       DistanceTo          (const niVector4d &other) const
    {
        return sqrt((A-other.A)*(A-other.A) + (B-other.B)*(B-other.B) + (C-other.C)*(C-other.C) + (D-other.D)*(D-other.D));
    }

    inline void         Init                (double a, double b, double c, double d)
    {
        A = a;
        B = b;
        C = c;
        D = d;
    }

    inline double       Length              () const
    {
        return sqrt(A*A + B*B + C*C + D*D);
    }

    inline void         Normalize           ()
    {
        double n2 = Length();
        if (niMath::_is_zero_(n2))
        {
            A = B = C = D = 0.0;
            return;
        }
        A /= n2;
        B /= n2;
        C /= n2;
        D /= n2;
    }

    inline double&      operator[]          (int idx)
    {
        if (0 == idx)
            return A;
        if (1 == idx)
            return B;
        if (2 == idx)
            return C;
        return D;
    }

    inline double       operator[]          (int idx) const
    {
        if (0 == idx)
            return A;
        if (1 == idx)
            return B;
        if (2 == idx)
            return C;
        return D;
    }

    inline niVector4d&  operator=           (const niVector4d &other)
    {
        A = other.A;
        B = other.B;
        C = other.C;
        D = other.D;
        return *this;
    }

    inline niVector4d&  operator+=          (const niVector4d &other)
    {
        A += other.A;
        B += other.B;
        C += other.C;
        D += other.D;
        return *this;
    }

    inline niVector4d&  operator-=          (const niVector4d &other)
    {
        A -= other.A;
        B -= other.B;
        C -= other.C;
        D -= other.D;
        return *this;
    }

    inline niVector4d   operator+           (const niVector4d &other) const
    {
        return niVector4d(A+other.A, B+other.B, C+other.C, D+other.D);
    }

    inline niVector4d   operator-           (const niVector4d &other) const
    {
        return niVector4d(A-other.A, B-other.B, C-other.C, D-other.D);
    }

    inline niVector4d&  operator*=          (double v)
    {
        A *= v;
        B *= v;
        C *= v;
        D *= v;
        return *this;
    }

    inline niVector4d&  operator/=          (double v)
    {
        A /= v;
        B /= v;
        C /= v;
        D /= v;
        return *this;
    }

    inline niVector4d   operator*           (double v) const
    {
        return niVector4d(A*v, B*v, C*v, D*v);
    }

    inline niVector4d   operator/           (double v) const
    {
        return niVector4d(A/v, B/v, C/v, D/v);
    }

    inline bool         operator==          (const niVector4d &other) const
    {
        if (0 != niMath::_compare_(A, other.A))
            return false;
        if (0 != niMath::_compare_(B, other.B))
            return false;
        if (0 != niMath::_compare_(C, other.C))
            return false;
        return niMath::_compare_(D, other.D) == 0;
    }

    inline bool         operator!=          (const niVector4d &other) const
    {
        if (0 != niMath::_compare_(A, other.A))
            return true;
        if (0 != niMath::_compare_(B, other.B))
            return true;
        if (0 != niMath::_compare_(C, other.C))
            return true;
        return 0 != niMath::_compare_(D, other.D);
    }

    inline bool         operator<           (const niVector4d &other) const
    {
        int cmp = niMath::_compare_(A, other.A);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        cmp = niMath::_compare_(B, other.B);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        cmp = niMath::_compare_(C, other.C);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        return niMath::_compare_(D, other.D) == -1;
    }

    inline bool         operator>           (const niVector4d &other) const
    {
        int cmp = niMath::_compare_(A, other.A);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        cmp = niMath::_compare_(B, other.B);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        cmp = niMath::_compare_(C, other.C);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        return niMath::_compare_(D, other.D) == 1;
    }

public:
    double              A;
    double              B;
    double              C;
    double              D;
};

#endif
