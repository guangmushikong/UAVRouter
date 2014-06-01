//! \file
// \brief
// Topology of triangle mesh
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version

#ifndef niTriMesh3dTopo_H
#define niTriMesh3dTopo_H

#include <niGeom/niTypes.h>
#include <niGeom/geometry/niGeomDef.h>

namespace ni
{
    namespace geometry
    {
        class niTriMesh3d;
        /**
         * \brief Topology of 3d triangle mesh
         *
         */
        class niTriMesh3dTopo
        {
        public:
            niTriMesh3dTopo         (){}

            niTriMesh3dTopo         (const niTriMesh3dTopo &topo_mesh);

            ~niTriMesh3dTopo        ();

            niTriMesh3dTopo&        operator=           (const niTriMesh3dTopo &other);

            bool                    BuildTopology       (const niTriMesh3d& trimesh, bool fBuildVert);
            
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

            bool                    BuildEdge2VF        (const niTriMesh3d& trimesh);
            bool                    BuildFace2VE        (const niTriMesh3d& trimesh);
            bool                    BuildVert2EF        (const niTriMesh3d& trimesh);

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
