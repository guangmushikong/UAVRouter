//! \file
// \brief
// Vector 2d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niVector2d_H
#define niVector2d_H

#include <niGeom/math/niMath.h>

/**
 * \brief niVector2d class
 *
 */
class niVector2d
{
public:
    niVector2d          () : X(0.0), Y(0.0)    {}

    niVector2d          (double x, double y) : X(x), Y(y)    {}

    niVector2d          (const niVector2d &other)
    {
        X = other.X;
        Y = other.Y;
    }
    
    inline double       Dot                 (const niVector2d &other) const
    {
        return X * other.X + Y * other.Y;
    }

    inline double       Dot                 (double x1, double y1) const
    {
        return X * x1 + Y * y1;
    }
    
    inline double       Cross               (const niVector2d &other) const
    {
        return X * other.Y - Y * other.X;
    }

    inline double       Cross               (double x1, double y1) const
    {
        return X * y1 - Y * x1;
    }

    inline double       DistanceTo          (const niVector2d &other) const
    {
        return sqrt((X-other.X) * (X-other.X) + (Y-other.Y) * (Y-other.Y));
    }


    inline double       DistanceTo          (double x, double y) const
    {
        return sqrt((X-x) * (X-x) + (Y-y) * (Y-y));
    }

    inline void         Init                (double x, double y)
    {
        X = x;
        Y = y;
    }

    inline double       Length              () const
    {
        return sqrt(X*X + Y*Y);
    }

    inline void         Normalize           ()
    {
        double n2 = Length();
        if (niMath::_is_zero_(n2))
        {
            X = Y = 0.0;
            return;
        }
        X /= n2;
        Y /= n2;
    }

    inline double       operator[]          (int idx) const
    {
        return idx == 0 ? X : Y;
    }

    inline niVector2d&  operator=           (const niVector2d &other)
    {
        X = other.X;
        Y = other.Y;
        return *this;
    }

    inline niVector2d&  operator+=          (const niVector2d &other)
    {
        X += other.X;
        Y += other.Y;
        return *this;
    }

    inline niVector2d&  operator-=          (const niVector2d &other)
    {
        X -= other.X;
        Y -= other.Y;
        return *this;
    }

    inline niVector2d   operator+           (const niVector2d &other) const
    {
        return niVector2d(X+other.X, Y+other.Y);
    }

    inline niVector2d   operator-           (const niVector2d &other) const
    {
        return niVector2d(X-other.X, Y-other.Y);
    }

    inline niVector2d&  operator*=          (double v)
    {
        X *= v;
        Y *= v;
        return *this;
    }

    inline niVector2d&  operator/=          (double v)
    {
        X /= v;
        Y /= v;
        return *this;
    }

    inline niVector2d   operator*           (double v) const
    {
        return niVector2d(X*v, Y*v);
    }

    inline niVector2d   operator/           (double v) const
    {
        return niVector2d(X/v, Y/v);
    }

    inline bool         operator==          (const niVector2d &other) const
    {
        //return X == other.X && Y == other.Y;
        //*
        if (0 != niMath::_compare_(X, other.X))
            return false;
        return niMath::_compare_(Y, other.Y) == 0;
        //*/
    }

    inline bool         operator!=          (const niVector2d &other) const
    {
        //return X != other.X || Y != other.Y;
        //*
        if (0 != niMath::_compare_(X, other.X))
            return true;
        return 0 != niMath::_compare_(Y, other.Y);
        //*/
    }

    inline bool         operator<           (const niVector2d &other) const
    {
        if (X < other.X)
            return true;
        if (X > other.X)
            return false;
        return Y < other.Y;
        /*
        int cmp = niMath::_compare_(X, other.X);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        return niMath::_compare_(Y, other.Y) == -1;
        */
    }

    inline bool         operator>           (const niVector2d &other) const
    {
        if (X > other.X)
            return true;
        if (X < other.X)
            return false;
        return Y > other.Y;

        /*
        int cmp = niMath::_compare_(X, other.X);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        return niMath::_compare_(Y, other.Y) == 1;
        */
    }

public:
    double              X;
    double              Y;
};

#endif
