//! \file
// \brief
// Polar class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niQuat3d_H
#define niQuat3d_H

#include <niGeom/math/niVector3d.h>

/**
 * \brief niQuat3d class
 *
 */
class niQuat3d
{
public:
    niQuat3d            () : m_w(0.0) {}
    niQuat3d            (double x, double y, double z, double w)
    {
        m_axis.Init(x, y, z);
        m_w = w;
    }
    niQuat3d            (const niVector3d &axis, double w)
    {
        m_axis = axis;
        m_w = w;
    }

    double              GetAngle            ()
    {
        return m_w;
    }

    void                SetAngle            (double w)
    {
        m_w = w;
    }

    const niVector3d&   GetAxis             () const
    {
        return m_axis;
    }

    niVector3d&         GetAxis             ()
    {
        return m_axis;
    }

    void                SetAxis             (double x, double y, double z)
    {
        m_axis.Init(x, y, z);
    }

    void                SetAxis             (const niVector3d &axis)
    {
        m_axis = axis;
    }

public:
    niVector3d          m_axis;
    double              m_w;
};

#endif
