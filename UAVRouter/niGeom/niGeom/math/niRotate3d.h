//! \file
// \brief
// niRotate3d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niRotate3d_H
#define niRotate3d_H

#include <niGeom/math/niVector3d.h>

enum ERotOrder
{
    XYZ = 1 << 0,
    XZY = 1 << 1,
    YXZ = 1 << 2,
    YZX = 1 << 3,
    ZXY = 1 << 4,
    ZYX = 1 << 5
};

/**
 * \brief niRotate3d class
 *
 */
class niRotate3d
{

public:
    niRotate3d          () : Y(0.0), P(0.0), R(0.0), RO(ZYX) {}
    niRotate3d          (double yaw
                        ,double pitch
                        ,double roll
                        ,ERotOrder ro=ZYX)
                        : Y(yaw), P(pitch), R(roll), RO(ro)
    {
    }

    niRotate3d          (const niVector3d &other, ERotOrder ro=ZYX) : RO(ro)
    {
        Y = other.X;
        P = other.Y;
        R = other.Z;
    }

    niRotate3d          (const niRotate3d &other)
    {
        Y = other.Y;
        P = other.P;
        R = other.R;
        RO = other.RO;
    }

    niVector3d          GetRotate           () const
    {
        return niVector3d(Y, P, R);
    }

    void                GetRotate           (double &yaw, double &pitch, double &roll)
    {
        yaw = Y;
        pitch = P;
        roll = R;
    }

    void                SetRotate           (double yaw, double pitch, double roll)
    {
        Y = yaw;
        P = pitch;
        R = roll;
    }

    void                SetRotate           (const niVector3d &other)
    {
        Y = other.X;
        P = other.Y;
        R = other.Z;
    }

    ERotOrder           GetRotOrder         () const
    {
        return RO;
    }

    void                SetRotOrder         (ERotOrder ro)
    {
        RO = ro;
    }

public:
    double              Y;
    double              P;
    double              R;
    ERotOrder           RO;
};

#endif
