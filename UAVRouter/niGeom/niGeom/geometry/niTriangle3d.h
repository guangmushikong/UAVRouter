//! \file
// \brief
// 3D Triangle
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTriangle3d_H
#define niTriangle3d_H

#include <niGeom/geometry/niGeom3dTypes.h>

namespace ni
{
    namespace geometry
    {
        class niGeomMath3d;
        class niTriangle3dFn;

        /**
         * \brief 3D Triangle
         *
         */
        class niTriangle3d
        {
            friend class niGeomMath3d;
            friend class niTriangle3dFn;
        public:
            niTriangle3d            () : m_valid(false)    {}

            niTriangle3d            (const niVert3d &v0, const niVert3d &v1, const niVert3d &v2)
            {
                MakeTriangle(v0, v1, v2);
            }

            niTriangle3d            (const niTriangle3d& other)
            {
                m_valid = other.m_valid;
                P0 = other.P0;
                P1 = other.P1;
                P2 = other.P2;
            }

            inline bool             IsValid                 () const
            {
                return m_valid;
            }

            bool                    MakeTriangle            (
                                                            const niVert3d &v0,
                                                            const niVert3d &v1,
                                                            const niVert3d &v2)
            {
                m_valid = false;
                if (v0 == v1)
                    return false;
                if (v1 == v2)
                    return false;
                if (v2 == v0)
                    return false;
                P0 = v0;
                P1 = v1;
                P2 = v2;
                m_valid = true;
                return true;
            }

            inline niVert3d        operator[]              (int idx) const
            {
                return (&P0)[idx%3];
            }

            inline niTriangle3d&    operator=               (const niTriangle3d& other)
            {
                m_valid = other.m_valid;
                P0 = other.P0;
                P1 = other.P1;
                P2 = other.P2;
            return *this;
            }

        protected:
            bool                    m_valid;
            niVert3d                P0;
            niVert3d                P1;
            niVert3d                P2;
        };
    }
};



#endif