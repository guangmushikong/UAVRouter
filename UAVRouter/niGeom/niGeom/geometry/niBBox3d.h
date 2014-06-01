//! \file
// \brief
// The bounding box 3d class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niBBox3d_H
#define niBBox3d_H

#include <niGeom/geometry/niGeom3dTypes.h>
#include <memory.h>
namespace ni
{
    namespace geometry
    {
        /**
         * \brief The bounding box 3d class.
         *
         */
        class niBBox3d
        {
        public:
            //-----------------------------------------------------------------------------
            // FUNCTION Construction
            //-----------------------------------------------------------------------------
            /**
            * Construction
            *
            */
            niBBox3d            ()
            {
                P1.X = P1.Y = P1.Z = 1e300;
                P2.X = P2.Y = P2.Z = -1e300;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Copy Construction
            //-----------------------------------------------------------------------------
            /**
            * Copy Construction
            *
            * @param        other:          other box
            * 
            */
            niBBox3d            (const niBBox3d &other)
            {
                P1 = other.P1;
                P2 = other.P2;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Volume
            //-----------------------------------------------------------------------------
            /**
            * Calc Volume of bbox
            * return        volume
            * 
            */
            inline double       Volume                  () const
            {
                return Xlength() * Ylength() * Zlength();
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append a vert into bbox
            *
            * @param        p:              point
            * 
            */
            inline void         Append                  (const niVert3d &p)
            {
                Append(p.X, p.Y, p.Z);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append a vert into bbox
            *
            * @param        p:              point
            * 
            */
            inline void         Append                  (const niVert3d *p)
            {
                Append(p->X, p->Y, p->Z);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append a vert into bbox
            *
            * @param        x:              x of vert
            * @param        y:              y of vert
            * @param        z:              z of vert
            * 
            */
            inline void         Append                  (double x, double y, double z)
            {
                P1.X = x < P1.X ? x : P1.X;
                P1.Y = y < P1.Y ? y : P1.Y;
                P1.Z = z < P1.Z ? z : P1.Z;
                P2.X = x > P2.X ? x : P2.X;
                P2.Y = y > P2.Y ? y : P2.Y;
                P2.Z = z > P2.Z ? z : P2.Z;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append another bbox into bbox
            *
            * @param        other:          other bbox
            * 
            */
            inline void         Append                  (const niBBox3d &other)
            {
                P1.X = other.P1.X < P1.X ?  other.P1.X : P1.X;
                P1.Y = other.P1.Y < P1.Y ?  other.P1.Y : P1.Y;
                P1.Z = other.P1.Z < P1.Z ?  other.P1.Z : P1.Z;

                P2.X = other.P2.X > P2.X ?  other.P2.X : P2.X;
                P2.Y = other.P2.Y > P2.Y ?  other.P2.Y : P2.Y;
                P2.Z = other.P2.Z > P2.Z ?  other.P2.Z : P2.Z;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Extent
            //-----------------------------------------------------------------------------
            /**
            * Extent bbox
            *
            * @param        eX:             length of x need to extent
            * @param        eY:             length of y need to extent
            * @param        ez:             length of z need to extent
            * 
            */
            inline void         Extent                  (double eX, double eY, double eZ)
            {
                P1.X -= eX;     P2.X += eX;
                P1.Y -= eY;     P2.Y += eY;
                P1.Z -= eZ;     P2.Z += eZ;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Reset
            //-----------------------------------------------------------------------------
            /**
            * Reset bbox
            *
            */
            inline void         Reset                   ()
            {
                P1.X = P1.Y = P1.Z = 1e300;
                P2.X = P2.Y = P2.Z = -1e300;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Scale
            //-----------------------------------------------------------------------------
            /**
            * Scale bbox
            *
            * @param        scale:          scale size
            * 
            */
            void                Scale                   (double scale)
            {
                double x = (P2.X - P1.X) * 0.5 * scale;
                double y = (P2.Y - P1.Y) * 0.5 * scale;
                double z = (P2.Z - P1.Z) * 0.5 * scale;
                P1.X -= x;
                P1.Y -= y;
                P1.Z -= z;
                P2.X += x;
                P2.Y += y;
                P2.Z += z;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Xlength
            //-----------------------------------------------------------------------------
            /**
            * length of x
            *
            * return        length
            * 
            */
            inline double       Xlength                 () const
            {
                return P1.X > P2.X ? 0.0 : P2.X - P1.X;
            }
            //-----------------------------------------------------------------------------
            // FUNCTION Ylength
            //-----------------------------------------------------------------------------
            /**
            * length of y
            *
            * return        length
            * 
            */
            inline double       Ylength                 () const
            {
                return P1.Y > P2.Y ? 0.0 : P2.Y - P1.Y;
            }
            //-----------------------------------------------------------------------------
            // FUNCTION Zlength
            //-----------------------------------------------------------------------------
            /**
            * length of z
            *
            * return        length
            * 
            */
            inline double       Zlength                 () const
            {
                return P1.Z > P2.Z ? 0.0 : P2.Z - P1.Z;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION FromBlob
            //-----------------------------------------------------------------------------
            /**
            * set bbox from blob
            *
            * @param        blob:           length of x need to extent
            * @param        blobSize:       length of y need to extent
            *
            * return        true:           success
            *               false:          failed
            * 
            */
            inline bool         FromBlob                (const unsigned char* blob, int blobSize)
            {
                if (0 == blob || blobSize != sizeof(niBBox3d))
                    return false;
                char * buffer = (char*)(&P1);
                memcpy( (void*)buffer, (const void*)blob, blobSize);
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION IsValid
            //-----------------------------------------------------------------------------
            /**
            * Check the bbox is valid
            *
            * return        true:               valid
            *               false:              invalid
            * 
            */
            inline bool         IsValid                 () const
            {
                return (P1.X <= P2.X) && (P1.Y <= P2.Y) && (P1.Z <= P2.Z);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION MaxAxis
            //-----------------------------------------------------------------------------
            /**
            * max axis of bbox
            *
            * return        0:              the max axis is x
            *               1:              the max axis is y
            *               2:              the max axis is z
            * 
            */
            inline int          MaxAxis                 () const
            {
                double max = P2.X - P1.X;
                double ly = P2.Y - P1.Y;
                double lz = P2.Z - P1.Z;
                int maxAxis = 0;

                if (max < ly)
                {
                    max = ly;
                    maxAxis = 1;
                }

                if (max < lz)
                {
                    max = lz;
                    maxAxis = 2;
                }
                return maxAxis;
            }

        public:
            niVert3d           P1;
            niVert3d           P2;
        };
    }
}


#endif