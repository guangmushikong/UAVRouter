//! \file
// \brief
// Operator to handle triangle  class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTriangle2dFn_H
#define niTriangle2dFn_H

#include <niGeom/geometry/niGeomDef.h>

namespace ni
{
    namespace geometry
    {
        class niTriangle2d;


        /**
         * \brief Operator to handle triangle  class
         *
         */
        class niTriangle2dFn
        {
        public:
            niTriangle2dFn              (niTriangle2d &triangle) : m_triangle(triangle)    {}

            virtual ~niTriangle2dFn     (){}

            EOrientation                Orientation                 () const;

            bool                        Reverse                     ();

        protected:
            niTriangle2d&               m_triangle;
        };
    }
}

#endif
