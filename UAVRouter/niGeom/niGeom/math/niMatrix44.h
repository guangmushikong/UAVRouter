//! \file
// \brief
// niMatrix44 class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niMatrix44_H
#define niMatrix44_H

#include <memory.h>
#include <niGeom/math/niVector3d.h>
#include <niGeom/math/niRotate3d.h>
#include <niGeom/math/niScale3d.h>
#include <niGeom/math/niTrans3d.h>
#include <niGeom/math/niMatrix33.h>

/**
 * \brief niMatrix44 class
 *
 */
class niMatrix44
{
public:
    niMatrix44                  () : C_ROW(4), C_COL(4), m_identity(false)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 16 );
    }

    niMatrix44                  (const niMatrix44 &other) : C_ROW(4), C_COL(4)
    {
        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 16 );
        m_identity = other.m_identity;
    }

    niMatrix44                  (double _mtx[]) : C_ROW(4), C_COL(4), m_identity(false)
    {
        memcpy( (void*)M, (void*)_mtx, sizeof(M[0]) * 16 );
    }

    niMatrix44                  (double a00, double a01, double a02, double a03
                                ,double a10, double a11, double a12, double a13
                                ,double a20, double a21, double a22, double a23
                                ,double a30, double a31, double a32, double a33)
                                : C_ROW(4), C_COL(4), m_identity(false)
    {
        M[ 0] = a00;    M[ 1] = a01;    M[ 2] = a02;    M[ 3] = a03;
        M[ 4] = a10;    M[ 5] = a11;    M[ 6] = a12;    M[ 7] = a13;
        M[ 8] = a20;    M[ 9] = a21;    M[10] = a22;    M[11] = a23;
        M[12] = a30;    M[13] = a31;    M[14] = a32;    M[15] = a33;
    }

    niMatrix44&                 operator =                  (const niMatrix44 &other)
    {
        if (this == &other)
            return *this;

        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 16 );
        m_identity = other.m_identity;
        return *this;
    }

    niMatrix44&                 operator =                  (double v)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 16 );
        M[0] = v;
        M[5] = v;
        M[10] = v;
        m_identity = (v == 1.0);
        return *this;
    }

    niMatrix44&                 operator +=                 (const niMatrix44 &other)
    {
        for (register int i = 0; i < 16; ++i)
        {
            M[i] += other.M[i];
        }
        m_identity = false;
        return *this;
    }

    niMatrix44&                 operator -=                 (const niMatrix44 &other)
    {
        for (register int i = 0; i < 16; ++i)
        {
            M[i] -= other.M[i];
        }
        m_identity = false;
        return *this;
    }

    
    niMatrix44                  operator +                  (const niMatrix44 &other) const
    {
        niMatrix44 rm;

        for (register int i = 0; i < 16; ++i)
        {
            rm.M[i] = M[i] + other.M[i];
        }
        rm.m_identity = false;
        return rm;
    }

    niMatrix44                  operator -                  (const niMatrix44 &other) const
    {
        niMatrix44 rm;

        for (register int i = 0; i < 16; ++i)
        {
            rm.M[i] = M[i] - other.M[i];
        }
        rm.m_identity = false;
        return rm;
    }
    niMatrix44&                 operator *=                 (double v)
    {
        for (register int i = 0; i < 16; ++i)
        {
            M[i] *= v;
        }

        if (v != 1.0)
        {
            m_identity = false;
        }
        return *this;
    }
    niMatrix44&                 operator /=                 (double v)
    {
        for (register int i = 0; i < 16; ++i)
        {
            M[i] /= v;
        }

        if (v != 1.0)
        {
            m_identity = false;
        }
        return *this;
    }
    
    niMatrix44                  operator *                  (double v) const
    {
        niMatrix44 rm;
        for (register int i = 0; i < 16; ++i)
        {
            rm.M[i] = M[i] * v;
        }
        rm.m_identity = m_identity;
        if (v != 1.0)
        {
            rm.m_identity = false;
        }
        return rm;
    }
    niMatrix44                  operator /                  (double v) const
    {
        niMatrix44 rm;
        for (register int i = 0; i < 16; ++i)
        {
            rm.M[i] = M[i] / v;
        }
        rm.m_identity = m_identity;
        if (v != 1.0)
        {
            rm.m_identity = false;
        }
        return rm;
    }
    niMatrix44                  operator *                  (const niMatrix44 &other) const
    {
        niMatrix44 rm;
        if (m_identity)
        {
            return other;
        }
        if (other.m_identity)
        {
            return *this;
        }
        rm.M[ 0] = M[ 0] * other.M[ 0] + M[ 1] * other.M[ 4] + M[ 2] * other.M[ 8] + M[ 3] * other.M[12];
        rm.M[ 1] = M[ 0] * other.M[ 1] + M[ 1] * other.M[ 5] + M[ 2] * other.M[ 9] + M[ 3] * other.M[13];
        rm.M[ 2] = M[ 0] * other.M[ 2] + M[ 1] * other.M[ 6] + M[ 2] * other.M[10] + M[ 3] * other.M[14];
        rm.M[ 3] = M[ 0] * other.M[ 3] + M[ 1] * other.M[ 7] + M[ 2] * other.M[11] + M[ 3] * other.M[15];

        rm.M[ 4] = M[ 4] * other.M[ 0] + M[ 5] * other.M[ 4] + M[ 6] * other.M[ 8] + M[ 7] * other.M[12];
        rm.M[ 5] = M[ 4] * other.M[ 1] + M[ 5] * other.M[ 5] + M[ 6] * other.M[ 9] + M[ 7] * other.M[13];
        rm.M[ 6] = M[ 4] * other.M[ 2] + M[ 5] * other.M[ 6] + M[ 6] * other.M[10] + M[ 7] * other.M[14];
        rm.M[ 7] = M[ 4] * other.M[ 3] + M[ 5] * other.M[ 7] + M[ 6] * other.M[11] + M[ 7] * other.M[15];

        rm.M[ 8] = M[ 8] * other.M[ 0] + M[ 9] * other.M[ 4] + M[10] * other.M[ 8] + M[11] * other.M[12];
        rm.M[ 9] = M[ 8] * other.M[ 1] + M[ 9] * other.M[ 5] + M[10] * other.M[ 9] + M[11] * other.M[13];
        rm.M[10] = M[ 8] * other.M[ 2] + M[ 9] * other.M[ 6] + M[10] * other.M[10] + M[11] * other.M[14];
        rm.M[11] = M[ 8] * other.M[ 3] + M[ 9] * other.M[ 7] + M[10] * other.M[11] + M[11] * other.M[15];

        rm.M[12] = M[12] * other.M[ 0] + M[13] * other.M[ 4] + M[14] * other.M[ 8] + M[15] * other.M[12];
        rm.M[13] = M[12] * other.M[ 1] + M[13] * other.M[ 5] + M[14] * other.M[ 9] + M[15] * other.M[13];
        rm.M[14] = M[12] * other.M[ 2] + M[13] * other.M[ 6] + M[14] * other.M[10] + M[15] * other.M[14];
        rm.M[15] = M[12] * other.M[ 3] + M[13] * other.M[ 7] + M[14] * other.M[11] + M[15] * other.M[15];
        return rm;
    }

    niVector3d                  operator *                  (const niVector3d &v) const
    {
        if (m_identity)
        {
            return v;
        }
        niVector3d rv;
        rv.X = M[ 0] * v.X + M[ 1] * v.Y + M[ 2] * v.Z + M[ 3];
        rv.Y = M[ 4] * v.X + M[ 5] * v.Y + M[ 6] * v.Z + M[ 7];
        rv.Z = M[ 8] * v.X + M[ 9] * v.Y + M[10] * v.Z + M[11];
        return rv;
    }

    double                      operator()                  (int row, int col) const
    {
        return M[row * C_COL + col];
    }

    double&                     operator()                  (int row, int col)
    {
        return M[row * C_COL + col];
    }

    double                      Det                         () const;

    bool                        Inverse                     ();

    void                        SetAll                      (double a00, double a01, double a02, double a03
                                                            ,double a10, double a11, double a12, double a13
                                                            ,double a20, double a21, double a22, double a23
                                                            ,double a30, double a31, double a32, double a33)
    {
        M[ 0] = a00;    M[ 1] = a01;    M[ 2] = a02;    M[ 3] = a03;
        M[ 4] = a10;    M[ 5] = a11;    M[ 6] = a12;    M[ 7] = a13;
        M[ 8] = a20;    M[ 9] = a21;    M[10] = a22;    M[11] = a23;
        M[12] = a30;    M[13] = a31;    M[14] = a32;    M[15] = a33;
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
        memset( (void*)M, 0, sizeof(M[0]) * 16 );
        M[ 0] = 1.0;
        M[ 3] = trans.X;
        M[ 5] = 1.0;
        M[ 7] = trans.Y;
        M[10] = 1.0;
        M[11] = trans.Z;
        M[15] = 1.0;
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
        memset( (void*)M, 0, sizeof(M[0]) * 16 );
        M[ 0] = scale.X;
        M[ 5] = scale.Y;
        M[10] = scale.Z;
        M[15] = 1.0;
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

    void                        Transpose                   ()
    {
        double S;
        S = M[ 1];  M[ 1] = M[ 4];  M[ 4] = S;
        S = M[ 2];  M[ 2] = M[ 8];  M[ 8] = S;
        S = M[ 3];  M[ 3] = M[12];  M[12] = S;
        S = M[ 6];  M[ 6] = M[ 9];  M[ 9] = S;
        S = M[ 7];  M[ 7] = M[13];  M[13] = S;
        S = M[11];  M[11] = M[14];  M[14] = S;
    }

    friend niMatrix44           operator *                  (double v, const niMatrix44 &m)
    {
        niMatrix44 rm;
        for (register int i = 0; i < 16; ++i)
        {
            rm.M[i] = m.M[i] * v;
        }
        rm.m_identity = m.m_identity;
        if (v != 1.0)
        {
            rm.m_identity = false;
        }
        return rm;
    }

protected:
    double                      M[16];
    const int                   C_ROW;
    const int                   C_COL;
    bool                        m_identity;
};

#endif
