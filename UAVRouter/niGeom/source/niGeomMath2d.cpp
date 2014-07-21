//! \file
// \brief
// Geometry math 2d class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#include <niGeom/geometry/niGeomMath2d.h>
#include <niGeom/geometry/niBBox2d.h>
#include <niGeom/geometry/niGeom2dTypes.h>
#include <niGeom/geometry/niLine2d.h>
#include <niGeom/geometry/niPolygon2d.h>
#include <niGeom/geometry/niTriangle2d.h>
#include <niGeom/geometry/tree/niBvhTree.h>

#include <stack>
#include <iostream>

namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION Angle
        //-----------------------------------------------------------------------------
        /**
        * calc the angle of a corner
        *
        * @param       p0:          the corner point
        * @param       p1:          endpoint of the first border
        * @param       p2:          endpoint of the second border
        * @return      the angle of the corner
        *             p2
        *            /
        *           /
        *         p0-----p1
        * 
        */
        double niGeomMath2d::Angle(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &p2)
        {
            niPoint2d d1 = p1 - p0;
            niPoint2d d2 = p2 - p0;
            double cross = d1.Cross(d2);
            double dot    = d1.Dot(d2);

            double norm_d12 = sqrt( (d1.X*d1.X + d1.Y*d1.Y) * (d2.X*d2.X + d2.Y*d2.Y) );
            if (0 == norm_d12)
                return 0;

            cross /= norm_d12;

            if (cross >= 1.0)
            {
                return _PI_ * 0.5;
            }
            else if (cross <= -1.0)
            {
                return _PI_ * 1.5;
            }
            double angle = asin(cross);

            if (dot < 0.0)
                angle = _PI_ - angle;
            else if (cross < 0.0)
                angle = _PI_ * 2.0 + angle;

            return angle;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Angle
        //-----------------------------------------------------------------------------
        /**
        * calc the angle of a corner
        *
        * @param       d1:          
        * @param       d2:          
        * @return      the angle of the corner
        *             d2
        *            /
        *           /
        *          O-----d1
        * 
        */
        double niGeomMath2d::Angle(
            const niPoint2d &d1,
            const niPoint2d &d2)
        {
            double cross    = d1.Cross(d2);
            double dot      = d1.Dot(d2);

            double norm_d12 = sqrt( (d1.X*d1.X + d1.Y*d1.Y) * (d2.X*d2.X + d2.Y*d2.Y) );
            if (0 == norm_d12)
                return 0;

            cross /= norm_d12;

            if (cross >= 1.0)
            {
                return _PI_ * 0.5;
            }
            else if (cross <= -1.0)
            {
                return _PI_ * 1.5;
            }
            double angle = asin(cross);

            if (dot < 0.0)
                angle = _PI_ - angle;
            else if (cross < 0.0)
                angle = _PI_ * 2.0 + angle;

            return angle;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _AvoidAbnormalData
        //-----------------------------------------------------------------------------
        /**
        * If the input data has N significant digits, it's beter to scale and round 
        *   the data to avoid decimal.
        * For example: 127123456.7 - 127123456.5 = 0.200000xxxxxxxxxx, is not 0.2
        * But 1271234567 - 1271234565 = 2, is accurate
        *
        * To reduce the number significant figures, all points should minus the center
        *   of bounding box. For example: 1271234567 * 37123456, significant digits 
        *   overflow, the result is not accurate.
        * If minus the center of bounding box, it will reduce 2-10 significant digits.
        * For example: 1271234567 - 1271230000 = 4567; 37123456 - 37120000 = 3456;
        *  4567 * 3456 is accurate
        *
        *
        * @param       num_sign_digital:    num number significant figures 
        * @param       bbox:                bbox 
        * @param       points:              store result
        */
        void niGeomMath2d::AvoidAbnormalData(
            const int num_sign_digital,
            const niBBox2d &bbox,
            niPoint2dArray &points)
        {
            int num_points = int(points.size());

            double maxBorder = fabs(bbox.P1.X);
            maxBorder = maxBorder > fabs(bbox.P1.Y) ? maxBorder : fabs(bbox.P1.Y);
            maxBorder = maxBorder > fabs(bbox.P2.X) ? maxBorder : fabs(bbox.P2.X);
            maxBorder = maxBorder > fabs(bbox.P2.Y) ? maxBorder : fabs(bbox.P2.Y);

            niPoint2d center = (bbox.P1 + bbox.P2) * 0.5;
            int exponent = num_sign_digital - (int)log10(maxBorder);
            double _SCALE_ = pow(10.0, exponent);

            center.X = floor(center.X * _SCALE_);
            center.Y = floor(center.Y * _SCALE_);

            for (int i = 0; i <num_points; ++i)
            {
                niPoint2d &p = points[i];
                p *= _SCALE_;
                p.X = floor(p.X + 0.5);
                p.Y = floor(p.Y + 0.5);
                p -= center;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _AvoidAbnormalData
        //-----------------------------------------------------------------------------
        /**
        * If the input data has N significant digits, it's beter to scale and round 
        *   the data to avoid decimal.
        * For example: 127123456.7 - 127123456.5 = 0.200000xxxxxxxxxx, is not 0.2
        * But 1271234567 - 1271234565 = 2, is accurate
        *
        * To reduce the number significant figures, all points should minus the center
        *   of bounding box. For example: 1271234567 * 37123456, significant digits 
        *   overflow, the result is not accurate.
        * If minus the center of bounding box, it will reduce 2-10 significant digits.
        * For example: 1271234567 - 1271230000 = 4567; 37123456 - 37120000 = 3456;
        *  4567 * 3456 is accurate
        *
        *
        * @param       _SCALE_:     scale
        * @param       center:      center
        * @param       points:      store result
        */
        void niGeomMath2d::AvoidAbnormalData(
            const double _SCALE_,
            niPoint2d center,
            niPoint2dArray &points)
        {
            int num_points = int(points.size());

            center.X = floor(center.X * _SCALE_);
            center.Y = floor(center.Y * _SCALE_);

            for (int i = 0; i <num_points; ++i)
            {
                niPoint2d &p = points[i];
                p *= _SCALE_;
                p.X = floor(p.X + 0.5);
                p.Y = floor(p.Y + 0.5);
                p -= center;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Area
        //-----------------------------------------------------------------------------
        /**
        * Calc area of point ring
        *
        * @param        points:     inpout point ring
        * return        area
        */
        double niGeomMath2d::Area(const niPoint2dArray &points)
        {
            size_t num_pnts = points.size();
            if (num_pnts < 3)
                return -1e300;

            double x0 = points[0].X;
            double y0 = points[0].Y;

            double x1, y1, x2, y2;
            x1 = points[1].X - x0;
            y1 = points[1].Y - y0;

            double area = 0.0;
            for (size_t i = 2; i < num_pnts; ++i)
            {
                x2 = points[i].X - x0;
                y2 = points[i].Y - y0;
                area += (x1 * y2 - x2 * y1);
                x1 = x2;
                y1 = y2;
            }
            area *= 0.5;
            return area;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Center
        //-----------------------------------------------------------------------------
        /**
        * Calc area of point ring
        *
        * @param        points:     inpout points
        * @param        center:     center
        * return        true:       success
        *               false:      failed
        */
        bool niGeomMath2d::Center(
            const niPoint2dArray &points,
            niPoint2d &center)
        {
            size_t numPoints = points.size();
            if (numPoints < 1)
                return false;

            double X = 0.0, Y = 0.0;

            for (size_t i = 0; i < numPoints; ++i)
            {
                X += points[i].X;
                Y += points[i].Y;
            }

            X /= double(numPoints);
            Y /= double(numPoints);

            center.Init(X, Y);
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION CalcConvexHull
        //-----------------------------------------------------------------------------
        /**
        * Calc  ConvexHull of points
        *
        * @param        points:         inpout points
        * @param        convexHullIds:  reference ids
        * return        true:           success
        *               false:          failed
        */
        bool niGeomMath2d::CalcConvexHull(
            const niPoint2dArray &points,
            niIntArray &convexHullIds)
        {
            size_t numPoints    = points.size();
            if (numPoints < 3)
                return false;

            int firstPointId = -1;
            double minY         = 1e300;

            for (size_t i = 0; i < numPoints; ++i)
            {
                if (minY > points[i].Y)
                {
                    minY = points[i].Y;
                    firstPointId = i;
                }
            }

            std::stack<int> iStack;
            EOrientation orientation;
            int vIdx1 = firstPointId;
            int vIdx2 = (vIdx1 + 1) % numPoints;
            int vIdx3 = -1;

            iStack.push( vIdx1 );
            iStack.push( vIdx2 );

            while (vIdx3 != firstPointId)
            {
                vIdx2 = iStack.top();
                vIdx3 = (vIdx2 + 1) % numPoints;

                orientation = niGeomMath2d::Orientation(
                    points[vIdx1], points[vIdx2], points[vIdx3]);
                if (eCounterClockwise == orientation)
                {
                    vIdx1 = vIdx2;
                }
                else
                {
                    iStack.pop();
                    while (iStack.size() > 1)
                    {
                        vIdx2 = iStack.top();
                        iStack.pop();
                        vIdx1 = iStack.top();
                        orientation = niGeomMath2d::Orientation(
                            points[vIdx1], points[vIdx2], points[vIdx3]);
                        if (eCounterClockwise == orientation)
                        {
                            iStack.push(vIdx2);
                            break;
                        }
                    }
                }
                if (vIdx3 != firstPointId)
                    iStack.push(vIdx3);
            }

            size_t numConvexHull = iStack.size();
            convexHullIds.resize(numConvexHull);
            int iter = numConvexHull-1;
            while (!iStack.empty())
            {
                convexHullIds[ iter-- ] = iStack.top();
                iStack.pop();
            }

			/*
            return iter == 0;
			changed by wangmiao 2014/7/20 ==>
				*/
			return iter == -1;

        }

        //-----------------------------------------------------------------------------
        // FUNCTION CalcPointsOBB
        //-----------------------------------------------------------------------------
        /**
        * Calc approximate oriented bbox of points
        *
        * @param        points:         inpout points
        * @param        center:         center of points
        * @param        angle:          rotate angle
        * return        true:           success
        *               false:          failed
        */
        bool niGeomMath2d::CalcPointsOBB(
            const niPoint2dArray &points,
            niPoint2d &center,
            double &angle)
        {
            size_t numPoints = points.size();
            if (numPoints < 1)
            {
                angle = 0.0;
                return false;
            }

            center = points[0];

            for (size_t i = 1; i < numPoints; ++i)
            {
                center += points[i];
            }

            center /= double(numPoints);

            double varX = 0.0, varY = 0.0, covXY = 0.0;

            for (size_t i = 0; i < numPoints; ++i)
            {
                niPoint2d pnt = points[i] - center;

                varX    += pnt.X * pnt.X;
                varY    += pnt.Y * pnt.Y;
                covXY   += pnt.X * pnt.Y;
            }

            if (varX < ZEROF || varY < ZEROF)
            {
                angle = 0.0;
                return true;
            }

            if (abs(varY - varX) < 0.0001)
                angle = 45.0 * _PI_ / 180.0;
            else
            {
                angle = atan(2.0 * covXY / (varY - varX)) / 2.0;
                if (covXY > 0.0)
                {
                    if (angle < 0.0)
                        angle = angle + _PI_ / 2.0;
                }
                if (covXY < 0.0)
                {
                    if (angle < 0.0)
                        angle = angle + _PI_;
                    else
                        angle = angle + _PI_ / 2.0;

                }
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION CalcPointsOBB
        //-----------------------------------------------------------------------------
        /**
        * Calc approximate oriented bbox of points
        *
        * @param        points:         inpout points
        * @param        center:         center of points
        * @param        rot:            rotate matrix
        * return        true:           success
        *               false:          failed
        */
        bool niGeomMath2d::CalcPointsOBB(
            const niPoint2dArray &points,
            niPoint2d &center,
            niMatrix22 &rot)
        {
            double angle;
            bool bStat = CalcPointsOBB(points, center, angle);
            if (!bStat)
                return false;

            rot.FromEulerAngle(angle);
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION DiameterOfBBox2BBox
        //-----------------------------------------------------------------------------
        /**
        * Calc diameter of two bboxes
        *
        * @param        bbox1:          bbox1
        * @param        bbox2:          bbox2
        * return        diameter
        */
        double niGeomMath2d::DiameterOfBBox2BBox(
            const niBBox2d &bbox1,
            const niBBox2d &bbox2)
        {
            double X1 = bbox1.P1.X < bbox2.P1.X ? bbox1.P1.X : bbox2.P1.X;
            double Y1 = bbox1.P1.Y < bbox2.P1.Y ? bbox1.P1.Y : bbox2.P1.Y;
            double X2 = bbox1.P2.X > bbox2.P2.X ? bbox1.P2.X : bbox2.P2.X;
            double Y2 = bbox1.P2.Y > bbox2.P2.Y ? bbox1.P2.Y : bbox2.P2.Y;

            return Distance(X1, Y1, X2, Y2);
        }


        //-----------------------------------------------------------------------------
        // FUNCTION Distance
        //-----------------------------------------------------------------------------
        /**
        * Calc distance of two point
        *
        * @param        X1:             X of point 1
        * @param        Y1:             Y of point 1
        * @param        X2:             X of point 2
        * @param        Y2:             Y of point 2
        * return        distance
        */
        double niGeomMath2d::Distance(
            const double& X1,
            const double& Y1,
            const double& X2,
            const double& Y2)
        {
            return sqrt( (X1-X2) * (X1-X2) + (Y1-Y2) * (Y1-Y2) );
        }

        //-----------------------------------------------------------------------------
        // FUNCTION DistanceOfBBox2BBox
        //-----------------------------------------------------------------------------
        /**
        * Calc the distance from bbox to bbox
        *
        * @param      bbox1:        box1
        * @param      bbox2:        box2
        * @return      return the distance of box1 to box2
        * 
        */
        double niGeomMath2d::DistanceOfBBox2BBox(
            const niBBox2d &bbox1,
            const niBBox2d &bbox2)
        {
            if (bbox1.P2.X < bbox2.P1.X)
            {
                if (bbox1.P2.Y < bbox2.P1.Y)
                    return bbox1.P2.DistanceTo(bbox2.P1);
                if (bbox1.P1.Y > bbox2.P2.Y)
                    return Distance(bbox1.P2.X, bbox1.P1.Y, bbox2.P1.X, bbox2.P2.Y);
                return bbox2.P1.X - bbox1.P2.X;
            }
            if (bbox2.P2.X < bbox1.P1.X)
            {
                if (bbox1.P2.Y < bbox2.P1.Y)
                    return Distance(bbox1.P1.X, bbox1.P2.Y, bbox2.P2.X, bbox2.P1.Y);
                if (bbox1.P1.Y > bbox2.P2.Y)
                    return bbox1.P1.DistanceTo(bbox2.P2);
                return bbox1.P1.X - bbox2.P2.X;
            }
            if (bbox1.P2.Y < bbox2.P1.Y)
                return bbox2.P1.Y - bbox1.P2.Y;
            if (bbox1.P1.Y > bbox2.P2.Y)
                return bbox1.P1.Y - bbox2.P2.Y;

            return 0.0;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION DistanceOfPoint2BBox
        //-----------------------------------------------------------------------------
        /**
        * Calc the distance from a point to bbox
        *
        * @param       p:           point
        * @param       bbox:        box
        * @return      return the distance of point to bbox
        * 
        */
        double niGeomMath2d::DistanceOfPoint2BBox(
            const niPoint2d &p,
            const niBBox2d &bbox)
        {
            if (p.X < bbox.P1.X)
            {
                if (p.Y < bbox.P1.Y)
                    return p.DistanceTo(bbox.P1);
                if (p.Y > bbox.P2.Y)
                    return p.DistanceTo(bbox.P1.X, bbox.P2.Y);
                return bbox.P1.X - p.X;
            }
            if (p.X > bbox.P2.X)
            {
                if (p.Y < bbox.P1.Y)
                    return p.DistanceTo(bbox.P2.X, bbox.P1.Y);
                if (p.Y > bbox.P2.Y)
                    return p.DistanceTo(bbox.P2);
                return p.X - bbox.P2.X;
            }
            if (p.Y < bbox.P1.Y)
                return bbox.P1.Y - p.Y;
            if (p.Y > bbox.P2.Y)
                return p.Y - bbox.P2.Y;
            return 0.0;
        }



        //-----------------------------------------------------------------------------
        // FUNCTION HeigthOfParallelLines
        //-----------------------------------------------------------------------------
        /**
        *
        * @param       P1(X1, Y1), P2(X2, Y2):      two points of line 1
        * @param       P3(X3, Y3), P4(X4, Y4):      two points of line 1
        * @return      the height of two parallel lines
        *   Example 1: Not Overlap, Height is 1e300
        *             P1----------------P2
        *             |                  |
        *   P3-----P4 |          OR      | P3---------P4
        *             |                  |
        * 
        *   Example 2: Overlap, Height is the distance of two lines
        *             P1----------------P2
        *             |                  |
        *        P3---P4 OR P3---P4 OR P3---P4 
        *             |                  |
        * 
        *   Example 3: Overlap, Height is the distance of two lines
        *             P1----------------P2
        *             |                  |
        *        P3---+------------------+---P4 
        *             |                  |
        */
        double niGeomMath2d::HeigthOfParallelLines(
            const double &X1, const double &Y1,
            const double &X2, const double &Y2,
            const double &X3, const double &Y3,
            const double &X4, const double &Y4)
        {
            double aX = X2 - X1;
            double aY = Y2 - Y1;

            double bX, bY, cX, cY;
            bX = X3 - X2;       bY = Y3 - Y2;
            cX = X3 - X1;       cY = Y3 - Y1;
            double dot13 = aX * cX + aY * cY;
            double dot23 = bX * cX + bY * cY;
            if (dot13 * dot23 <= 0)         //Example 2
            {
                return fabs(aX * cY - aY * cX) / sqrt( aX * aX + aY * aY );
            }

            bX = X4 - X2;       bY = Y4 - Y2;
            cX = X4 - X1;       cY = Y4 - Y1;
            double dot14 = aX * cX + aY * cY;
            double dot24 = bX * cX + bY * cY;
            if (dot14 * dot24 <= 0)         //Example 2
            {
                return fabs(aX * cY - aY * cX) / sqrt( aX * aX + aY * aY );
            }

            if (dot13 * dot14 < 0)          //Example 3
            {
                return fabs(aX * cY - aY * cX) / sqrt( aX * aX + aY * aY );
            }

            return 1e300;                   //Example 1
        }


        //-----------------------------------------------------------------------------
        // FUNCTION HeigthOfParallelLines
        //-----------------------------------------------------------------------------
        /**
        *
        * @param       P1(X1, Y1), P2(X2, Y2):      two points of line 1
        * @param       P3(X3, Y3), P4(X4, Y4):      two points of line 1
        * @return      the height of two parallel lines
        *   Example 1: Not Overlap, Height is 1e300
        *             P1----------------P2
        *             |                  |
        *   P3-----P4 |          OR      | P3---------P4
        *             |                  |
        * 
        *   Example 2: Overlap, Height is the distance of two lines
        *             P1----------------P2
        *             |                  |
        *        P3---P4 OR P3---P4 OR P3---P4 
        *             |                  |
        * 
        *   Example 3: Overlap, Height is the distance of two lines
        *             P1----------------P2
        *             |                  |
        *        P3---+------------------+---P4 
        *             |                  |
        */
        double niGeomMath2d::HeigthOfParallelLines(
            const niPoint2d &P1,
            const niPoint2d &P2,
            const niPoint2d &P3,
            const niPoint2d &P4)
        {
            return HeigthOfParallelLines(
                P1.X, P1.Y,
                P2.X, P2.Y,
                P3.X, P3.Y,
                P4.X, P4.Y);
        }


        //-----------------------------------------------------------------------------
        // FUNCTION HeightOfPoint2Line
        //-----------------------------------------------------------------------------
        /**
        * Is the two bbox overlap each other
        *
        * @param       P:           Point
        * @param       P1:          end point of line
        * @param       P2:          end point of line
        * @return      height
        * 
        * @param       P1(X1, Y1), P2(X2, Y2):      two points of line 1
        * @param       P3(X3, Y3), P4(X4, Y4):      two points of line 1
        * @return      the height of two parallel lines
        *   Example 1: Not Overlap, Height is 1e300
        *             P1----------------P2
        *             |                  |
        *          P  |         OR       | P
        *             |                  |
        * 
        *   Example 2: Overlap, Height is the distance of two lines
        *             P1----------------P2
        *             |                  |
        *             P    OR   P   OR   P
        *             |                  |
        * 
        */
        double niGeomMath2d::HeightOfPoint2Line(
            const niPoint2d &P,
            const niPoint2d &P1,
            const niPoint2d &P2)
        {
            double aX = P2.X - P1.X;
            double aY = P2.Y - P1.Y;

            double bX, bY, cX, cY;
            bX = P.X - P2.X;       bY = P.Y - P2.Y;
            cX = P.X - P1.X;       cY = P.Y - P1.Y;
            double dot12 = aX * bX + aY * bY;
            double dot13 = aX * cX + aY * cY;
            if (dot12 * dot13 <= 0.0)
            {
                return fabs(aX * cY - aY * cX) / sqrt( aX * aX + aY * aY );
            }
            else
            {
                return 1e300;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsBoxOverlapBox
        //-----------------------------------------------------------------------------
        /**
        * Is the two bbox overlap each other
        *
        * @param       bbox1:       box
        * @param       bbox2:       box
        * @return      if overlap to each other, return true, or return false
        * 
        */
        bool niGeomMath2d::IsBoxOverlapBox(
            const niBBox2d &bbox1,
            const niBBox2d &bbox2)
        {
            if (bbox1.P2.X < bbox2.P1.X)
                return false;
            if (bbox1.P2.Y < bbox2.P1.Y)
                return false;
            if (bbox1.P1.X > bbox2.P2.X)
                return false;
            if (bbox1.P1.Y > bbox2.P2.Y)
                return false;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsLineOverlapBox
        //-----------------------------------------------------------------------------
        /**
        * Is line segment overlap bbox.
        *
        * @param       p0:          point 0
        * @param       p1:          point 1
        * @param       bbox:        box
        * @return      if overlap to each other, return true, or return false
        * 
        */
        bool niGeomMath2d::IsLineOverlapBox(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niBBox2d &bbox)
        {
            //calculate the half-length-vectors of the AABB
            double hX = bbox.P2.X - bbox.P1.X;
            double tX = p0.X + p1.X - bbox.P2.X - bbox.P1.X;
            //calculate line-direction
            double dX = p1.X - p0.X;
            if ( fabs(tX) > (hX + fabs(dX)) )
                return false;

            //calculate the half-length-vectors of the AABB
            double hY = bbox.P2.Y - bbox.P1.Y;
            double tY = p0.Y + p1.Y - bbox.P2.Y - bbox.P1.Y;
            //calculate line-direction
            double dY = p1.Y - p0.Y;
            if ( fabs(tY) > (hY + fabs(dY)) )
                return false;

            if( fabs(tY*dX-tX*dY) > (fabs(hX*dY) + fabs(hY*dX)) )
                return false;
            //no separating axis found, objects overlap
            return true;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION IsLineISectBox
        //-----------------------------------------------------------------------------
        /**
        * Is a line with two endpoint(segment) intersect a bbox
        *
        * @param       p0:          endpoint of line
        * @param       p1:          endpoint of line
        * @param       bbox:        box
        * @return      if intersect, return true, or return false
        */
        bool niGeomMath2d::IsLineISectBox(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niBBox2d &bbox)
        {
            double X, Y;
            double dx = p1.X - p0.X;
            double dy = p1.Y - p0.Y;
            if (!niMath::_is_zero_(dx))
            {
                X = bbox.P1.X;
                if (niMath::_in_range_(X, p0.X, p1.X))
                {
                    Y = dy / dx * (X - p0.X) + p0.Y;
                    if (niMath::_in_range_(Y, bbox.P1.Y, bbox.P2.Y))
                        return true;
                }
                X = bbox.P2.X;
                if (niMath::_in_range_(X, p0.X, p1.X))
                {
                    Y = dy / dx * (X - p0.X) + p0.Y;
                    if (niMath::_in_range_(Y, bbox.P1.Y, bbox.P2.Y))
                        return true;
                }
            }
            if (!niMath::_is_zero_(dy))
            {
                Y = bbox.P1.Y;
                if (niMath::_in_range_(Y, p0.Y, p1.Y))
                {
                    X = dx / dy * (Y - p0.Y) + p0.X;
                    if (niMath::_in_range_(X, bbox.P1.X, bbox.P2.X))
                        return true;
                }
                Y = bbox.P2.Y;
                if (niMath::_in_range_(Y, p0.Y, p1.Y))
                {
                    X = dx / dy * (Y - p0.Y) + p0.X;
                    if (niMath::_in_range_(X, bbox.P1.X, bbox.P2.X))
                        return true;
                }
            }

            return false;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION IsPointBetweenAngle
        //-----------------------------------------------------------------------------
        /**
        * Is a point between an angle
        *
        * @param       p0:          the corner point
        * @param       p1:          endpoint of the first border
        * @param       p2:          endpoint of the second border
        * @param       p:           point
        * @return      if in, return true, or return false
        *             p2
        *            /          p3
        *           /       p4
        *         p0-----p1
        *                   p5
        * p3, p4 is between the angle(p2 p0 p1), p5 is not between then angle
        */
        bool niGeomMath2d::IsPointBetweenAngle(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &p2,
            const niPoint2d &p)
        {
            niPoint2d d1 = p1 - p0;
            niPoint2d d2 = p2 - p0;
            niPoint2d d = p - p0;
            d1.Normalize();
            d2.Normalize();
            d.Normalize();
            double cross1 = d.Cross(d1);
            if (niMath::_is_zero_(cross1))
                return false;
            double cross2 = d.Cross(d2);
            if (niMath::_is_zero_(cross2))
                return false;
            return cross1 * cross2 < 0.0;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInBox
        //-----------------------------------------------------------------------------
        /**
        * Is a point in bbox
        *
        * @param       p:           the corner point
        * @param       bbox:        box
        * @return      if in, return true, or return false
        */

        bool niGeomMath2d::IsPointInBox(
            const niPoint2d &p,
            const niBBox2d &bbox)
        {
            if (p.X < bbox.P1.X)
                return false;
            if (p.Y < bbox.P1.Y)
                return false;
            if (p.X > bbox.P2.X)
                return false;
            if (p.Y > bbox.P2.Y)
                return false;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInPolygon
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a polygon
        *
        * @param       p:           point
        * @param       polygon:     polygon
        * @return      eOutGeometry:    out of polygon
        *              eOnBorder:       on the border of polygon
        *              eInGeometry:     in polygon
        *              eErrorGeometry:  polygon is error
        */
        EPointInGeometry niGeomMath2d::IsPointInPolygon(
            const niPoint2d &p,
            const niPolygon2d &polygon)
        {
            const niPoint2dArray &points = polygon.GetPoints();

            int num_lines = int(points.size() - 1);
            if (num_lines < 3)
                return eOutGeometry;

            EIntersectType isIn = eNotIntersect, isectType;

            for (int i = 0; i < num_lines; ++i)
            {
                const niPoint2d &pa = points[i];
                const niPoint2d &pb = points[i+1];
                if (IsPointOnLine(p, pa, pb))
                    return eOnBorder;


                isectType = IsXRayISectLine(p, pa, pb);
                if (eNotIntersect == isectType || eIntersectOverlap == isectType)
                    continue;
                isIn = (EIntersectType)(int(isIn) ^ int(isectType));
            }
            if (isIn == eIntersect)
                return eInGeometry;
            return eErrorGeometry;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInTriangle
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a triangle
        *
        * @param       p:           point
        * @param       tri:         triangle
        * @return      eOutGeometry:    out of the triangle
        *              eOnBorder:       on the border of the triangle
        *              eOnPoint:        on one point of the triangle
        *              eInGeometry:     in the triangle
        *              eErrorGeometry:  the triangle is error
        */
        EPointInGeometry niGeomMath2d::IsPointInTriangle(
            const niPoint2d &p,
            const niTriangle2d &tri)
        {
            return IsPointInTriangle(p, tri.P0, tri.P1, tri.P2);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInTriangle
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a triangle
        *
        * @param       p:           point
        * @param       a:           point of triangle
        * @param       b:           point of triangle
        * @param       c:           point of triangle
        * @return      eOutGeometry:    out of the triangle
        *              eOnBorder:       on the border of the triangle
        *              eOnPoint:        on one point of the triangle
        *              eInGeometry:     in the triangle
        *              eErrorGeometry:  the triangle is error
        */
        EPointInGeometry niGeomMath2d::IsPointInTriangle(
            const niPoint2d &p,
            const niPoint2d &a,
            const niPoint2d &b,
            const niPoint2d &c)
        {
            niPoint2d pa(a - p);
            niPoint2d pb(b - p);
            niPoint2d pc(c - p);
            double cross1 = pa.Cross(pb);
            double cross2 = pb.Cross(pc);
            if (cross1 * cross2 < -ZEROF)
                return eOutGeometry;
            double cross3 = pc.Cross(pa);
            if (cross1 * cross3 < -ZEROF)
                return eOutGeometry;
            if (cross3 * cross2 < -ZEROF)
                return eOutGeometry;

            double area = fabs( (b-a).Cross(c-a) );
            double a2 = fabs(cross1) + fabs(cross2) + fabs(cross3);
            if (0 != niMath::_compare_(area, a2))
                return eOutGeometry;

            int fInGeometry = 1;
            if (0 == cross1)
                fInGeometry = fInGeometry << 1;
            if (0 == cross2)
                fInGeometry = fInGeometry << 1;
            if (0 == cross3)
                fInGeometry = fInGeometry << 1;

            return EPointInGeometry(fInGeometry);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointInTriangle
        //-----------------------------------------------------------------------------
        /**
        * Is a point in a triangle
        *
        * @param       p:           point
        * @param       a:           point of triangle
        * @param       b:           point of triangle
        * @param       c:           point of triangle
        * @param       area:        area of triangle
        * @return      eOutGeometry:    out of the triangle
        *              eOnBorder:       on the border of the triangle
        *              eOnPoint:        on one point of the triangle
        *              eInGeometry:     in the triangle
        *              eErrorGeometry:  the triangle is error
        */
        EPointInGeometry niGeomMath2d::IsPointInTriangle(
            const niPoint2d &p,
            const niPoint2d &a,
            const niPoint2d &b,
            const niPoint2d &c,
            double area)
        {
            niPoint2d pa(a - p);
            niPoint2d pb(b - p);
            niPoint2d pc(c - p);
            double cross1 = pa.Cross(pb);
            double cross2 = pb.Cross(pc);
            double c12 = cross1 * cross2;
            if (c12 < -ZEROF)
                return eOutGeometry;
            double cross3 = pc.Cross(pa);
            double c23 = cross2 * cross3;
            if (c12 < -ZEROF)
                return eOutGeometry;
            double c31 = cross3 * cross1;
            if (c12 < -ZEROF)
                return eOutGeometry;
            if (c12 >= ZEROF && c23 >= ZEROF && c31 >= ZEROF)
                return eInGeometry;

            double a2 = fabs(cross1) + fabs(cross2) + fabs(cross3);
            if (0 != niMath::_compare_(area, a2))
                return eOutGeometry;

            int fInGeometry = 1;
            if (0 == cross1)
                fInGeometry = fInGeometry << 1;
            if (0 == cross2)
                fInGeometry = fInGeometry << 1;
            if (0 == cross3)
                fInGeometry = fInGeometry << 1;

            return EPointInGeometry(fInGeometry);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION IsPointOnLine
        //-----------------------------------------------------------------------------
        /**
        * Is a point on a line
        *
        * @param       p:           point
        * @param       p0:          endpoint of line
        * @param       p1:          endpoint of line
        * @return      true:        on line
        *              false:       not on line
        */
        bool niGeomMath2d::IsPointOnLine(
            const niPoint2d &p,
            const niPoint2d &p0,
            const niPoint2d &p1)
        {
            if (p == p0 || p == p1)
                return true;
            double l1 = p0.DistanceTo(p1);
            double l2 = p.DistanceTo(p0) + p.DistanceTo(p1);
            return 0 == niMath::_compare_(l1, l2);
        }


        //-----------------------------------------------------------------------------
        // FUNCTION IsXRayISectBox
        //-----------------------------------------------------------------------------
        /**
        * Is a Ray towards x-positive intersect with a bbox
        *
        * @param       p:           point
        * @param       bbox:        bbox
        * @return      true:        intersect
        *              false:       not intersect
        */
        bool niGeomMath2d::IsXRayISectBox(
            const niPoint2d &p,
            const niBBox2d &bbox)
        {
            if (1 == niMath::_compare_(p.X, bbox.P2.X))
                return false;
            if (-1 == niMath::_compare_(p.Y, bbox.P1.Y))
                return false;
            if (1 == niMath::_compare_(p.Y, bbox.P2.Y))
                return false;

            return true;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION IsYRayISectBox
        //-----------------------------------------------------------------------------
        /**
        * Is a Ray towards y-positive intersect with a bbox
        *
        * @param       p:           point
        * @param       bbox:        bbox
        * @return      true:        intersect
        *              false:       not intersect
        */
        bool niGeomMath2d::IsYRayISectBox(
            const niPoint2d &p,
            const niBBox2d &bbox)
        {
            if (1 == niMath::_compare_(p.Y, bbox.P2.Y))
                return false;
            if (-1 == niMath::_compare_(p.X, bbox.P1.X))
                return false;
            if (1 == niMath::_compare_(p.X, bbox.P2.X))
                return false;

            return true;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION IsXRayISectLine
        //-----------------------------------------------------------------------------
        /**
        * Is a Ray towards x-positive intersect with a line
        *
        * @param       p:           point
        * @param       p0:          endpoint of a line
        * @param       p1:          endpoint of a line
        * @return      true:        intersect
        *              false:       not intersect
        */
        EIntersectType niGeomMath2d::IsXRayISectLine(
            const niPoint2d &p,
            const niPoint2d &p0,
            const niPoint2d &p1)
        {
            int cmp1 = niMath::_compare_(p.Y, p0.Y);
            int cmp2 = niMath::_compare_(p.Y, p1.Y);
            if (-1 == cmp1 * cmp2)
            {
                double X = p0.X + (p.Y - p0.Y) / (p1.Y - p0.Y) * (p1.X - p0.X);
                if (1 == niMath::_compare_(p.X, X))
                    return eNotIntersect;
                return eIntersect;
            }
            else if (1 == cmp1 * cmp2)
            {
                return eNotIntersect;
            }
            else if (0 == cmp1 && 0 == cmp2)
            {
                if (1 == niMath::_compare_(p.X, p0.X) && 1 == niMath::_compare_(p.X, p1.X))
                    return eNotIntersect;
                return eIntersectOverlap;
            }
            else if (0 == cmp1)
            {
                if (1 == niMath::_compare_(p.X, p0.X))
                    return eNotIntersect;
                return (cmp2 == -1 ? eIntersectLeftHand : eIntersectRightHand);
            }
            else if (0 == cmp2)
            {
                if (1 == niMath::_compare_(p.X, p1.X))
                    return eNotIntersect;
                return (cmp1 == -1 ? eIntersectLeftHand : eIntersectRightHand);
            }
            else
            {
                return eNotIntersect;
            }
        }


        //-----------------------------------------------------------------------------
        // FUNCTION IsYRayISectLine
        //-----------------------------------------------------------------------------
        /**
        * Is a Ray towards y-positive intersect with a line
        *
        * @param       p:           point
        * @param       p0:          endpoint of a line
        * @param       p1:          endpoint of a line
        * @return      true:        intersect
        *              false:       not intersect
        */
        EIntersectType niGeomMath2d::IsYRayISectLine(
            const niPoint2d &p,
            const niPoint2d &p0,
            const niPoint2d &p1)
        {
            int cmp1 = niMath::_compare_(p.X, p0.X);
            int cmp2 = niMath::_compare_(p.X, p1.X);
            if (-1 == cmp1 * cmp2)
            {
                double Y = p0.Y + (p.X - p0.X) / (p1.X - p0.X) * (p1.Y - p0.Y);
                if (1 == niMath::_compare_(p.Y, Y))
                    return eNotIntersect;
                return eIntersect;
            }
            else if ( 1 == cmp1 * cmp2)
            {
                return eNotIntersect;
            }
            else if (0 == cmp1 && 0 == cmp2)
            {
                if (1 == niMath::_compare_(p.Y, p0.Y) && 1 == niMath::_compare_(p.Y, p1.Y))
                    return eNotIntersect;
                return eIntersectOverlap;
            }
            else if (0 == cmp1)
            {
                if (1 == niMath::_compare_(p.Y, p0.Y))
                    return eNotIntersect;
                return (cmp2 == -1 ? eIntersectRightHand : eIntersectLeftHand);
            }
            else if (0 == cmp2)
            {
                if (1 == niMath::_compare_(p.Y, p1.Y))
                    return eNotIntersect;
                return (cmp1 == -1 ? eIntersectRightHand : eIntersectLeftHand);
            }
            else
            {
                return eNotIntersect;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION LineOverlapLine
        //-----------------------------------------------------------------------------
        /**
        * Is a line intersect another line
        *
        * @param       p0:          endpoint of a line1
        * @param       p1:          endpoint of a line1
        * @param       q0:          endpoint of a line2
        * @param       q1:          endpoint of a line2
        * @return      true:        eIntersect
        *              false:       eNotIntersect / Parallel / eIntersectConnect
        */
        bool niGeomMath2d::LineOverlapLine(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &q0,
            const niPoint2d &q1)
        {
            double pX = p1.X - p0.X;
            double pY = p1.Y - p0.Y;
            double qX = q1.X - q0.X;
            double qY = q1.Y - q0.Y;

            double cross = pX * qY - pY * qX;

            double dX = q0.X - p0.X;
            double dY = q0.Y - p0.Y;

            if (niMath::_is_zero_(cross))
            {
                double dxq = dX * qY - dY * qX;
                if (niMath::_is_zero_( dxq ))
                {
                    int overlap;
                    if (pX != 0.0)
                        overlap = niMath::_is_overlap_(p0.X, p1.X, q0.X, q1.X);
                    else
                        overlap = niMath::_is_overlap_(p0.Y, p1.Y, q0.Y, q1.Y);
                    if (1 == overlap)
                    {
                        return true;        //return eIntersect;
                    }
                    else if (-1 == overlap) 
                    {
                        return false;       //return eNotIntersect;
                    }
                    else
                    {
                        return false;       //return eIntersectConnect;
                    }
                }
                else
                {
                    return false;           //Parallel
                }
            }

            double tp = (dX * qY - dY * qX) / cross;
            if (tp > 1.0 + ZEROD || tp < -ZEROD)
                return false;               //return eNotIntersect;

            double tq = (dX * pY - dY * pX) / cross;
            if (tq > 1.0 + ZEROD || tq < -ZEROD)
                return false;               //return eNotIntersect;

            int counter = 0;
            if (tp < ZEROD || tp > 1.0 - ZEROD)
            {
                ++counter;
            }
            if (tq < ZEROD || tq > 1.0 - ZEROD)
            {
                ++counter;
            }

            if (counter < 2)
            {
                return true;            //return eIntersect;
            }
            else
            {
                return false;           //return eIntersectConnect;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION LineOverlapLine
        //-----------------------------------------------------------------------------
        /**
        * Is a line intersect another line
        *
        * @param       p0:          endpoint of a line1
        * @param       p1:          endpoint of a line1
        * @param       q0:          endpoint of a line2
        * @param       q1:          endpoint of a line2
        * @param       minHeight:
        *                           If Parallel, return HeigthOfParallelLines
        *                           If intersect, minHeight is 0.0
        *                           If eIntersectConnect, minHeight is 1e300
        *                           Not intersect, the extention of q0q1 intersect p0p1, calc the height
        *
        * @return      true:        eIntersect
        *              false:       eNotIntersect / Parallel / eIntersectConnect
        */
        bool niGeomMath2d::LineOverlapLine(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &q0,
            const niPoint2d &q1,
            double &minHeight)
        {
            double pX = p1.X - p0.X;
            double pY = p1.Y - p0.Y;
            double qX = q1.X - q0.X;
            double qY = q1.Y - q0.Y;

            double cross = pX * qY - pY * qX;

            double dX = q0.X - p0.X;
            double dY = q0.Y - p0.Y;

            if (niMath::_is_zero_(cross))
            {
                double dxq = dX * qY - dY * qX;
                if (niMath::_is_zero_( dxq ))
                {
                    int overlap;
                    if (pX != 0.0)
                        overlap = niMath::_is_overlap_(p0.X, p1.X, q0.X, q1.X);
                    else
                        overlap = niMath::_is_overlap_(p0.Y, p1.Y, q0.Y, q1.Y);
                    if (1 == overlap)
                    {
                        minHeight = 0.0;
                        return true;        //return eIntersect;
                    }
                    else if (-1 == overlap) 
                    {
                        minHeight = 1e300;
                        return false;       //return eNotIntersect;
                    }
                    else
                    {
                        minHeight = 1e300;
                        return false;       //return eIntersectConnect;
                    }
                }
                else
                {
                    minHeight = HeigthOfParallelLines(p0, p1, q0, q1);
                    return false;           //Parallel
                }
            }

            double tp = (dX * qY - dY * qX) / cross;
            if (tp > 1.0 + ZEROD || tp < -ZEROD)
                return false;               //return eNotIntersect;

            double tq = (dX * pY - dY * pX) / cross;
            if (tq > 1.0 + ZEROD || tq < -ZEROD)
            {
                dX = qX * tq;
                dY = qY * tq;
                if (tq > 1.0)
                {
                    dX -= qX;
                    dY -= qY;
                }
                minHeight = fabs(dX * pY - dY * pX) / sqrt(pX * pX + pY * pY);

                return false;               //return eNotIntersect;
            }

            int counter = 0;
            if (tp < ZEROD || tp > 1.0 - ZEROD)
            {
                ++counter;
            }
            if (tq < ZEROD || tq > 1.0 - ZEROD)
            {
                ++counter;
            }

            if (counter < 2)
            {
                return true;            //return eIntersect;
            }
            else
            {
                return false;           //return eIntersectConnect;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION LineISectLine
        //-----------------------------------------------------------------------------
        /**
        * Is two line intersect each other
        *
        * @param       p:           point
        * @param       p0:          endpoint of the first line
        * @param       p1:          endpoint of the first line
        * @param       q0:          endpoint of the second line
        * @param       q1:          endpoint of the second line
        * @return      true:        intersect
        *              false:       not intersect
        */
        bool niGeomMath2d::LineISectLine(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &q0,
            const niPoint2d &q1,
            niPoint2d &isec)
        {

            double pX = p1.X - p0.X;
            double pY = p1.Y - p0.Y;
            double qX = q1.X - q0.X;
            double qY = q1.Y - q0.Y;

            double cross = pX * qY - pY * qX;

            if (niMath::_is_zero_(cross))
            {
                return false;
            }

            double dX = q0.X - p0.X;
            double dY = q0.Y - p0.Y;

            double t = (dX * qY - dY * qX) / cross;
            if (t > 1.0 + ZEROD || t < -ZEROD)
                return false;

            t = (dX * pY - dY * pX) / cross;
            if (t > 1.0 + ZEROD || t < -ZEROD)
                return false;

            isec.Init(p0.X + pX * t, p0.Y + pY * t);

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Orientation
        //-----------------------------------------------------------------------------
        /**
        * Calc Orientation of a triangle
        *
        * @param       p:           point
        * @param       p0:          point of the triangle
        * @param       p1:          point of the triangle
        * @param       p2:          point of the triangle
        * @return      eCounterClockwise / eClockwise
        */
        EOrientation niGeomMath2d::Orientation(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const niPoint2d &p2)
        {
            double area = (p1-p0).Cross(p2-p0);
            return area > 0.0 ? eCounterClockwise : eClockwise;
        }


        EOrientation niGeomMath2d::Orientation(
            const niPoint2dArray &points)
        {
            size_t num_pnts = points.size();
            if (num_pnts < 3)
                return eUndefined;

            double x0 = points[0].X;
            double y0 = points[0].Y;

            double x1, y1, x2, y2;
            x1 = points[1].X - x0;
            y1 = points[1].Y - y0;

            double area = 0.0;
            for (size_t i = 2; i < num_pnts; ++i)
            {
                x2 = points[i].X - x0;
                y2 = points[i].Y - y0;
                area += (x1 * y2 - x2 * y1);
                x1 = x2;
                y1 = y2;
            }
            return area > 0.0 ? eCounterClockwise : eClockwise;
        }
    }
}
