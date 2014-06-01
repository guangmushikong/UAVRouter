//! \file
// \brief
// Operator to handle polygon  class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niPolygon2dFn_H
#define niPolygon2dFn_H

#include <niGeom/geometry/niGeomDef.h>

namespace ni
{
    namespace geometry
    {
        class niPolygon2d;

        /**
         * \brief Operator to handle polygon  class
         *
         */
        class niPolygon2dFn
        {
        public:
            niPolygon2dFn       (niPolygon2d &polygon) : m_polygon(polygon)    {}

            virtual ~niPolygon2dFn(){}

            double              Area                () const;

            EOrientation        Orientation         () const;

            bool                Reverse             ();

            bool                Scale               (double _scale_);

            bool                TransToCenter       ();

        protected:
            niPolygon2d&        m_polygon;
        };
    }
}


#endif
