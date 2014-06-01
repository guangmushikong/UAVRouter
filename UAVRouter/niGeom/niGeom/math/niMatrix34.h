//! \file
// \brief
// matrix33 class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niMatrix34_H
#define niMatrix34_H

#include <memory.h>
#include <niGeom/math/niVector3d.h>
#include <niGeom/math/niRotate3d.h>
#include <niGeom/math/niScale3d.h>
#include <niGeom/math/niTrans3d.h>
#include <niGeom/math/niMatrix33.h>

/**
 * \brief niMatrix34 class
 *
 */
class niMatrix34
{
public:
    niMatrix34                  () : C_ROW(3), C_COL(4), m_identity(false)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 12 );
    }

    niMatrix34                  (const niMatrix34 &other) : C_ROW(3), C_COL(4)
    {
        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 12 );
        m_identity = other.m_identity;
    }

    niMatrix34                  (double _mtx[]) : C_ROW(3), C_COL(4), m_identity(false)
    {
        memcpy( (void*)M, (void*)_mtx, sizeof(M[0]) * 12 );
    }

    niMatrix34                  (double a00, double a01, double a02, double a03
                                ,double a10, double a11, double a12, double a13
                                ,double a20, double a21, double a22, double a23)
                                : C_ROW(3), C_COL(4), m_identity(false)
    {
        M[ 0] = a00;    M[ 1] = a01;    M[ 2] = a02;    M[ 3] = a03;
        M[ 4] = a10;    M[ 5] = a11;    M[ 6] = a12;    M[ 7] = a13;
        M[ 8] = a20;    M[ 9] = a21;    M[10] = a22;    M[11] = a23;
    }

    niMatrix34&                 operator =                  (const niMatrix34 &other)
    {
        if (this == &other)
            return *this;

        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 12 );
        m_identity = other.m_identity;
        return *this;
    }

    niMatrix34&                 operator =                  (double v)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 12 );
        M[0] = v;
        M[5] = v;
        M[10] = v;
        m_identity = (v == 1.0);
        return *this;
    }

    niMatrix34&                 operator +=                 (const niMatrix34 &other)
    {
        for (register int i = 0; i < 12; ++i)
        {
            M[i] += other.M[i];
        }
        m_identity = false;
        return *this;
    }

    niMatrix34&                 operator -=                 (const niMatrix34 &other)
    {
        for (register int i = 0; i < 12; ++i)
        {
            M[i] -= other.M[i];
        }
        m_identity = false;
        return *this;
    }

    
    niMatrix34                  operator +                  (const niMatrix34 &other) const
    {
        niMatrix34 rm;

        for (register int i = 0; i < 12; ++i)
        {
            rm.M[i] = M[i] + other.M[i];
        }
        rm.m_identity = false;
        return rm;
    }

    niMatrix34                  operator -                  (const niMatrix34 &other) const
    {
        niMatrix34 rm;

        for (register int i = 0; i < 12; ++i)
        {
            rm.M[i] = M[i] - other.M[i];
        }
        rm.m_identity = false;
        return rm;
    }

    niMatrix34&                 operator *=                 (double v)
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            for (register int i = 0; i < 12; ++i)
            {
                M[i] *= v;
            }
            m_identity = false;
            return *this;
        }
    }
    niMatrix34&                 operator /=                 (double v)
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            for (register int i = 0; i < 12; ++i)
            {
                M[i] /= v;
            }
            m_identity = false;
            return *this;
        }
    }
    
    niMatrix34                  operator *                  (double v) const
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            niMatrix34 rm;
            for (register int i = 0; i < 12; ++i)
            {
                rm.M[i] = M[i] * v;
            }
            return rm;
        }
    }
    niMatrix34                  operator /                  (double v) const
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            niMatrix34 rm;
            for (register int i = 0; i < 12; ++i)
            {
                rm.M[i] = M[i] / v;
            }
            return rm;
        }
    }

    niMatrix34                  operator *                  (const niMatrix34 &other) const
    {
        if (m_identity)
        {
            return other;
        }
        if (other.m_identity)
        {
            return *this;
        }
        niMatrix34 rm;
        rm.M[ 0] = M[ 0] * other.M[ 0] + M[ 1] * other.M[ 4] + M[ 2] * other.M[ 8];
        rm.M[ 1] = M[ 0] * other.M[ 1] + M[ 1] * other.M[ 5] + M[ 2] * other.M[ 9];
        rm.M[ 2] = M[ 0] * other.M[ 2] + M[ 1] * other.M[ 6] + M[ 2] * other.M[10];
        rm.M[ 3] = M[ 0] * other.M[ 3] + M[ 1] * other.M[ 7] + M[ 2] * other.M[11] + M[ 3];

        rm.M[ 4] = M[ 4] * other.M[ 0] + M[ 5] * other.M[ 4] + M[ 6] * other.M[ 8];
        rm.M[ 5] = M[ 4] * other.M[ 1] + M[ 5] * other.M[ 5] + M[ 6] * other.M[ 9];
        rm.M[ 6] = M[ 4] * other.M[ 2] + M[ 5] * other.M[ 6] + M[ 6] * other.M[10];
        rm.M[ 7] = M[ 4] * other.M[ 3] + M[ 5] * other.M[ 7] + M[ 6] * other.M[11] + M[ 7];

        rm.M[ 8] = M[ 8] * other.M[ 0] + M[ 9] * other.M[ 4] + M[10] * other.M[ 8];
        rm.M[ 9] = M[ 8] * other.M[ 1] + M[ 9] * other.M[ 5] + M[10] * other.M[ 9];
        rm.M[10] = M[ 8] * other.M[ 2] + M[ 9] * other.M[ 6] + M[10] * other.M[10];
        rm.M[11] = M[ 8] * other.M[ 3] + M[ 9] * other.M[ 7] + M[10] * other.M[11] + M[11];
        return rm;
    }

    niVector3d                  operator *                  (const niVector3d &v) const
    {
        if (m_identity)
        {
            return v;
        }
        else
        {
            niVector3d rv;
            rv.X = M[ 0] * v.X + M[ 1] * v.Y + M[ 2] * v.Z + M[ 3];
            rv.Y = M[ 4] * v.X + M[ 5] * v.Y + M[ 6] * v.Z + M[ 7];
            rv.Z = M[ 8] * v.X + M[ 9] * v.Y + M[10] * v.Z + M[11];
            return rv;
        }
    }

    double                      operator()                  (int row, int col) const
    {
        return M[row * C_COL + col];
    }

    double&                     operator()                  (int row, int col)
    {
        return M[row * C_COL + col];
    }

    void                        SetAll                      (double a00, double a01, double a02, double a03
                                                            ,double a10, double a11, double a12, double a13
                                                            ,double a20, double a21, double a22, double a23)
    {
        M[ 0] = a00;    M[ 1] = a01;    M[ 2] = a02;    M[ 3] = a03;
        M[ 4] = a10;    M[ 5] = a11;    M[ 6] = a12;    M[ 7] = a13;
        M[ 8] = a20;    M[ 9] = a21;    M[10] = a22;    M[11] = a23;
        m_identity = false;
    }

    void                        GetMatrix33                 (niMatrix33 &m33)
    {
        m33.M[0] = M[0];    m33.M[1] = M[1];    m33.M[2] = M[ 2];
        m33.M[3] = M[4];    m33.M[4] = M[5];    m33.M[5] = M[ 6];
        m33.M[6] = M[8];    m33.M[7] = M[9];    m33.M[8] = M[10];
    }

    void                        GetTrans                    (niTrans3d &trans)
    {
        trans.X = M[3];
        trans.Y = M[7];
        trans.Z = M[11];
    }

    void                        MakeTrans                   (const niTrans3d &trans)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 12 );
        M[ 0] = 1.0;
        M[ 3] = trans.X;
        M[ 5] = 1.0;
        M[ 7] = trans.Y;
        M[10] = 1.0;
        M[11] = trans.Z;
        m_identity = false;
    }

    void                        GetScale                    (niScale3d &scale)
    {
        scale.X = sqrt( M[0] * M[0] + M[3] * M[3] + M[6] * M[6] );
        scale.Y = sqrt( M[1] * M[1] + M[4] * M[4] + M[7] * M[7] );
        scale.Z = sqrt( M[2] * M[2] + M[5] * M[5] + M[8] * M[8] );
    }

    void                        MakeScale                   (const niScale3d &scale)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 12 );
        M[ 0] = scale.X;
        M[ 5] = scale.Y;
        M[10] = scale.Z;
        m_identity = (scale.X==1.0 && scale.Y==1.0 && scale.Z==1.0);
    }

    void                        FromAxisAngle               (const niVector3d& axis
                                                            ,double angle);

    void                        ToAxisAngle                 (niVector3d& axis
                                                            ,double& angle) const;


    bool                        ToEulerAnglesXYZ            (niRotate3d &rot) const;
    bool                        ToEulerAnglesXZY            (niRotate3d &rot) const;
    bool                        ToEulerAnglesYXZ            (niRotate3d &rot) const;
    bool                        ToEulerAnglesYZX            (niRotate3d &rot) const;
    bool                        ToEulerAnglesZXY            (niRotate3d &rot) const;
    bool                        ToEulerAnglesZYX            (niRotate3d &rot) const;

    void                        FromEulerAnglesXYZ          (const niRotate3d &rot);
    void                        FromEulerAnglesXZY          (const niRotate3d &rot);
    void                        FromEulerAnglesYXZ          (const niRotate3d &rot);
    void                        FromEulerAnglesYZX          (const niRotate3d &rot);
    void                        FromEulerAnglesZXY          (const niRotate3d &rot);
    void                        FromEulerAnglesZYX          (const niRotate3d &rot);

    friend niMatrix34           operator *                  (double v, const niMatrix34 &m)
    {
        if (v == 1.0)
        {
            return m;
        }
        else
        {
            niMatrix34 rm;
            for (register int i = 0; i < 12; ++i)
            {
                rm.M[i] = m.M[i] * v;
            }
            return rm;
        }
    }

protected:
    double                      M[12];
    const int                   C_ROW;
    const int                   C_COL;
    bool                        m_identity;
};

#endif
