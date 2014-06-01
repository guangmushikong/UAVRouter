#include <niGeom/math/niMatrix34.h>

void niMatrix34::FromAxisAngle(const niVector3d& axis, double angle)
{
    if ( fabs(sin(angle)) >= ZEROD )
    {
        niMatrix33 rot;
        rot.FromAxisAngle(axis, angle);
        M[ 0] = rot.M[0];    M[ 1] = rot.M[1];    M[ 2] = rot.M[2];    M[ 3] = 0.0;
        M[ 4] = rot.M[3];    M[ 5] = rot.M[4];    M[ 6] = rot.M[5];    M[ 7] = 0.0;
        M[ 8] = rot.M[6];    M[ 9] = rot.M[7];    M[10] = rot.M[8];    M[11] = 0.0;
        m_identity = false;
    }
    else
    {
        *this = 1.0;
    }
}

//-----------------------------------------------------------------------
void niMatrix34::ToAxisAngle(niVector3d& axis, double& angle) const
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

    double fTrace = M[0] + M[5] + M[10];
    double cosA = 0.5 * ( fTrace - 1.0 );
    angle = acos( cosA );  // in [0,PI]

    if ( angle > 0.0  )
    {
        if ( angle < _PI_  )
        {
            axis.X = M[9] - M[6];
            axis.Y = M[2] - M[8];
            axis.Z = M[4] - M[1];
            axis.Normalize();
        }

        else
        {
            // angle is PI
            double halfInv;
            if ( M[0] >= M[5] )
            {
                // r00 >= r11
                if ( M[0] >= M[10] )
                {
                    // r00 is maximum diagonal term
                    axis.X = 0.5 * sqrt( M[0] - M[5] - M[10] + 1.0 );
                    halfInv = 0.5 / axis.X;
                    axis.Y = halfInv * M[1];
                    axis.Z = halfInv * M[2];
                }

                else
                {
                    // r22 is maximum diagonal term
                    axis.Z = 0.5 * sqrt( M[10] - M[0] - M[5] + 1.0 );
                    halfInv = 0.5 / axis.Z;
                    axis.X = halfInv * M[2];
                    axis.Y = halfInv * M[6];
                }
            }

            else
            {
                // r11 > r00

                if ( M[5] >= M[10] )
                {
                    // r11 is maximum diagonal term
                    axis.Y = 0.5 * sqrt( M[5] - M[0] - M[10] + 1.0 );
                    halfInv = 0.5 / axis.Y;
                    axis.X = halfInv * M[1];
                    axis.Z = halfInv * M[6];
                }

                else
                {
                    // r22 is maximum diagonal term
                    axis.Z = 0.5 * sqrt( M[10] - M[0] - M[5] + 1.0 );
                    halfInv = 0.5 / axis.Z;
                    axis.X = halfInv * M[2];
                    axis.Y = halfInv * M[6];
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
bool niMatrix34::ToEulerAnglesXYZ(niRotate3d &rot) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    rot.P = asin( M[2] ) ;

    if ( rot.P <  _PI_/2.0  )
    {
        if ( rot.P >  -_PI_/2.0  )
        {
            rot.Y = atan2( -M[6], M[10] );
            rot.R = atan2( -M[1], M[0] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( M[4], M[5] );
            rot.R =  0.0 ;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( M[4], M[5] );
        rot.R = 0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix34::ToEulerAnglesXZY(niRotate3d &rot) const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    rot.P = asin( -M[1] );

    if ( rot.P <  _PI_/2.0 )
    {
        if ( rot.P >  -_PI_/2.0 )
        {
            rot.Y = atan2( M[9], M[5] );
            rot.R = atan2( M[2], M[0] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( -M[8], M[10] );
            rot.R = 0.0 ;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( -M[8], M[10] );
        rot.R =  0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix34::ToEulerAnglesYXZ(niRotate3d &rot) const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    rot.P = asin( -M[6] );

    if ( rot.P <  _PI_/2.0 )
    {
        if ( rot.P > -_PI_/2.0 )
        {
            rot.Y = atan2( M[2], M[10] );
            rot.R = atan2( M[4], M[5] );
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
bool niMatrix34::ToEulerAnglesYZX(niRotate3d &rot) const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    rot.P = asin( M[4] );

    if ( rot.P <  _PI_/2.0  )
    {
        if ( rot.P >  -_PI_/2.0 )
        {
            rot.Y = atan2( -M[8], M[0] );
            rot.R = atan2( -M[6], M[5] );
            return true;
        }

        else
        {
            // WARNING.  Not a unique solution.
            double fRmY_radian = atan2( M[9], M[10] );
            rot.R =  0.0;  // any angle works
            rot.Y = rot.R - fRmY_radian;
            return false;
        }
    }

    else
    {
        // WARNING.  Not a unique solution.
        double fRpY_radian = atan2( M[9], M[10] );
        rot.R =  0.0;  // any angle works
        rot.Y = fRpY_radian - rot.R;
        return false;
    }
}

//-----------------------------------------------------------------------
bool niMatrix34::ToEulerAnglesZXY(niRotate3d &rot) const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    rot.P = asin( M[9] );

    if ( rot.P <  _PI_/2.0 )
    {
        if ( rot.P >  -_PI_/2.0 )
        {
            rot.Y = atan2( -M[1], M[5] );
            rot.R = atan2( -M[8], M[10] );
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
bool niMatrix34::ToEulerAnglesZYX (niRotate3d &rot) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    rot.P = asin( -M[8] );

    if ( rot.P < _PI_/2.0 )
    {
        if ( rot.P > -_PI_/2.0 )
        {
            rot.Y = atan2( M[4], M[0] );
            rot.R = atan2( M[9], M[10] );
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
void niMatrix34::FromEulerAnglesXYZ(const niRotate3d &rot)
{
    niMatrix33 tm;
    tm.FromEulerAnglesXYZ(rot);
    M[ 0] = tm.M[0];    M[ 1] = tm.M[1];    M[ 2] = tm.M[2];    M[ 3] = 0.0;
    M[ 4] = tm.M[3];    M[ 5] = tm.M[4];    M[ 6] = tm.M[5];    M[ 7] = 0.0;
    M[ 8] = tm.M[6];    M[ 9] = tm.M[7];    M[10] = tm.M[8];    M[11] = 0.0;
    m_identity = tm.m_identity;
}

//-----------------------------------------------------------------------
void niMatrix34::FromEulerAnglesXZY(const niRotate3d &rot)
{
    niMatrix33 tm;
    tm.FromEulerAnglesXZY(rot);
    M[ 0] = tm.M[0];    M[ 1] = tm.M[1];    M[ 2] = tm.M[2];    M[ 3] = 0.0;
    M[ 4] = tm.M[3];    M[ 5] = tm.M[4];    M[ 6] = tm.M[5];    M[ 7] = 0.0;
    M[ 8] = tm.M[6];    M[ 9] = tm.M[7];    M[10] = tm.M[8];    M[11] = 0.0;
    m_identity = tm.m_identity;
}

//-----------------------------------------------------------------------
void niMatrix34::FromEulerAnglesYXZ(const niRotate3d &rot)
{
    niMatrix33 tm;
    tm.FromEulerAnglesYXZ(rot);
    M[ 0] = tm.M[0];    M[ 1] = tm.M[1];    M[ 2] = tm.M[2];    M[ 3] = 0.0;
    M[ 4] = tm.M[3];    M[ 5] = tm.M[4];    M[ 6] = tm.M[5];    M[ 7] = 0.0;
    M[ 8] = tm.M[6];    M[ 9] = tm.M[7];    M[10] = tm.M[8];    M[11] = 0.0;
    m_identity = tm.m_identity;
}

//-----------------------------------------------------------------------
void niMatrix34::FromEulerAnglesYZX(const niRotate3d &rot)
{
    niMatrix33 tm;
    tm.FromEulerAnglesYZX(rot);
    M[ 0] = tm.M[0];    M[ 1] = tm.M[1];    M[ 2] = tm.M[2];    M[ 3] = 0.0;
    M[ 4] = tm.M[3];    M[ 5] = tm.M[4];    M[ 6] = tm.M[5];    M[ 7] = 0.0;
    M[ 8] = tm.M[6];    M[ 9] = tm.M[7];    M[10] = tm.M[8];    M[11] = 0.0;
    m_identity = tm.m_identity;
}

//-----------------------------------------------------------------------
void niMatrix34::FromEulerAnglesZXY(const niRotate3d &rot)
{
    niMatrix33 tm;
    tm.FromEulerAnglesZXY(rot);

    M[ 0] = tm.M[0];    M[ 1] = tm.M[1];    M[ 2] = tm.M[2];    M[ 3] = 0.0;
    M[ 4] = tm.M[3];    M[ 5] = tm.M[4];    M[ 6] = tm.M[5];    M[ 7] = 0.0;
    M[ 8] = tm.M[6];    M[ 9] = tm.M[7];    M[10] = tm.M[8];    M[11] = 0.0;
    m_identity = tm.m_identity;
}

//-----------------------------------------------------------------------
void niMatrix34::FromEulerAnglesZYX(const niRotate3d &rot)
{
    niMatrix33 tm;
    tm.FromEulerAnglesZYX(rot);
    M[ 0] = tm.M[0];    M[ 1] = tm.M[1];    M[ 2] = tm.M[2];    M[ 3] = 0.0;
    M[ 4] = tm.M[3];    M[ 5] = tm.M[4];    M[ 6] = tm.M[5];    M[ 7] = 0.0;
    M[ 8] = tm.M[6];    M[ 9] = tm.M[7];    M[10] = tm.M[8];    M[11] = 0.0;
    m_identity = tm.m_identity;
}
