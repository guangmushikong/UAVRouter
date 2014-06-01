//! \file
// \brief
// The curve2d class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version
// Reference http://en.wikipedia.org/wiki/Catmull-Rom

#ifndef niCurve2d_H
#define niCurve2d_H

#include <niGeom/geometry/niGeom2dTypes.h>
#include <iostream>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief The curve2d class.
         *
         */
        class niCurve2d
        {
        public:
            /**
             * \brief control point
             *
             */
            struct _SControlPoint
            {
                niPoint2d           m_endPoint;
                niPoint2d           m_inTangent;
                niPoint2d           m_outTangent;
                int                 m_continuity;
            };
        public:
            niCurve2d               () {}

            static bool             CreateCurve             (
                                                            const niPoint2d &p1,
                                                            const niPoint2d &m1,
                                                            const niPoint2d &p2,
                                                            const niPoint2d &m2,
                                                            double lenStep,
                                                            niPoint2dArray &curves);


            static bool             CreateCurve             (
                                                            const niPoint2d &p1,
                                                            const niPoint2d &m1,
                                                            const niPoint2d &p2,
                                                            const niPoint2d &m2,
                                                            int num_span,
                                                            niPoint2dArray &curves);

            static bool             MakeCurve               (
                                                            const _SControlPoint &cv1,
                                                            const _SControlPoint &cv2,
                                                            double lenStep,
                                                            niPoint2dArray &curves);

            static bool             MakeCurve               (
                                                            const _SControlPoint &cv1,
                                                            const _SControlPoint &cv2,
                                                            int num_span,
                                                            niPoint2dArray &curves);

            static bool             MakeCurve               (
                                                            const niPoint2d &ep0,
                                                            const niPoint2d &ep1,
                                                            const niPoint2d &ep2,
                                                            const niPoint2d &ep3,
                                                            double lenStep,
                                                            double continuity,
                                                            niPoint2dArray &curves);

            static bool             MakeCurve               (
                                                            const niPoint2d &ep0,
                                                            const niPoint2d &ep1,
                                                            const niPoint2d &ep2,
                                                            const niPoint2d &ep3,
                                                            int num_span,
                                                            double continuity,
                                                            niPoint2dArray &curves);

            static bool             MakeCurves              (
                                                            niPoint2dArray &endPoints,
                                                            double lenStep,
                                                            niPoint2dArray &curves);

            //-----------------------------------------------------------------------------
            // FUNCTION Hermite
            //-----------------------------------------------------------------------------
            /**
            * Hermite
            * @param        p1:             end point 1
            * @param        m1:             start tangent
            * @param        p2:             end point 2
            * @param        m2:             end tangent
            * 
            */
            static inline niPoint2d Hermite                 (
                                                            const niPoint2d &p1,
                                                            const niPoint2d &m1,
                                                            const niPoint2d &p2,
                                                            const niPoint2d &m2,
                                                            double t)
            {
                double t2 = t * t;
                double t3 = t2 * t;

                double c1 = t3 * 2 - t2 * 3 + 1.0;
                double c2 = t3 - t2 * 2 + t;
                double c3 = 1.0 - c1;
                double c4 = t3 - t2;

                return p1 * c1 + m1 * c2 + p2 * c3 + m2 * c4;
            }

        protected:
            static bool             ResampleT               (
                                                            const niDoubleArray &sampleLengths,
                                                            double step,
                                                            double curLen,
                                                            int initPoisiton,
                                                            double &t);

        protected:
            static int              sSampleRate;
        };
    }
}


#endif
