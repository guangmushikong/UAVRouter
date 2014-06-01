//! \file
// \brief
// The curve3d class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version
// Reference http://en.wikipedia.org/wiki/Catmull-Rom

#ifndef niCurve3d_H
#define niCurve3d_H

#include <iostream>
#include <niGeom/geometry/niGeom3dTypes.h>

namespace ni
{
    namespace geometry
    {
        /**
         * \brief The curve3d class.
         *
         */
        class niCurve3d
        {
        public:
            /**
             * \brief control point
             *
             */
            struct _SControlPoint
            {
                niVert3d            m_endPoint;
                niVert3d            m_inTangent;
                niVert3d            m_outTangent;
                int                 m_continuity;
            };
        public:
            niCurve3d               () {}

            static bool             CreateCurve             (
                                                            const niVert3d &p1,
                                                            const niVert3d &m1,
                                                            const niVert3d &p2,
                                                            const niVert3d &m2,
                                                            double lenStep,
                                                            niVert3dArray &curves);


            static bool             CreateCurve             (
                                                            const niVert3d &p1,
                                                            const niVert3d &m1,
                                                            const niVert3d &p2,
                                                            const niVert3d &m2,
                                                            int num_span,
                                                            niVert3dArray &curves);

            static bool             MakeCurve               (
                                                            const _SControlPoint &cv1,
                                                            const _SControlPoint &cv2,
                                                            double lenStep,
                                                            niVert3dArray &curves);

            static bool             MakeCurve               (
                                                            const _SControlPoint &cv1,
                                                            const _SControlPoint &cv2,
                                                            int num_span,
                                                            niVert3dArray &curves);

            static bool             MakeCurve               (
                                                            const niVert3d &ep0,
                                                            const niVert3d &ep1,
                                                            const niVert3d &ep2,
                                                            const niVert3d &ep3,
                                                            double lenStep,
                                                            double continuity,
                                                            niVert3dArray &curves);

            static bool             MakeCurve               (
                                                            const niVert3d &ep0,
                                                            const niVert3d &ep1,
                                                            const niVert3d &ep2,
                                                            const niVert3d &ep3,
                                                            int num_span,
                                                            double continuity,
                                                            niVert3dArray &curves);

            static bool             MakeCurves              (
                                                            niVert3dArray &endPoints,
                                                            double lenStep,
                                                            niVert3dArray &curves);

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
            static inline niVert3d  Hermite                 (
                                                            const niVert3d &p1,
                                                            const niVert3d &m1,
                                                            const niVert3d &p2,
                                                            const niVert3d &m2,
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
