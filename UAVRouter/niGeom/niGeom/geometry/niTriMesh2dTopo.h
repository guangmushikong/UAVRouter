//! \file
// \brief
// Topology of triangle mesh
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTriMesh2dTopo_H
#define niTriMesh2dTopo_H

#include <niGeom/niTypes.h>
#include <niGeom/geometry/niGeomDef.h>

namespace ni
{
    namespace geometry
    {
        class niTriMesh2d;
        class niTriMesh2dFn;

        /**
         * \brief Topology of 2d triangle mesh
         *
         */
        class niTriMesh2dTopo
        {
            friend class niTriMesh2dFn;

        public:
            niTriMesh2dTopo         (){}

            niTriMesh2dTopo         (const niTriMesh2dTopo &topo_mesh);

            ~niTriMesh2dTopo        ();

            niTriMesh2dTopo&        operator=           (const niTriMesh2dTopo &other);

            bool                    BuildTopology       (const niTriMesh2d& trimesh, bool fBuildVert);
            
            const niVert2efArray&   GetVert2EF          () const
            {
                return m_v2ef_list;
            }
            const niEdge2vfArray&   GetEdge2VF          () const
            {
                return m_e2vf_list;
            }
            const niFace2veArray&   GetFace2VE          () const
            {
                return m_f2ve_list;
            }

            int                     FindEdge            (niFace2VE &f2ve, int v1, int v2);

            bool                    IsValid             () const
            {
                return m_valid;
            }

            bool                    RemoveFace          (int f);

            bool                    RemoveFaces         (niIntArray fs);


        protected:

            bool                    BuildEdge2VF        (const niTriMesh2d& trimesh);
            bool                    BuildFace2VE        (const niTriMesh2d& trimesh);
            bool                    BuildVert2EF        (const niTriMesh2d& trimesh);

            bool                    Destory             ();

            bool                    m_valid;

        protected:
            niVert2efArray          m_v2ef_list;
            niEdge2vfArray          m_e2vf_list;
            niFace2veArray          m_f2ve_list;
        };
    }
}
#endif

