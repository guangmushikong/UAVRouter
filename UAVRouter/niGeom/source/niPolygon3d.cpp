#include <niGeom/geometry/niPolygon3d.h>

#include <fstream>
#include <iomanip>

namespace ni
{
    namespace geometry
    {
        //-----------------------------------------------------------------------------
        // FUNCTION Append
        //-----------------------------------------------------------------------------
        /**
        * Add a vert into polygon.
        *
        * @param       other:           point
        */
        void niPolygon3d::Append(const niVert3d &other)
        {
            int num = int(m_verts.size());
            switch(num)
            {
            case 0:
                break;
            case 1:
                if (other == m_verts[0])
                    return;
            default:
                if (other == m_verts[0])
                    return;
                if (other == m_verts[num-1])
                    return;
            }
            m_verts.push_back(other);
            m_valid = (m_verts.size() > 2);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION GetVerts
        //-----------------------------------------------------------------------------
        /**
        * Get a copy of verts in polygon
        *
        * @param       verts:       store return verts
        * @return      true:        success
        *              false:       failed
        */
        bool niPolygon3d::GetVerts(
            niVert3dArray &verts) const
        {
            verts.clear();
            if (!m_valid)
                return false;
            verts = m_verts;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION MakePolygon
        //-----------------------------------------------------------------------------
        /**
        * Make a polygon from input points
        *
        * @param       points:      input points
        * @return      true:        success to create a polygon
        *              false:       failed to create a polygon
        */
        bool niPolygon3d::MakePolygon(niVert3dArray &verts)
        {
            m_verts.clear();
            m_valid = false;
            int num = int(verts.size());
            if (num < 3)
                return false;

            m_verts.push_back(verts[0]);

            niVert3d lastVert = verts[0];
            for (int i = 1; i < num; ++i)
            {
                niVert3d &point = verts[i];
                if (point != lastVert)
                {
                    m_verts.push_back(point);
                    lastVert = point;
                }
            }
            if (lastVert == verts[0])
                m_verts.pop_back();

            m_valid = (m_verts.size() >= 3);

            return m_valid;
        }
    }
}