//! \file
// \brief
// niMatrix33 class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#include <niGeom/math/niMatrix33.h>

double niMatrix33::Det() const
{
    double A0 = M[4] * M[8] - M[5] * M[7];
    double A1 = M[5] * M[6] - M[3] * M[8];
    double A2 = M[3] * M[7] - M[4] * M[6];

    double fDet = M[0] * A0 + M[1] * A1 + M[2] * A2;
    return fDet;
}


bool niMatrix33::Inverse()
{
    double A[9];
    A[0] = M[4] * M[8] - M[5] * M[7];
    A[1] = M[2] * M[7] - M[1] * M[8];
    A[2] = M[1] * M[5] - M[2] * M[4];
    A[3] = M[5] * M[6] - M[3] * M[8];
    A[4] = M[0] * M[8] - M[2] * M[6];
    A[5] = M[2] * M[3] - M[0] * M[5];
    A[6] = M[3] * M[7] - M[4] * M[6];
    A[7] = M[1] * M[6] - M[0] * M[7];
    A[8] = M[0] * M[4] - M[1] * M[3];

    double fDet = M[0] * A[0] + M[1] * A[3] + M[2] * A[6];

    if (fDet >= -ZEROF && fDet <= ZEROF)
        return false;

    double fInvDet = 1.0 / fDet;
    for (register int i = 0; i < 9; ++i)
    {
        M[i] = A[i] * fInvDet;
    }

    return true;
}

// Gram-Schmidt orthogonalization.
void niMatrix33::Orthogonalization()
{
    double fInv = 1.0 / sqrt(M[0] * M[0] + M[3] * M[3] + M[6] * M[6]);
    M[0] *= fInv;
    M[3] *= fInv;
    M[6] *= fInv;

    double fDot0 = M[0] * M[1] + M[3] * M[4] + M[6] * M[7];
    M[1] -= fDot0 * M[0];
    M[4] -= fDot0 * M[3];
    M[7] -= fDot0 * M[6];

    fInv = 1.0 / sqrt(M[1] * M[1] + M[4] * M[4] +  M[7] * M[7]);
    M[1] *= fInv;
    M[4] *= fInv;
    M[7] *= fInv;

    double fDot1 = M[1] * M[2] + M[4] * M[5] + M[7] * M[8];
    fDot0 = M[0] * M[2] + M[3] * M[5] + M[6] * M[8];
    M[2] -= fDot0 * M[0] + fDot1 * M[1];
    M[5] -= fDot0 * M[3] + fDot1 * M[4];
    M[8] -= fDot0 * M[6] + fDot1 * M[7];

    fInv = 1.0 / sqrt( M[2] * M[2] + M[5] * M[5] + M[8] * M[8] );
    M[2] *= fInv;
    M[5] *= fInv;
    M[8] *= fInv;
}

void niMatrix33::FromAxisAngle(const niVector3d& axis, double angle)
{
    double cosA = cos(angle);
    if ( cosA < 1-ZEROD )
    {
        niMatrix33 P( 0.0, -axis.Z, axis.Y, axis.Z, 0.0, -axis.X, -axis.Y, axis.X, 0.0 );
        niMatrix33 PP = P * P * ( 1.0 - cosA );
        P *= sin(angle);
        niMatrix33 &rot = *this;
        rot = 1.0;
        rot += P;
        rot += PP;
    }
    else
    {
        *this = 1.0;
    }
}

