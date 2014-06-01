//! \file
// \brief
// niTrans3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niTrans3d_H
#define niTrans3d_H

#include <niGeom/math/niVector3d.h>

/**
 * \brief niTrans3d class
 *
 */
class niTrans3d
{
public:
    niTrans3d           () : X(0.0), Y(0.0), Z(0.0) {}
    niTrans3d           (double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }
    niTrans3d           (const niVector3d &other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
    }
    niTrans3d           (const niTrans3d &other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
    }

    niVector3d          GetTrans            () const
    {
        return niVector3d(X, Y, Z);
    }

    void                GetTrans            (double &x, double &y, double &z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    void                SetTrans            (double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    void                SetTrans            (const niVector3d &other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
    }

    niTrans3d&          operator +=         (const niTrans3d &other)
    {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        return *this;
    }

    niTrans3d&          operator -=         (const niTrans3d &other)
    {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        return *this;
    }

    niTrans3d           operator +          (const niTrans3d &other) const
    {
        return niTrans3d(X + other.X, Y + other.Y, Z + other.Z);
    }

    niTrans3d           operator -          (const niTrans3d &other) const
    {
        return niTrans3d(X - other.X, Y - other.Y, Z - other.Z);
    }
    
    niTrans3d&          operator *=         (double v)
    {
        X *= v;
        Y *= v;
        Z *= v;
        return *this;
    }

    niTrans3d&          operator /=         (double v)
    {
        X /= v;
        Y /= v;
        Z /= v;
        return *this;
    }

    niTrans3d           operator *          (double v) const
    {
        return niTrans3d(X * v, Y * v, Z * v);
    }
    niTrans3d           operator /          (double v) const
    {
        return niTrans3d(X / v, Y / v, Z / v);
    }

public:
    double              X;
    double              Y;
    double              Z;
};

#endif
