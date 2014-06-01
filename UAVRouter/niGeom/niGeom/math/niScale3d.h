//! \file
// \brief
// niScale3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niScale3d_H
#define niScale3d_H

#include <niGeom/math/niVector3d.h>

/**
 * \brief niScale3d class
 *
 */
class niScale3d
{
public:
    niScale3d           () : X(0.0), Y(0.0), Z(0.0) {}
    niScale3d           (double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }
    niScale3d           (const niVector3d &ohter)
    {
        X = ohter.X;
        Y = ohter.Y;
        Z = ohter.Z;
    }
    niScale3d           (const niScale3d &ohter)
    {
        X = ohter.X;
        Y = ohter.Y;
        Z = ohter.Z;
    }

    niVector3d          GetScale            () const
    {
        return niVector3d(X, Y, Z);
    }

    void                GetScale            (double &x, double &y, double &z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    void                SetScale            (double x, double y, double z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    void                SetScale            (const niVector3d &ohter)
    {
        X = ohter.X;
        Y = ohter.Y;
        Z = ohter.Z;
    }
    niScale3d&          operator *=         (double v)
    {
        X *= v;
        Y *= v;
        Z *= v;
        return *this;
    }

    niScale3d&          operator /=         (double v)
    {
        X /= v;
        Y /= v;
        Z /= v;
        return *this;
    }

    niScale3d           operator *          (double v) const
    {
        return niScale3d(X * v, Y * v, Z * v);
    }
    niScale3d           operator /          (double v) const
    {
        return niScale3d(X / v, Y / v, Z / v);
    }


public:
    double              X;
    double              Y;
    double              Z;
};

#endif
