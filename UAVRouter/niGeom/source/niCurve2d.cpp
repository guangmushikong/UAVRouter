//! \file
// \brief
// The curve2d class.
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version
// Reference http://en.wikipedia.org/wiki/Catmull-Rom

#include <niGeom/geometry/niCurve2d.h>

namespace ni
{
    namespace geometry
    {
        int niCurve2d::sSampleRate = 5;

        //-----------------------------------------------------------------------------
        // FUNCTION CreateCurve
        //-----------------------------------------------------------------------------
        /**
        * CreateCurve between two end points
        * @param        p1:             end point 1
        * @param        m1:             start tangent
        * @param        p2:             end point 2
        * @param        m2:             end tangent
        * @param        lenStep:        step of length
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::CreateCurve(
            const niPoint2d &p1,
            const niPoint2d &m1,
            const niPoint2d &p2,
            const niPoint2d &m2,
            double lenStep,
            niPoint2dArray &curves)
        {
            double estimateLength = p1.DistanceTo(p2) * (niCurve2d::sSampleRate + 1);
            int estimateSpan = (int)floor(estimateLength / lenStep + 0.5);

            double length = 0.0;
            double t;
            double step = 1.0 / (estimateSpan);
            niPoint2d last = p1;
            niPoint2d curr;
            for (t = step; t < 1.0 + ZEROF; t += step)
            {
                curr = niCurve2d::Hermite(p1, m1, p2, m2, t);
                length += last.DistanceTo(curr);
                last = curr;
            }

            int num_span = (int)floor(length / lenStep);
            double s1 = fabs( lenStep - length / num_span );
            double s2 = fabs( lenStep - length / (num_span+1) );
            num_span = s1 <= s2 ? num_span : num_span+1;

            return CreateCurve(p1, m1, p2, m2, num_span, curves);

        }

        //-----------------------------------------------------------------------------
        // FUNCTION CreateCurve
        //-----------------------------------------------------------------------------
        /**
        * CreateCurve between two end points
        * @param        p1:             end point 1
        * @param        m1:             start tangent
        * @param        p2:             end point 2
        * @param        m2:             end tangent
        * @param        num_span:       number of points
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::CreateCurve(
            const niPoint2d &p1,
            const niPoint2d &m1,
            const niPoint2d &p2,
            const niPoint2d &m2,
            int num_span,
            niPoint2dArray &curves)
        {
            niPoint2dArray          samplePoints;
            niDoubleArray           sampleLengths;

            int numSamples = num_span * niCurve2d::sSampleRate + 1;

            samplePoints.resize (numSamples);
            sampleLengths.resize(numSamples);

            samplePoints[0]         = p1;
            sampleLengths[0]        = 0.0;

            double t;
            double length = 0.0;
            double step = 1.0 / (numSamples - 1);
            int it = 1;
            for (t = step; t < 1.0 + ZEROF; t += step, ++it)
            {
                samplePoints[it] = niCurve2d::Hermite(p1, m1, p2, m2, t);
                length += samplePoints[it].DistanceTo( samplePoints[it-1] );
                sampleLengths[it] = length;
            }
            samplePoints[numSamples-1] = p2;

            double avgLength = length / (numSamples-1);
            double curLen;

            curves.resize(num_span + 1);
            curves[0]           = p1;
            curves[num_span]    = p2;
            for (int i = 1; i <= num_span; ++i)
            {
                int initPosition = i * niCurve2d::sSampleRate;
                curLen = avgLength * initPosition;
                ResampleT(sampleLengths, step, curLen, initPosition, t);
                curves[i] = niCurve2d::Hermite(p1, m1, p2, m2, t);
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeCurve
        //-----------------------------------------------------------------------------
        /**
        * MakeCurve between two control points
        * @param        cv1:            control point 1
        * @param        cv2:            control point 2
        * @param        lenStep:        step of length
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::MakeCurve(
            const _SControlPoint &cv1,
            const _SControlPoint &cv2,
            double lenStep,
            niPoint2dArray &curves)
        {
            double c1, c2;
            c1 = (1 + cv1.m_continuity) * 0.5;
            c2 = 1.0 - c1;

            const niPoint2d &p1 = cv1.m_endPoint;
            //const niPoint2d &m1 = cv1.m_inTangent;
            niPoint2d m1 = cv1.m_inTangent * c1 - cv1.m_outTangent * c2;

            c1 = (1 + cv2.m_continuity) * 0.5;
            c2 = 1.0 - c1;
            const niPoint2d &p2 = cv2.m_endPoint;
            //const niPoint2d &m2 = cv2.m_inTangent;
            niPoint2d m2 = cv2.m_inTangent * c2 - cv2.m_outTangent * c1;

            return CreateCurve(p1, m1, p2, m2, lenStep, curves);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeCurve
        //-----------------------------------------------------------------------------
        /**
        * MakeCurve between two control points
        * @param        cv1:            control point 1
        * @param        cv2:            control point 2
        * @param        num_span:       number of points
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::MakeCurve(
            const _SControlPoint &cv1,
            const _SControlPoint &cv2,
            int num_span,
            niPoint2dArray &curves)
        {
            double c1, c2;
            c1 = (1 + cv1.m_continuity) * 0.5;
            c2 = 1.0 - c1;

            const niPoint2d &p1 = cv1.m_endPoint;
            //const niPoint2d &m1 = cv1.m_inTangent;
            niPoint2d m1 = cv1.m_inTangent * c1 - cv1.m_outTangent * c2;

            c1 = (1 + cv2.m_continuity) * 0.5;
            c2 = 1.0 - c1;
            const niPoint2d &p2 = cv2.m_endPoint;
            //const niPoint2d &m2 = cv2.m_inTangent;
            niPoint2d m2 = cv2.m_inTangent * c2 - cv2.m_outTangent * c1;

            return CreateCurve(p1, m1, p2, m2, num_span, curves);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeCurve
        //-----------------------------------------------------------------------------
        /**
        * MakeCurve between two end points(ep1, ep2)
        * @param        ep0:            end point
        * @param        ep1:            end point
        * @param        ep2:            end point
        * @param        ep3:            end point
        * @param        lenStep:        step of length
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::MakeCurve(
            const niPoint2d &ep0,
            const niPoint2d &ep1,
            const niPoint2d &ep2,
            const niPoint2d &ep3,
            double lenStep,
            double continuity,
            niPoint2dArray &curves)
        {
            double c1 = (1 + continuity) * 0.5;
            double c2 = 1 - c1;
            niPoint2d m1 = (ep1 - ep0) * c1 + (ep2 - ep1) * c2;
            niPoint2d m2 = (ep2 - ep1) * c2 + (ep3 - ep2) * c1;

            return CreateCurve(ep1, m1, ep2, m2, lenStep, curves);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeCurve
        //-----------------------------------------------------------------------------
        /**
        * MakeCurve from endpoints
        * @param        endpoints:      end points
        * @param        lenStep:        step of length
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::MakeCurves(
            niPoint2dArray &endPoints,
            double lenStep,
            niPoint2dArray &curves)
        {
            int numSegments = int(endPoints.size()) - 1;
            if (numSegments < 2)
                return false;

            niPoint2dArray curve;

            MakeCurve(endPoints[0], endPoints[0], endPoints[1], endPoints[2], lenStep, 0.0, curves);
            for (int i = 1; i < numSegments-1; ++i)
            {
                MakeCurve(endPoints[i-1], endPoints[i], endPoints[i+1], endPoints[i+2], lenStep, 0.0, curve);
                niPoint2dArray::iterator iter = curve.begin();
                iter++;
                for (; iter != curve.end(); iter++)
                {
                    curves.push_back( *iter );
                }
            }
            MakeCurve(endPoints[numSegments-2], endPoints[numSegments-1], endPoints[numSegments], endPoints[numSegments], lenStep, 0.0, curve);
            niPoint2dArray::iterator iter = curve.begin();
            iter++;
            for (; iter != curve.end(); iter++)
            {
                curves.push_back( *iter );
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakeCurve
        //-----------------------------------------------------------------------------
        /**
        * MakeCurve between two end points(ep1, ep2)
        * @param        ep0:            end point
        * @param        ep1:            end point
        * @param        ep2:            end point
        * @param        ep3:            end point
        * @param        num_span:       number of points
        * @param        curves:         points of curves
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::MakeCurve(
            const niPoint2d &ep0,
            const niPoint2d &ep1,
            const niPoint2d &ep2,
            const niPoint2d &ep3,
            int num_span,
            double continuity,
            niPoint2dArray &curves)
        {
            double c1 = (1 + continuity) * 0.5;
            double c2 = 1 - c1;
            niPoint2d m1 = (ep1 - ep0) * c1 + (ep2 - ep1) * c2;
            niPoint2d m2 = (ep2 - ep1) * c2 + (ep3 - ep2) * c1;

            return CreateCurve(ep1, m1, ep2, m2, num_span, curves);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION ResampleT
        //-----------------------------------------------------------------------------
        /**
        * Resample the interval value t.
        * @param        sampleLengths:  input lengths of spans.
        * @param        step:           length step
        * @param        curLen:         length of Curve
        * @param        initPoisiton:   current position need resample
        * @param        t:              the result of t
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niCurve2d::ResampleT(
            const niDoubleArray &sampleLengths,
            double step,
            double curLen,
            int initPoisiton,
            double &t)
        {
            int _l = 0;
            int _m = initPoisiton;
            int _r = int(sampleLengths.size() - 1);
            double _val;

            while (_l <= _r)
            {
                _val = sampleLengths[_m];
                if (curLen < _val + ZEROF)
                {
                    if (curLen >= sampleLengths[_m-1])
                    {
                        t = (curLen - sampleLengths[_m-1]) / (sampleLengths[_m] - sampleLengths[_m-1]) * step + _m * step;
                        return true;
                    }
                    else
                    {
                        _r = _m - 1;
                        _m = (_l + _r) / 2;
                    }
                }
                else
                {
                    if (curLen <= sampleLengths[_m+1])
                    {
                        t = (curLen - sampleLengths[_m]) / (sampleLengths[_m+1] - sampleLengths[_m]) * step + _m * step;
                        return true;
                    }
                    else
                    {
                        _l = _m + 1;
                        _m = (_l + _r) / 2;
                    }
                }
            }

            return false;
        }

    }
}