//-----------------------------------------------------------------------
void niMatrix33::ToAxisAngle(niVector3d& axis, double& angle) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    double fTrace = M[0] + M[4] + M[8];
    double cosA = 0.5 * ( fTrace - 1.0 );
    angle = acos( cosA );  // in [0,PI]

    if ( angle > 0.0  )
    {
        if ( angle < _PI_  )
        {
            axis.X = M[7] - M[5];
            axis.Y = M[2] - M[6];
            axis.Z = M[3] - M[1];
            axis.Normalize();
        }

        else
        {
            // angle is PI
            double halfInv;
            if ( M[0] >= M[4] )
            {
                // r00 >= r11
                if ( M[0] >= M[8] )
                {
                    // r00 is maximum diagonal term
                    axis.X = 0.5 * sqrt( M[0] - M[4] - M[8] + 1.0 );
                    halfInv = 0.5 / axis.X;
                    axis.Y = halfInv * M[1];
                    axis.Z = halfInv * M[2];
                }

                else
                {
                    // r22 is maximum diagonal term
                    axis.Z = 0.5 * sqrt( M[8] - M[0] - M[4] + 1.0 );
                    halfInv = 0.5 / axis.Z;
                    axis.X = halfInv * M[2];
                    axis.Y = halfInv * M[5];
                }
            }

            else
            {
                // r11 > r00

                if ( M[4] >= M[8] )
                {
                    // r11 is maximum diagonal term
                    axis.Y = 0.5 * sqrt( M[4] - M[0] - M[8] + 1.0 );
                    halfInv = 0.5 / axis.Y;
                    axis.X = halfInv * M[1];
                    axis.Z = halfInv * M[5];
                }

                else
                {
                    // r22 is maximum diagonal term
                    axis.Z = 0.5 * sqrt( M[8] - M[0] - M[4] + 1.0 );
                    halfInv = 0.5 / axis.Z;
                    axis.X = halfInv * M[2];
                    axis.Y = halfInv * M[5];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        axis.X = 1.0;
        axis.Y = 0.0;
        axis.Z = 0.0;
    }
}



//-----------------------------------------------------------------------
bool niMatrix33::ToEulerAnglesXYZ(niRotate3d &rot) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    rot.P = asin( M[2] ) ;

    if ( rot.P <  _PI_/2.0  )
    {
        if ( rot.P >  -_PI_/2.0  )
        {
            rot.Y = atan2( -M[5], M[8] );
            rot.R = atan2( -M[1], M[0] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( M[3], M[4] );
            rot.R =  0.0 ;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( M[3], M[4] );
        rot.R = 0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix33::ToEulerAnglesXZY(niRotate3d &rot) const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    rot.P = asin( -M[1] );

    if ( rot.P <  _PI_/2.0 )
    {
        if ( rot.P >  -_PI_/2.0 )
        {
            rot.Y = atan2( M[7], M[4] );
            rot.R = atan2( M[2], M[0] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( -M[6], M[8] );
            rot.R = 0.0 ;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( -M[6], M[8] );
        rot.R =  0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix33::ToEulerAnglesYXZ(niRotate3d &rot) const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    rot.P = asin( -M[5] );

    if ( rot.P <  _PI_/2.0 )
    {
        if ( rot.P > -_PI_/2.0 )
        {
            rot.Y = atan2( M[2], M[8] );
            rot.R = atan2( M[3], M[4] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( -M[1], M[0] );
            rot.R =  0.0 ;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( -M[1], M[0] );
        rot.R =  0.0 ;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix33::ToEulerAnglesYZX(niRotate3d &rot) const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    rot.P = asin( M[3] );

    if ( rot.P <  _PI_/2.0  )
    {
        if ( rot.P >  -_PI_/2.0 )
        {
            rot.Y = atan2( -M[6], M[0] );
            rot.R = atan2( -M[5], M[4] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( M[7], M[8] );
            rot.R =  0.0;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( M[7], M[8] );
        rot.R =  0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix33::ToEulerAnglesZXY(niRotate3d &rot) const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    rot.P = asin( M[7] );

    if ( rot.P <  _PI_/2.0 )
    {
        if ( rot.P >  -_PI_/2.0 )
        {
            rot.Y = atan2( -M[1], M[4] );
            rot.R = atan2( -M[6], M[8] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( M[2], M[0] );
            rot.R = 0.0;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( M[2], M[0] );
        rot.R = 0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix33::ToEulerAnglesZYX (niRotate3d &rot) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    rot.P = asin( -M[6] );

    if ( rot.P < _PI_/2.0 )
    {
        if ( rot.P > -_PI_/2.0 )
        {
            rot.Y = atan2( M[3], M[0] );
            rot.R = atan2( M[7], M[8] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( -M[1], M[2] );
            rot.R = 0.0;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( -M[1], M[2] );
        rot.R =  0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}


//-----------------------------------------------------------------------
void niMatrix33::FromEulerAnglesXYZ(const niRotate3d &rot)
{
    double cosA, sinA;

    cosA = cos( rot.Y );
    sinA = sin( rot.Y );
    niMatrix33 kXMat;
    if (fabs(sinA) < ZEROD)
    {
        kXMat = 1.0;
    }
    else
    {
        kXMat.SetAll( 1.0, 0.0, 0.0, 0.0, cosA, -sinA, 0.0, sinA, cosA );
    }
    

    cosA = cos( rot.P );
    sinA = sin( rot.P );
    niMatrix33 kYMat;
    if (fabs(sinA) < ZEROD)
    {
        kYMat = 1.0;
    }
    else
    {
        kYMat.SetAll( cosA, 0.0, sinA, 0.0, 1.0, 0.0, -sinA, 0.0, cosA );
    }

    cosA = cos( rot.R );
    sinA = sin( rot.R );
    niMatrix33 kZMat;
    if (fabs(sinA) < ZEROD)
    {
        kZMat = 1.0;
    }
    else
    {
        kZMat.SetAll( cosA, -sinA, 0.0, sinA, cosA, 0.0, 0.0, 0.0, 1.0 );
    }

    *this = kXMat * ( kYMat * kZMat );
}

//-----------------------------------------------------------------------
void niMatrix33::FromEulerAnglesXZY(const niRotate3d &rot)
{
    double cosA, sinA;

    cosA = cos( rot.Y );
    sinA = sin( rot.Y );
    niMatrix33 kXMat;
    if (fabs(sinA) < ZEROD)
    {
        kXMat = 1.0;
    }
    else
    {
        kXMat.SetAll( 1.0, 0.0, 0.0, 0.0, cosA, -sinA, 0.0, sinA, cosA );
    }

    cosA = cos( rot.P );
    sinA = sin( rot.P );
    niMatrix33 kZMat;
    if (fabs(sinA) < ZEROD)
    {
        kZMat = 1.0;
    }
    else
    {
        kZMat.SetAll( cosA, -sinA, 0.0, sinA, cosA, 0.0, 0.0, 0.0, 1.0 );
    }

    cosA = cos( rot.R );
    sinA = sin( rot.R );
    niMatrix33 kYMat;
    if (fabs(sinA) < ZEROD)
    {
        kYMat = 1.0;
    }
    else
    {
        kYMat.SetAll( cosA, 0.0, sinA, 0.0, 1.0, 0.0, -sinA, 0.0, cosA );
    }

    *this  = kXMat * ( kZMat * kYMat );
}

//-----------------------------------------------------------------------
void niMatrix33::FromEulerAnglesYXZ(const niRotate3d &rot)
{
    double cosA, sinA;

    cosA = cos( rot.Y );
    sinA = sin( rot.Y );
    niMatrix33 kYMat;
    if (fabs(sinA) < ZEROD)
    {
        kYMat = 1.0;
    }
    else
    {
        kYMat.SetAll( cosA, 0.0, sinA, 0.0, 1.0, 0.0, -sinA, 0.0, cosA );
    }

    cosA = cos( rot.P );
    sinA = sin( rot.P );
    niMatrix33 kXMat;
    if (fabs(sinA) < ZEROD)
    {
        kXMat = 1.0;
    }
    else
    {
        kXMat.SetAll( 1.0, 0.0, 0.0, 0.0, cosA, -sinA, 0.0, sinA, cosA );
    }

    cosA = cos( rot.R );
    sinA = sin( rot.R );
    niMatrix33 kZMat;
    if (fabs(sinA) < ZEROD)
    {
        kZMat = 1.0;
    }
    else
    {
        kZMat.SetAll( cosA, -sinA, 0.0, sinA, cosA, 0.0, 0.0, 0.0, 1.0 );
    }


    *this  = kYMat * ( kXMat * kZMat );
}

//-----------------------------------------------------------------------
void niMatrix33::FromEulerAnglesYZX(const niRotate3d &rot)
{
    double cosA, sinA;

    cosA = cos( rot.Y );
    sinA = sin( rot.Y );
    niMatrix33 kYMat;
    if (fabs(sinA) < ZEROD)
    {
        kYMat = 1.0;
    }
    else
    {
        kYMat.SetAll( cosA, 0.0, sinA, 0.0, 1.0, 0.0, -sinA, 0.0, cosA );
    }

    cosA = cos( rot.P );
    sinA = sin( rot.P );
    niMatrix33 kZMat;
    if (fabs(sinA) < ZEROD)
    {
        kZMat = 1.0;
    }
    else
    {
        kZMat.SetAll( cosA, -sinA, 0.0, sinA, cosA, 0.0, 0.0, 0.0, 1.0 );
    }

    cosA = cos( rot.R );
    sinA = sin( rot.R );
    niMatrix33 kXMat;
    if (fabs(sinA) < ZEROD)
    {
        kXMat = 1.0;
    }
    else
    {
        kXMat.SetAll( 1.0, 0.0, 0.0, 0.0, cosA, -sinA, 0.0, sinA, cosA );
    }

    *this  = kYMat * ( kZMat * kXMat );
}

//-----------------------------------------------------------------------
void niMatrix33::FromEulerAnglesZXY(const niRotate3d &rot)
{
    double cosA, sinA;

    cosA = cos( rot.Y );
    sinA = sin( rot.Y );
    niMatrix33 kZMat;
    if (fabs(sinA) < ZEROD)
    {
        kZMat = 1.0;
    }
    else
    {
        kZMat.SetAll( cosA, -sinA, 0.0, sinA, cosA, 0.0, 0.0, 0.0, 1.0 );
    }


    cosA = cos( rot.P );
    sinA = sin( rot.P );
    niMatrix33 kXMat;
    if (fabs(sinA) < ZEROD)
    {
        kXMat = 1.0;
    }
    else
    {
        kXMat.SetAll( 1.0, 0.0, 0.0, 0.0, cosA, -sinA, 0.0, sinA, cosA );
    }

    cosA = cos( rot.R );
    sinA = sin( rot.R );
    niMatrix33 kYMat;
    if (fabs(sinA) < ZEROD)
    {
        kYMat = 1.0;
    }
    else
    {
        kYMat.SetAll( cosA, 0.0, sinA, 0.0, 1.0, 0.0, -sinA, 0.0, cosA );
    }


    *this  = kZMat * ( kXMat * kYMat );
}

//-----------------------------------------------------------------------
void niMatrix33::FromEulerAnglesZYX(const niRotate3d &rot)
{
    double cosA, sinA;

    cosA = cos( rot.Y );
    sinA = sin( rot.Y );
    niMatrix33 kZMat;
    if (fabs(sinA) < ZEROD)
    {
        kZMat = 1.0;
    }
    else
    {
        kZMat.SetAll( cosA, -sinA, 0.0, sinA, cosA, 0.0, 0.0, 0.0, 1.0 );
    }

    cosA = cos( rot.P );
    sinA = sin( rot.P );
    niMatrix33 kYMat;
    if (fabs(sinA) < ZEROD)
    {
        kYMat = 1.0;
    }
    else
    {
        kYMat.SetAll( cosA, 0.0, sinA, 0.0, 1.0, 0.0, -sinA, 0.0, cosA );
    }

    cosA = cos( rot.R );
    sinA = sin( rot.R );
    niMatrix33 kXMat;
    if (fabs(sinA) < ZEROD)
    {
        kXMat = 1.0;
    }
    else
    {
        kXMat.SetAll( 1.0, 0.0, 0.0, 0.0, cosA, -sinA, 0.0, sinA, cosA );
    }

    *this  = kZMat * ( kYMat * kXMat );
}