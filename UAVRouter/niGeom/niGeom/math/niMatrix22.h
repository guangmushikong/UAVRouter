//! \file
// \brief
// niMatrix22 class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version


#ifndef niMatrix22_H
#define niMatrix22_H

#include <memory.h>
#include <niGeom/math/niVector2d.h>

/**
 * \brief niMatrix22 class
 *
 */
class niMatrix22
{
public:
    niMatrix22                  () : C_ROW(2), C_COL(2), m_identity(false)
    {
        memset( (void*)M, 0, sizeof(M[0]) * 4 );
    }

    niMatrix22                  (double v) : C_ROW(2), C_COL(2)
    {
        M[0] = v;       M[1] = 0.0;
        M[2] = 0.0;     M[3] = v;
        m_identity = (v == 1.0);
    }

    niMatrix22                  (const niMatrix22 &other) : C_ROW(2), C_COL(2)
    {
        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 4 );
        m_identity = other.m_identity;
    }

    niMatrix22                  (double _mtx[]) : C_ROW(3), C_COL(3)
    {
        memcpy( (void*)M, (void*)_mtx, sizeof(M[0]) * 4 );
        m_identity = false;
    }

    niMatrix22                  (double a00, double a01
                                ,double a10, double a11)
                                : C_ROW(3), C_COL(3), m_identity(false)
    {
        M[0] = a00;     M[1] = a01;
        M[2] = a10;     M[3] = a11;
    }

    niMatrix22&                 operator =                  (const niMatrix22 &other)
    {
        if (this == &other)
            return *this;

        memcpy( (void*)M, (void*)other.M, sizeof(M[0]) * 4 );
        m_identity = other.m_identity;
        return *this;
    }

    niMatrix22&                 operator =                  (double v)
    {
        M[0] = v;       M[1] = 0.0;
        M[2] = 0.0;     M[3] = v;
        m_identity = (v == 1.0);
        return *this;
    }

    niMatrix22&                 operator +=                 (const niMatrix22 &other)
    {
        for (register int i = 0; i < 4; ++i)
        {
            M[i] += other.M[i];
        }
        m_identity = false;
        return *this;
    }

    niMatrix22&                 operator -=                 (const niMatrix22 &other)
    {
        for (register int i = 0; i < 4; ++i)
        {
            M[i] -= other.M[i];
        }
        m_identity = false;
        return *this;
    }

    
    niMatrix22                  operator +                  (const niMatrix22 &other) const
    {
        niMatrix22 rm;

        for (register int i = 0; i < 4; ++i)
        {
            rm.M[i] = M[i] + other.M[i];
        }
        rm.m_identity = false;
        return rm;
    }

    niMatrix22                  operator -                  (const niMatrix22 &other) const
    {
        niMatrix22 rm;

        for (register int i = 0; i < 4; ++i)
        {
            rm.M[i] = M[i] - other.M[i];
        }
        rm.m_identity = false; 
        return rm;
    }

    niMatrix22&                 operator *=                 (double v)
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            for (register int i = 0; i < 4; ++i)
            {
                M[i] *= v;
            }
            m_identity = false;
            return *this;
        }
    }
    niMatrix22&                 operator /=                 (double v)
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            for (register int i = 0; i < 4; ++i)
            {
                M[i] /= v;
            }
            m_identity = false;
            return *this;
        }
    }
    
    niMatrix22                  operator *                  (double v) const
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            niMatrix22 rm;
            for (register int i = 0; i < 4; ++i)
            {
                rm.M[i] = M[i] * v;
            }
            return rm;
        }
    }
    niMatrix22                  operator /                  (double v) const
    {
        if (v == 1.0)
        {
            return *this;
        }
        else
        {
            niMatrix22 rm;
            for (register int i = 0; i < 4; ++i)
            {
                rm.M[i] = M[i] / v;
            }
            return rm;
        }
    }

    niMatrix22                  operator *                  (const niMatrix22 &other) const
    {
        if (m_identity)
        {
            return other;
        }
        if (other.m_identity)
        {
            return *this;
        }
        niMatrix22 rm;
        rm.M[0] = M[0] * other.M[0] + M[1] * other.M[2];
        rm.M[1] = M[0] * other.M[1] + M[1] * other.M[3];
        rm.M[2] = M[2] * other.M[0] + M[3] * other.M[2];
        rm.M[3] = M[2] * other.M[1] + M[3] * other.M[3];
        return rm;
    }

    niVector2d                  operator *                  (const niVector2d &v) const
    {
        if (m_identity)
        {
            return v;
        }
        else
        {
            niVector2d rv;
            rv.X = M[0] * v.X + M[1] * v.Y;
            rv.Y = M[2] * v.X + M[3] * v.Y;
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

    double                      Det                         () const
    {
        return M[0] * M[3] - M[1] * M[2];
    }

    
    void                        FromEulerAngle              (double angle)
    {
        M[0] = M[3] = cos(angle);
        M[2] = sin(angle);
        M[1] = -M[2];
    }

    bool                        Inverse                     ()
    {
        double det = Det();
        if (fabs(det) < ZEROF)
            return false;

        M[0] += M[3];
        M[3] = M[0] - M[3];
        M[0] -= M[3];
        
        M[1] = -M[1];
        M[2] = -M[2];
        
        M[0] /= det;
        M[1] /= det;
        M[2] /= det;
        M[3] /= det;

        return true;
    }

    void                        SetAll                      (double a00, double a01
                                                            ,double a10, double a11)
    {
        M[0] = a00;     M[1] = a01;
        M[2] = a10;     M[3] = a11;
        m_identity = false;
    }


    void                        Transpose                   ()
    {
        double S;
        S = M[1];    M[1] = M[2];    M[2] = S;
    }

    friend niMatrix22           operator *                  (double v, const niMatrix22 &m)
    {
        if (v == 1.0)
        {
            return m;
        }
        else
        {
            niMatrix22 rm;
            for (register int i = 0; i < 4; ++i)
            {
                rm.M[i] = m.M[i] * v;
            }
            return rm;
        }
    }

    friend class niMatrix34;
    friend class niMatrix44;

protected:
    double                      M[2];
    const int                   C_ROW;
    const int                   C_COL;
    bool                        m_identity;
};

#endif
