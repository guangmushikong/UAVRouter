//! \file
// \brief
// Vector 3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niVector3d_H
#define niVector3d_H

#include <niGeom/math/niMath.h>

/**
 * \brief niVector3d class
 *
 */
class niVector3d
{
public:
    niVector3d          () : X(0.0), Y(0.0), Z(0.0)    {}

    niVector3d          (double x, double y, double z) : X(x), Y(y), Z(z)    {}

    niVector3d          (const niVector3d &other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
    }

    inline double       Dot                 (const niVector3d &other) const
    {
        return X * other.X + Y * other.Y + Z * other.Z;
    }

    inline niVector3d   Cross               (const niVector3d &other) const
    {
        double x = Y * other.Z - Z * other.Y;
        double y = Z * other.X - X * other.Z;
        double z = X * other.Y - Y * other.X;
        return niVector3d(x, y, z);
    }
    
    inline double       DistanceTo          (const niVector3d &other) const
    {
        return sqrt((X-other.X) * (X-other.X) + (Y-other.Y) * (Y-other.Y) + (Z-other.Z) * (Z-other.Z));
    }

    inline double       DistanceTo          (double x, double y, double z) const
    {
        x -= X;     y -= Y;     z -= Z;
        return sqrt(x * x + y * y + z * z);
    }

    inline void         Init                (double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    inline double       Length              () const
    {
        return sqrt(X*X + Y*Y + Z*Z);
    }

    inline void         Normalize           ()
    {
        double n2 = Length();
        if (niMath::_is_zero_(n2))
        {
            X = Y = Z = 0.0;
            return;
        }
        X /= n2;
        Y /= n2;
        Z /= n2;
    }

    inline double&      operator[]          (int idx)
    {
        if (0 == idx)
            return X;
        if (1 == idx)
            return Y;
        return Z;
    }

    inline double       operator[]          (int idx) const
    {
        if (0 == idx)
            return X;
        if (1 == idx)
            return Y;
        return Z;
    }

    inline niVector3d&  operator=           (const niVector3d &other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        return *this;
    }

    inline niVector3d&  operator+=          (const niVector3d &other)
    {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        return *this;
    }

    inline niVector3d&  operator-=          (const niVector3d &other)
    {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        return *this;
    }

    inline niVector3d   operator+           (const niVector3d &other) const
    {
        return niVector3d(X+other.X, Y+other.Y, Z+other.Z);
    }

    inline niVector3d   operator-           (const niVector3d &other) const
    {
        return niVector3d(X-other.X, Y-other.Y, Z-other.Z);
    }

    inline niVector3d&  operator*=          (double v)
    {
        X *= v;
        Y *= v;
        Z *= v;
        return *this;
    }

    inline niVector3d&  operator/=          (double v)
    {
        X /= v;
        Y /= v;
        Z /= v;
        return *this;
    }

    inline niVector3d   operator*           (double v) const
    {
        return niVector3d(X*v, Y*v, Z*v);
    }

    inline niVector3d   operator/           (double v) const
    {
        return niVector3d(X/v, Y/v, Z/v);
    }

    inline bool         operator==          (const niVector3d &other) const
    {
        if (0 != niMath::_compare_(X, other.X))
            return false;
        if (0 != niMath::_compare_(Y, other.Y))
            return false;
        return niMath::_compare_(Z, other.Z) == 0;
    }

    inline bool         operator!=          (const niVector3d &other) const
    {
        if (0 != niMath::_compare_(X, other.X))
            return true;
        if (0 != niMath::_compare_(Y, other.Y))
            return true;
        return 0 != niMath::_compare_(Z, other.Z);
    }

    inline bool         operator<           (const niVector3d &other) const
    {
        int cmp = niMath::_compare_(X, other.X);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        cmp = niMath::_compare_(Y, other.Y);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        return niMath::_compare_(Z, other.Z) == -1;
    }

    inline bool         operator>           (const niVector3d &other) const
    {
        int cmp = niMath::_compare_(X, other.X);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        cmp = niMath::_compare_(Y, other.Y);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        return niMath::_compare_(Z, other.Z) == 1;
    }

public:
    double              X;
    double              Y;
    double              Z;
};

#endif
