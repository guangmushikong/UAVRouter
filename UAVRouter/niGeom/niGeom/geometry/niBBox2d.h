//! \file
// \brief
// The bounding box 2d class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niBBox2d_H
#define niBBox2d_H

#include <niGeom/geometry/niGeom2dTypes.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief The bounding box 2d class.
         *
         */
        class niBBox2d
        {
        public:
            //-----------------------------------------------------------------------------
            // FUNCTION Construction
            //-----------------------------------------------------------------------------
            /**
            * Construction
            * 
            */
            niBBox2d            ()
            {
                P1.X = P1.Y = 1e300;
                P2.X = P2.Y = -1e300;
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
            niBBox2d            (const niBBox2d &other)
            {
                P1 = other.P1;
                P2 = other.P2;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Area
            //-----------------------------------------------------------------------------
            /**
            * Calc Area of bbox
            * return        area
            * 
            */
            inline double       Area                    () const
            {
                return Xlength() * Ylength();
            }
            
            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append a point into bbox
            *
            * @param        p:              point
            * 
            */
            inline void         Append                  (const niPoint2d &p)
            {
                Append(p.X, p.Y);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append a point into bbox
            *
            * @param        p:              point
            * 
            */
            inline void         Append                  (const niPoint2d *p)
            {
                Append(p->X, p->Y);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Append
            //-----------------------------------------------------------------------------
            /**
            * Append a point into bbox
            *
            * @param        x:              x of point
            * @param        y:              y of point
            * 
            */
            inline void         Append                  (double x, double y)
            {
                P1.X = x < P1.X ? x : P1.X;
                P1.Y = y < P1.Y ? y : P1.Y;
                P2.X = x > P2.X ? x : P2.X;
                P2.Y = y > P2.Y ? y : P2.Y;
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
            inline void         Append                  (const niBBox2d &other)
            {
                P1.X = other.P1.X < P1.X ?  other.P1.X : P1.X;
                P1.Y = other.P1.Y < P1.Y ?  other.P1.Y : P1.Y;

                P2.X = other.P2.X > P2.X ?  other.P2.X : P2.X;
                P2.Y = other.P2.Y > P2.Y ?  other.P2.Y : P2.Y;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Extent
            //-----------------------------------------------------------------------------
            /**
            * Extent bbox
            *
            * @param        eX:             length of x need to extent
            * @param        eY:             length of y need to extent
            * 
            */
            inline void         Extent                  (double eX, double eY)
            {
                P1.X -= eX;     P2.X += eX;
                P1.Y -= eY;     P2.Y += eY;
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
                P1.X = P1.Y = 1e300;
                P2.X = P2.Y = -1e300;
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
                P1.X -= x;
                P1.Y -= y;
                P2.X += x;
                P2.Y += y;
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
                return (P1.X <= P2.X) && (P1.Y <= P2.Y);
            }

            //-----------------------------------------------------------------------------
            // FUNCTION MaxAxis
            //-----------------------------------------------------------------------------
            /**
            * max axis of bbox
            *
            * return        0:              the max axis is x
            *               1:              the max axis is y
            * 
            */
            inline int          MaxAxis                 () const
            {
                return P2.X - P1.X > P2.Y - P1.Y ? 0 : 1;
            }

        public:
            niPoint2d           P1;
            niPoint2d           P2;
        };
    }
}

#endif
