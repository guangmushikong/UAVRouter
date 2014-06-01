//! \file
// \brief
// Define tree types
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTreeTypes_H
#define niTreeTypes_H

#include <niGeom/geometry/niBBox2d.h>

namespace ni
{
    namespace geometry
    {
        namespace tree
        {
            /**
             * \brief bbox cost
             *
             */
            struct niBBoxCost
            {
                niBBox2d            m_bbox;
                float               m_cost;
            };
        }
    }
}

#endif
