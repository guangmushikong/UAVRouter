#include "GomoGeometry2D.h"


#include <QDebug>

namespace Gomo {

namespace Geometry2D {


static double INVALID_COORDINATE_VALUE = -999999.00;

PolygonOrientation2D::PolygonOrientation2D( const Point2DArray pts)
{
    m_polygon_pts= pts;

    m_center.X=INVALID_COORDINATE_VALUE;
    m_center.Y=INVALID_COORDINATE_VALUE;

    ni::geometry::niPolygon2d polygon(m_polygon_pts);
    ni::geometry::niPolygon2dFn polyFunc(polygon);
    m_orignal_area = polyFunc.Area();

    m_extended_baseline = 200.0;// 200 meters as default

}


PolygonOrientation2D::~PolygonOrientation2D()
{

}


bool PolygonOrientation2D::GetOrientation_ConvexHull(OptimalOrientationInfo& info)
{
    info.__model= CONVEXHULL;
    info.__angle = 0;

    //first get convexhull
    Point2DArray polyon_convexhull;

    ni::niIntArray convexHullIds;
    if (ni::geometry::
            niGeomMath2d::CalcConvexHull(
                               m_polygon_centralized,
                               convexHullIds))
    {
        ni::niIntArray::iterator it_vec = convexHullIds.begin();

        for( ; it_vec!= convexHullIds.end(); it_vec++ )
        {
            polyon_convexhull.push_back(m_polygon_centralized[*it_vec]);
        }

    }
    else
    {
        return false;
    }


    //use this map to find the optimal roation angle that make the smallest mbr area after roation
    std::map<double /* mbr area after rotation*/, double /*angle*/> mbr_angle;

    int numPoints=polyon_convexhull.size();
    double angle_by_edge;

    if(numPoints > 2)
    {
        Point2DArray::iterator it_vertex = polyon_convexhull.begin();

        for( ; (it_vertex+1)!= polyon_convexhull.end(); it_vertex++ )
        {
            //calculate the angle of each edge one by one
            angle_by_edge = LineOrientation( *it_vertex,*(it_vertex+1));

            double rotated_mbr_area=-1;

            if( PolygonRoationExperiment(angle_by_edge,rotated_mbr_area))
            {
                mbr_angle[rotated_mbr_area] = angle_by_edge;
            }
            else
            {
                false;
            }

        }

    }
    else
    {
        return false;
    }


    // take the first element , ie the smallest mbr area after rotation
    info.__angle = mbr_angle.begin()->second;
    info.__rotated_mbr_area = mbr_angle.begin()->first;

    return true;
}

bool PolygonOrientation2D::GetOrientation_Skeleton(OptimalOrientationInfo& info)
{
    info.__model= SKELETON;


    info.__angle = 0;
    PolygonRoationExperiment(info.__angle,info.__rotated_mbr_area);

    return true;

}

bool PolygonOrientation2D::GetOrientation_LineApprox(OptimalOrientationInfo& info)
{
    double angle = 0.0;

    info.__model = LINE_APPROXIMATION;

    int numPoints = m_polygon_centralized.size();

    double varX = 0.0, varY = 0.0, covXY = 0.0;

    for (size_t i = 0; i < numPoints; ++i)
    {
        Point2D pnt = m_polygon_centralized[i];//cetralize
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

            angle -= _PI_ / 4.0;//added by wangmiao
        }

        if (covXY < 0.0)
        {
            if (angle < 0.0)
                angle = angle + _PI_;
            else
                angle = angle + _PI_ / 2.0;

            angle += _PI_ / 4.0 ;//added by wangmiao
        }
    }

    info.__angle = angle;
    PolygonRoationExperiment(info.__angle,info.__rotated_mbr_area);

    return true;


}

bool PolygonOrientation2D::PolygonRoationExperiment(const double & angle,
                              double& mbr_area_after_rotation)
{
    //reverse the angle, then apply the rotation
    double reverse_angle = angle * (-1);

    Point2DArray rotated_vetices;

    std::vector<Point2D>::iterator itVetex = m_polygon_centralized.begin();
    for( ; itVetex!=m_polygon_centralized.end();itVetex++)
    {
        Point2D newp= Rotate2D(*itVetex,reverse_angle);
        rotated_vetices.push_back(newp);
    }

    Point2D lt,rb;

    MBR2D(rotated_vetices,lt,rb);

    mbr_area_after_rotation = (fabs(lt.X-rb.X)+m_extended_baseline)*fabs(lt.Y-rb.Y);

    return true;

}

bool PolygonOrientation2D::Centralization()
{
    m_polygon_centralized.clear();

    int numPoints = m_polygon_pts.size();

    if(numPoints >=2 && m_center.X !=INVALID_COORDINATE_VALUE )
    {
        for (size_t i = 0; i < numPoints; ++i)
        {
            Point2D pnt = m_polygon_pts[i] - m_center;//cetralize
            m_polygon_centralized.push_back(pnt);
        }

    }
    else
    {
        return false;
    }

    return true;

}

Point2D PolygonOrientation2D::GetCenter()
{

    int numPoints = m_polygon_pts.size();

    if(numPoints >=2)
    {
        Point2D center= m_polygon_pts[0];

        for (size_t i = 1; i < numPoints; ++i)
        {
            center += m_polygon_pts[i];
        }

        center /= double(numPoints);

        m_center =center;
    }
    else
    {
        m_center.X=INVALID_COORDINATE_VALUE;
        m_center.Y=INVALID_COORDINATE_VALUE;
    }

    return m_center;

}

bool PolygonOrientation2D::GetOptimalOrientation(double & optimal_angle)
{
    ostringstream streamdebug;

    m_center = GetCenter();

    if(Centralization()==false)
    {
        return false;
    }

    streamdebug.str("");
    streamdebug<<"m_orignal_area="<<m_orignal_area<<std::endl;
    qDebug(streamdebug.str().c_str());

    OptimalOrientationInfo orien_info;
    if(GetOrientation_LineApprox(orien_info)==true)
    {
        double diff_area = fabs(m_orignal_area - orien_info.__rotated_mbr_area  );

        m_model_orienation.insert(std::make_pair(diff_area,orien_info));


        streamdebug.str("");
        streamdebug<<"diff_area="<<diff_area<<std::endl;
        orien_info.Output(streamdebug);
        qDebug(streamdebug.str().c_str());
    }

    if(GetOrientation_ConvexHull(orien_info)==true)
    {
        double diff_area = fabs(m_orignal_area - orien_info.__rotated_mbr_area  );

        m_model_orienation.insert(std::make_pair(diff_area,orien_info));

        streamdebug.str("");
        streamdebug<<"diff_area="<<diff_area<<std::endl;
        orien_info.Output(streamdebug);
        qDebug(streamdebug.str().c_str());
    }

    // std::map would use the least key as the begin.
    m_orientation_angle = m_model_orienation.begin()->second.__angle;


    // make sure the angle be smaller than 90 degree
    if( m_orientation_angle> _PI_ / 2.0 )
    {
        m_orientation_angle -= _PI_ ;
    }

    optimal_angle = m_orientation_angle;    
    streamdebug.str("");
    streamdebug<<"m_orientation_angle="<<m_orientation_angle<<std::endl;
    qDebug(streamdebug.str().c_str());

    return true;

}


}

}
