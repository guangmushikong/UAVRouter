//! \file
// \brief
// Polar class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niPolar2d_H
#define niPolar2d_H

#include <niGeom/math/niMath.h>

/**
 * \brief niPolar2d class
 *
 */
class niPolar2d
{
public:
    niPolar2d           () : Radius(0.0), Theta(0.0)    {}

    niPolar2d           (double r, double t) : Radius(r), Theta(t)    {}

    niPolar2d           (const niPolar2d &p)
    {
        Radius  = p.Radius;
        Theta   = p.Theta;
    }

    inline void         Init                (double r, double t)
    {
        Radius  = r;
        Theta   = t;
    }

    inline bool         operator==          (const niPolar2d &p) const
    {
        if (0 != niMath::_compare_(Radius, p.Radius))
            return false;
        return niMath::_compare_(Theta, p.Theta) == 0;
    }

    inline bool         operator!=          (const niPolar2d &p) const
    {
        if (0 != niMath::_compare_(Radius, p.Radius))
            return true;
        return 0 != niMath::_compare_(Theta, p.Theta);
    }

    inline bool         operator<           (const niPolar2d &p) const
    {
        int cmp = niMath::_compare_(Radius, p.Radius);
        if (-1 == cmp)
            return true;
        if (1 == cmp)
            return false;
        return niMath::_compare_(Theta, p.Theta) == -1;
    }

    inline bool         operator>           (const niPolar2d &p) const
    {
        int cmp = niMath::_compare_(Radius, p.Radius);
        if (1 == cmp)
            return true;
        if (-1 == cmp)
            return false;
        return niMath::_compare_(Theta, p.Theta) == 1;
    }

public:
    double              Radius;
    double              Theta;
};

#endif
