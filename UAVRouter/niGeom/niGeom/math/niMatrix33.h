//! \file
// \brief
// niMatrix33 class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niMatrix33_H
#define niMatrix33_H

#include <memory.h>
#include <niGeom/math/niVector3d.h>
#include <niGeom/math/niRotate3d.h>
#include <niGeom/math/niScale3d.h>

class niMatrix34;
class niMatrix44;

/**
 * \brief niMatrix33 class
 *
 */
class niMatrix33
{
public:
    niMatrix33                  () : C_ROW(3), C_COL(3), m_identity(false)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 9 );
    }

    niMatrix33                  (double v) : C_ROW(3), C_COL(3)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 9 );
        M[0] = v;   M[4] = v;   M[8] = v;
        m_identity = (v == 1.0);
    }

    niMatrix33                  (const niMatrix33 &other) : C_ROW(3), C_COL(3)
    {
        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 9 );
        m_identity = other.m_identity;
    }

    niMatrix33                  (double _mtx[]) : C_ROW(3), C_COL(3)
    {
        memcpy( (void*)M, (void*)_mtx, sizeof(M[0]) * 9 );
        m_identity = false;
    }

    niMatrix33                  (double a00, double a01, double a02
                                ,double a10, double a11, double a12
                                ,double a20, double a21, double a22)
                                : C_ROW(3), C_COL(3), m_identity(false)
    {
        M[0] = a00;     M[1] = a01;     M[2] = a02;
        M[3] = a10;     M[4] = a11;     M[5] = a12;
        M[6] = a20;     M[7] = a21;     M[8] = a22;
    }

    niMatrix33&                 operator =                  (const niMatrix33 &other)
    {
        if (this == &other)
            return *this;

        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 9 );
        m_identity = other.m_identity;
        return *this;
    }

    niMatrix33&                 operator =                  (double v)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 9 );
        M[0] = v;   M[4] = v;   M[8] = v;
        m_identity = (v == 1.0);
        return *this;
    }

    niMatrix33&                 operator +=                 (const niMatrix33 &other)
    {
        for (register int i = 0; i < 9; ++i)
        {
            M[i] += other.M[i];
        }
        m_identity = false;
        return *this;
    }

    niMatrix33&                 operator -=                 (const niMatrix33 &other)
    {
        for (register int i = 0; i < 9; ++i)
        {
            M[i] -= other.M[i];
        }
        m_identity = false;
        return *this;
    }

    
    niMatrix33                  operator +                  (const niMatrix33 &other) const
    {
        niMatrix33 rm;

        for (register int i = 0; i < 9; ++i)
        {
            rm.M[i] = M[i] + other.M[i];
        }
        rm.m_identity = false;
        return rm;
    }

    niMatrix33                  operator -                  (const niMatrix33 &other) const
    {
        niMatrix33 rm;

        for (register int i = 0; i < 9; ++i)
        {
            rm.M[i] = M[i] - other.M[i];
        }
        rm.m_identity = false; 
        return rm;
    }

    niMatrix33&                 operator *=                 (double v)
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            for (register int i = 0; i < 9; ++i)
            {
                M[i] *= v;
            }
            m_identity = false;
            return *this;
        }
    }
    niMatrix33&                 operator /=                 (double v)
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            for (register int i = 0; i < 9; ++i)
            {
                M[i] /= v;
            }
            m_identity = false;
            return *this;
        }
    }
    
    niMatrix33                  operator *                  (double v) const
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            niMatrix33 rm;
            for (register int i = 0; i < 9; ++i)
            {
                rm.M[i] = M[i] * v;
            }
            return rm;
        }
    }
    niMatrix33                  operator /                  (double v) const
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            niMatrix33 rm;
            for (register int i = 0; i < 9; ++i)
            {
                rm.M[i] = M[i] / v;
            }
            return rm;
        }
    }

    niMatrix33                  operator *                  (const niMatrix33 &other) const
    {
        if (m_identity)
        {
            return other;
        }
        if (other.m_identity)
        {
            return *this;
        }
        niMatrix33 rm;
        rm.M[0] = M[0] * other.M[0] + M[1] * other.M[3] + M[2] * other.M[6];
        rm.M[1] = M[0] * other.M[1] + M[1] * other.M[4] + M[2] * other.M[7];
        rm.M[2] = M[0] * other.M[2] + M[1] * other.M[5] + M[2] * other.M[8];
        rm.M[3] = M[3] * other.M[0] + M[4] * other.M[3] + M[5] * other.M[6];
        rm.M[4] = M[3] * other.M[1] + M[4] * other.M[4] + M[5] * other.M[7];
        rm.M[5] = M[3] * other.M[2] + M[4] * other.M[5] + M[5] * other.M[8];
        rm.M[6] = M[6] * other.M[0] + M[7] * other.M[3] + M[8] * other.M[6];
        rm.M[7] = M[6] * other.M[1] + M[7] * other.M[4] + M[8] * other.M[7];
        rm.M[8] = M[6] * other.M[2] + M[7] * other.M[5] + M[8] * other.M[8];
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
            rv.X = M[0] * v.X + M[1] * v.Y + M[2] * v.Z;
            rv.Y = M[3] * v.X + M[4] * v.Y + M[5] * v.Z;
            rv.Z = M[6] * v.X + M[7] * v.Y + M[8] * v.Z;
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

    double                      Det                         () const;

    bool                        Inverse                     ();

    void                        Orthogonalization           ();

    void                        SetAll                      (double a00, double a01, double a02
                                                            ,double a10, double a11, double a12
                                                            ,double a20, double a21, double a22)
    {
        M[0] = a00;     M[1] = a01;     M[2] = a02;
        M[3] = a10;     M[4] = a11;     M[5] = a12;
        M[6] = a20;     M[7] = a21;     M[8] = a22;
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
        memset( (void*)M, 0, sizeof(M[0]) * 9 );
        M[0] = scale.X;
        M[4] = scale.Y;
        M[8] = scale.Z;
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
        S = M[1];    M[1] = M[3];    M[3] = S;
        S = M[2];    M[2] = M[6];    M[6] = S;
        S = M[5];    M[5] = M[7];    M[7] = S;
    }

    friend niMatrix33           operator *                  (double v, const niMatrix33 &m)
    {
        if (v == 1.0)
        {
            return m;
        }
        else
        {
            niMatrix33 rm;
            for (register int i = 0; i < 9; ++i)
            {
                rm.M[i] = m.M[i] * v;
            }
            return rm;
        }
    }

    friend class niMatrix34;
    friend class niMatrix44;

protected:
    double                      M[9];
    const int                   C_ROW;
    const int                   C_COL;
    bool                        m_identity;
};

#endif
