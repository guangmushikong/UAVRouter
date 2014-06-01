//! \file
// \brief
// Operator to handle triangle mesh class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#include <niGeom/geometry/niTriMesh2dFn.h>
#include <niGeom/geometry/niTriMesh2d.h>
#include <niGeom/geometry/niGeomMath2d.h>
#include <niGeom/geometry/niTriMesh2dTopo.h>

#include <iostream>
#include <algorithm>
#include <list>
#include <stack>

namespace ni
{
    namespace geometry
    {
        inline bool _sort_(
            const niTripleT<int, int, int> *p1, 
            const niTripleT<int, int, int> *p2)
        {
            return *p1 < *p2;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildFromPoints
        //-----------------------------------------------------------------------------
        /**
        * Build a convex trimesh from points.
        *
        * @param        points:         points
        * @param        convexHullIds:  reference point ids of convexHull
        * @return       true:           success
        *               false:          failed
        */
        bool niTriMesh2dFn::BuildFromPoints(
            const niPoint2dArray &points,
            niIntArray &convexHullIds)
        {
            size_t numPoints    = points.size();
            if (numPoints < 3)
                return false;

            size_t firstPointId = -1;
            double minY         = 1e300;

            for (size_t i = 0; i < numPoints; ++i)
            {
                if (minY > points[i].Y)
                {
                    minY = points[i].Y;
                    firstPointId = i;
                }
            }

            const niPoint2d &p0 = points[ firstPointId ];
            niPoint2d d1(10.0, 0.0), d2;

            size_t numBorder = numPoints-1;

            //angle, length, index
            niArrayT< niTripleT<double, double, int> > alis;
            niTripleT<double, double, int> ali;
            alis.reserve(numBorder);
            for (size_t i = 0; i < numPoints; ++i)
            {
                if (i == firstPointId)
                    continue;

                d2 = points[i] - p0;
                ali.m_v1    = niGeomMath2d::Angle(d1, d2);
                ali.m_v2    = d2.Length();
                ali.m_v3    = i;
                alis.push_back(ali);
            }

            std::sort(alis.begin(), alis.end());

            niTriFace2d         face;
            niTriFace2dArray    faces;
            faces.reserve(numBorder*2);

            int vIdx1 = firstPointId, vIdx2, vIdx3;
            for (size_t i = 0; i < numBorder-1; ++i)
            {
                vIdx2 = alis[i].m_v3;
                vIdx3 = alis[i+1].m_v3;
                face.Init(vIdx1, vIdx2, vIdx3);
                faces.push_back(face);
            }

            std::stack<int> iStack;
            EOrientation orientation;
            vIdx1 = alis[0].m_v3;
            iStack.push( vIdx1 );
            for (size_t i = 1; i < numBorder-1; ++i)
            {
                vIdx1 = iStack.top();
                vIdx2 = alis[i].m_v3;
                vIdx3 = alis[i+1].m_v3;
                orientation = niGeomMath2d::Orientation(
                    points[vIdx1], points[vIdx2], points[vIdx3]);
                if (eCounterClockwise == orientation)
                {
                    iStack.push(vIdx2);
                    vIdx1 = vIdx2;
                }
                else
                {
                    face.Init(vIdx1, vIdx3, vIdx2);
                    faces.push_back(face);
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
                        else
                        {
                            face.Init(vIdx1, vIdx3, vIdx2);
                            faces.push_back(face);
                        }
                    }
                }
            }

            iStack.push(vIdx3);

            bool bStat = m_trimesh.MakeTriMesh(points, faces);
            if (!bStat)
            {
                return false;
            }

            size_t numConvexHull = iStack.size() + 1;
            convexHullIds.resize(numConvexHull);
            int iter = numConvexHull-1;
            while (!iStack.empty())
            {
                convexHullIds[ iter-- ] = iStack.top();
                iStack.pop();
            }
            convexHullIds[ iter ] = firstPointId;

            return iter == 0;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildTriangleFan
        //-----------------------------------------------------------------------------
        /**
        * Make a fan of vert
        *
        * @param       topology:    triangle mesh topology
        * @param       fan:         store result
        * @param       aIdx:        vert id
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::BuildTriangleFan(
            niTriMesh2dTopo &topology,
            _STriMeshFanStrip &fan,
            int aIdx)
        {
            fan.m_IdOfFaces.clear();
            fan.m_IdOfPoints.clear();

            niVert2EF &v2ef = topology.m_v2ef_list[aIdx];

            std::list<int> points_ring;

            int eIdx, bIdx = -1, cIdx = -1;
            int fIdx0 = v2ef.m_v2f[0];

            //Search CounterClockwise
            int fIdx = fIdx0;
            do
            {
                niFace2VE &f2ve = topology.m_f2ve_list[fIdx];
                f2ve.GetVerts(aIdx, bIdx, cIdx);
                fan.m_IdOfFaces.push_back(fIdx);
                points_ring.push_back(cIdx);

                //Find next edge
                eIdx = topology.FindEdge(f2ve, aIdx, cIdx);

                niEdge2VF &e2vf = topology.m_e2vf_list[eIdx];
                fIdx = e2vf.GetFace(aIdx, cIdx);
            } while (-1 != fIdx && fIdx != fIdx0);


            if (fIdx == fIdx0)                  //triangle ring
            {
                niFace2VE &f2ve = topology.m_f2ve_list[fIdx];
                f2ve.GetVerts(aIdx, bIdx, cIdx);
                points_ring.push_front(bIdx);
            }
            else
            {
                //Search Clockwise
                fIdx = fIdx0;
                do
                {
                    niFace2VE &f2ve = topology.m_f2ve_list[fIdx];
                    f2ve.GetVerts(aIdx, bIdx, cIdx);
                    if (fIdx != fIdx0)
                        fan.m_IdOfFaces.push_back(fIdx);
                    points_ring.push_front(bIdx);

                    //Find next edge
                    eIdx = topology.FindEdge(f2ve, aIdx, bIdx);

                    niEdge2VF &e2vf = topology.m_e2vf_list[eIdx];
                    fIdx = e2vf.GetFace(bIdx, aIdx);
                } while (-1 != fIdx && fIdx != fIdx0);
            }

            if (points_ring.size() != fan.m_IdOfFaces.size() + 1)
                return false;

            fan.m_IdOfPoints.clear();
            fan.m_IdOfPoints.push_back(aIdx);

            std::list<int>::iterator iter;
            for (iter = points_ring.begin(); iter != points_ring.end(); iter++)
            {
                fan.m_IdOfPoints.push_back(*iter);
            }

            return true;
        }


        //-----------------------------------------------------------------------------
        // FUNCTION BuildTriangleFans
        //-----------------------------------------------------------------------------
        /**
        * Make fans
        *
        * @param       &fans:       store result
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::BuildTriangleFans(niArrayT<_STriMeshFanStrip> &fans)
        {
            if (!m_trimesh.IsValid())
                return false;

            niTriMesh2dTopo topology;
            bool bStat = topology.BuildTopology(m_trimesh, true);
            if (!bStat)
                return false;

            int num_verts = int(topology.m_v2ef_list.size());
            niArrayT< niTripleT<int, int, int> >    vlinks;
            niArrayT< niTripleT<int, int, int>*>   vlinks_ptr;
            vlinks.resize(num_verts);
            vlinks_ptr.resize(num_verts);

            for (int i = 0; i < num_verts; ++i)
            {
                vlinks[i].m_v1 = int(topology.m_v2ef_list[i].m_v2f.size());
                vlinks[i].m_v2 = i;
                vlinks[i].m_v3 = 0;
                vlinks_ptr[i] = &vlinks[i];
            }

            _STriMeshFanStrip fan;

            do
            {
                std::sort(vlinks_ptr.begin(), vlinks_ptr.end(), _sort_);

                int num = int(vlinks_ptr.size());
                for (int i = num-1; i >= 0; --i)
                {
                    if (vlinks_ptr[i]->m_v3)
                        continue;
                    int aIdx = vlinks_ptr[i]->m_v2;

                    bStat = BuildTriangleFan(topology, fan, aIdx);
                    if (!bStat)
                        return false;

                    // test point orientation of the fan by checking the orientation of its first triangle
                    niPoint2d p = m_trimesh.m_points[ fan.m_IdOfPoints[0] ];
                    niPoint2d q = m_trimesh.m_points[ fan.m_IdOfPoints[1] ];
                    niPoint2d r = m_trimesh.m_points[ fan.m_IdOfPoints[2] ];
                    EOrientation orientation = niGeomMath2d::Orientation(p, q, r);

                    // fix orientation for this fan, by reversing point order
                    if ( orientation == eClockwise )
                    {
                        std::reverse(fan.m_IdOfPoints.begin()+1, fan.m_IdOfPoints.end());
                    }

                    fans.push_back(fan);

                    int n = fan.m_IdOfFaces.size();
                    for (int j = 0; j < n; ++j)
                    {
                        int fIdx = fan.m_IdOfFaces[j];
                        niFace2VE &f2ve = topology.m_f2ve_list[fIdx];
                        vlinks[ f2ve.m_f2v[0] ].m_v3 = 1;
                        vlinks[ f2ve.m_f2v[0] ].m_v1--;
                        vlinks[ f2ve.m_f2v[1] ].m_v3 = 1;
                        vlinks[ f2ve.m_f2v[1] ].m_v1--;
                        vlinks[ f2ve.m_f2v[2] ].m_v3 = 1;
                        vlinks[ f2ve.m_f2v[2] ].m_v1--;
                    }

                    topology.RemoveFaces(fan.m_IdOfFaces);
                }

                niArrayT< niTripleT<int, int, int>*> vlinks_swap;
                for (int i = 0; i < num; ++i)
                {
                    int aIdx = vlinks_ptr[i]->m_v2;
                    niVert2EF &v2ef = topology.m_v2ef_list[aIdx];
                    if (v2ef.m_v2e.size() > 0)
                    {
                        vlinks_ptr[i]->m_v3 = 0;
                        vlinks_swap.push_back(vlinks_ptr[i]);
                    }
                }
                vlinks_ptr = vlinks_swap;
            } while (vlinks_ptr.size() > 0);

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildTriangleStrip
        //-----------------------------------------------------------------------------
        /**
        * Make a strip
        *
        * @param       topology:    triangle mesh topology
        * @param       strip:       store result
        * @param       used_flags:  indicate which face is used
        * @param       fIdx:        face id
        * @param       vIdx:        vert id
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::BuildTriangleStrip(
            niTriMesh2dTopo &topology,
            _STriMeshFanStrip &strip,
            niCharArray &used_flags,
            int fIdx,
            int vIdx)
        {
            int aIdx, bIdx = -1, cIdx = -1, f1, f2, f3, e1, e2, num;

            _STriMeshFanStrip strip_working;

            std::list<int>      points_ring;
            niIntArray          IdOfFaces;

            //Search right
            aIdx = vIdx;
            f1 = fIdx;
            do
            {
                IdOfFaces.push_back(f1);
                used_flags[f1] = 1;

                niFace2VE &f2ve1 = topology.m_f2ve_list[f1];
                f2ve1.GetVerts(aIdx, bIdx, cIdx);

                points_ring.push_back(aIdx);
                points_ring.push_back(cIdx);

                e1 = topology.FindEdge(f2ve1, bIdx, cIdx);
                f2 = topology.m_e2vf_list[e1].GetFace(cIdx, bIdx);
                if (-1 == f2 || used_flags[f2])
                {
                    points_ring.push_back(bIdx);
                    f1 = -1;
                    break;
                }

                IdOfFaces.push_back(f2);
                used_flags[f2] = 1;

                niFace2VE &f2ve2 = topology.m_f2ve_list[f2];

                aIdx = bIdx;

                f2ve2.GetVerts(aIdx, bIdx, cIdx);

                e2 = topology.FindEdge(f2ve2, aIdx, bIdx);
                f3 = topology.m_e2vf_list[e2].GetFace(bIdx, aIdx);

                if (-1 == f3 || used_flags[f3])
                {
                    points_ring.push_back(aIdx);
                    points_ring.push_back(bIdx);
                    f1 = -1;
                    break;
                }
                f1 = f3;
            } while (-1 != f1 && f1 != fIdx);

            if (f1 != fIdx)
            {
                aIdx = vIdx;
                f1 = fIdx;
                do
                {
                    niFace2VE &f2ve1 = topology.m_f2ve_list[f1];
                    f2ve1.GetVerts(aIdx, bIdx, cIdx);
                    if (f1 != fIdx)
                    {
                        used_flags[f1] = 1;
                        IdOfFaces.push_back(f1);
                        points_ring.push_front(cIdx);
                        points_ring.push_front(aIdx);
                    }

                    e1 = topology.FindEdge(f2ve1, cIdx, aIdx);
                    f2 = topology.m_e2vf_list[e1].GetFace(aIdx, cIdx);
                    if (-1 == f2 || used_flags[f2])
                        break;

                    IdOfFaces.push_back(f2);
                    used_flags[f2] = 1;

                    niFace2VE &f2ve2 = topology.m_f2ve_list[f2];
                    f2ve2.GetVerts(aIdx, bIdx, cIdx);

                    e2 = topology.FindEdge(f2ve2, cIdx, aIdx);
                    f3 = topology.m_e2vf_list[e2].GetFace(aIdx, cIdx);

                    if (-1 == f3 || used_flags[f3])
                    {
                        points_ring.push_front(cIdx);
                        f1 = -1;
                        break;
                    }
                    niFace2VE &f2ve3 = topology.m_f2ve_list[f3];
                    f2ve3.GetVerts(aIdx, bIdx, cIdx);
                    f1 = f3;
                    aIdx = cIdx;
                } while (-1 != f1 && f1 != fIdx);

            }

            num = int(IdOfFaces.size());
            for (int i = 0 ; i < num; ++i)
            {
                used_flags[ IdOfFaces[i] ] = 0;
            }

            if (num + 2 != int(points_ring.size()))
                return false;

            if (IdOfFaces.size() <= strip.m_IdOfFaces.size())
                return false;

            strip.m_IdOfPoints.clear();

            std::list<int>::iterator iter;
            for (iter = points_ring.begin(); iter != points_ring.end(); iter++)
            {
                strip.m_IdOfPoints.push_back(*iter);
            }

            strip.m_IdOfFaces = IdOfFaces;

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION BuildTriangleStrips
        //-----------------------------------------------------------------------------
        /**
        * Make strips
        *
        * @param       strips:      store result
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::BuildTriangleStrips(niArrayT<_STriMeshFanStrip> &strips)
        {
            if (!m_trimesh.IsValid())
                return false;

            niTriMesh2dTopo topology;
            bool bStat = topology.BuildTopology(m_trimesh, true);
            if (!bStat)
                return false;

            _STriMeshFanStrip strip;

            int num_faces = int(topology.m_f2ve_list.size());
            int counter = 0;

            niCharArray used_flags;
            used_flags.resize(num_faces, 0);

            while (counter < num_faces)
            {
                for (int fIdx = 0; fIdx < num_faces; ++fIdx)
                {
                    if (used_flags[fIdx])
                        continue;

                    niFace2VE &f2ve = topology.m_f2ve_list[fIdx];

                    strip.m_IdOfFaces.clear();
                    strip.m_IdOfPoints.clear();

                    BuildTriangleStrip(topology, strip, used_flags, fIdx, f2ve.m_f2v[0]);
                    BuildTriangleStrip(topology, strip, used_flags, fIdx, f2ve.m_f2v[1]);
                    BuildTriangleStrip(topology, strip, used_flags, fIdx, f2ve.m_f2v[2]);

                    if (strip.m_IdOfPoints.size() < 1)
                        return false;

                    // test point orientation of the strip by checking the orientation of its first triangle
                    niPoint2d p = m_trimesh.m_points[ strip.m_IdOfPoints[0] ];
                    niPoint2d q = m_trimesh.m_points[ strip.m_IdOfPoints[1] ];
                    niPoint2d r = m_trimesh.m_points[ strip.m_IdOfPoints[2] ];
                    EOrientation orientation = niGeomMath2d::Orientation(p, q, r);

                    // fix orientation for this strip, by adding a fake point at the begining
                    // of the strip, which creates a fake degenerated triangle in the strip, 
                    // which in turn fix the orientation of the whole strip
                    if ( orientation == eClockwise )
                    {
                        strip.m_IdOfPoints.insert( strip.m_IdOfPoints.begin(), strip.m_IdOfPoints[1] );
                    }

                    strips.push_back(strip);
                    topology.RemoveFaces(strip.m_IdOfFaces);
                    int num = int(strip.m_IdOfFaces.size());
                    for (int i = 0 ; i < num; ++i)
                    {
                        used_flags[ strip.m_IdOfFaces[i] ] = 1;
                    }
                    counter += num;
                }
            }
#ifdef _DEBUG
            niIntArray checker;
            checker.resize(num_faces, 0);

            int num_strips = int(strips.size());

            for (int i = 0; i < num_strips; ++i)
            {
                _STriMeshFanStrip &strip = strips[i];
                int nf = int(strip.m_IdOfFaces.size());
                for (int j = 0; j < nf; ++j)
                {
                    checker[ strip.m_IdOfFaces[j] ]++;
                }
            }

            for (int i = 0; i < num_faces; ++i)
            {
                if (checker[i] != 1)
                {
                    std::cout<<"BuildTriangleStrips checker failed"<<std::endl;
                    return false;
                }
            }
#endif
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION OptimizeTriMesh
        //-----------------------------------------------------------------------------
        /**
        * Optimize triangle mesh
        *
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::OptimizeTriMesh()
        {
            if (!m_trimesh.IsValid())
                return false;

            niTriMesh2dTopo topology;
            bool bStat = topology.BuildTopology(m_trimesh, false);
            if (!bStat)
                return false;

            return OptimizeTriMesh(topology);
        }


        //-----------------------------------------------------------------------------
        // FUNCTION OptimizeTriMesh
        //-----------------------------------------------------------------------------
        /**
        * Optimize triangle mesh
        *
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::OptimizeTriMesh(niTriMesh2dTopo &topology)
        {
            if (!m_trimesh.IsValid())
                return false;

            int num_edges = int(topology.m_e2vf_list.size());
            int counter = 0, time = 10;
            do
            {
                counter = 0;
                for (int eIdx = 0; eIdx < num_edges; ++eIdx)
                {
                    OptimizeAdjacentTriangles(topology, eIdx, counter);
                }
                --time;
            } while (counter != 0 && time > 0);

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION OptimizeAdjacentTriangles
        //-----------------------------------------------------------------------------
        /**
        * Optimize triangle mesh
        *
        * @param       topology:    topology of triangle mesh
        * @param       eIdx:        edge id (v1, v2)
        * @param       counter:     return changed number of faces
        * @return      true:        success
        *              false:       failed
        *   Function1:
                If L(v1, v2) > L(v3, v4), need Swap
        *           v1                v1
        *          /| \              /  \
        *         / |  \            /    \
        *        /  |   \          /      \
        *       v3   |   v4  ==>  v3------ v4
        *        \   |  /          \      /
        *          \ | /             \   /
        *            v2                v2
        *
        *   Function2:
        *       If det > 0, need swap
        *       const niPoint2d A, B, C, D;
        *
        *       M[0] = A.X - D.X;   M[1] = A.Y - D.Y;   M[2] = M[0] * (A.X + D.X) + M[1] * (A.Y + D.Y);
        *       M[3] = B.X - D.X;   M[4] = B.Y - D.Y;   M[5] = M[3] * (B.X + D.X) + M[4] * (B.Y + D.Y);
        *       M[6] = C.X - D.X;   M[7] = C.Y - D.Y;   M[8] = M[6] * (C.X + D.X) + M[7] * (C.Y + D.Y);
        *
        *       double A0 = M[4] * M[8] - M[5] * M[7];
        *       double A1 = M[5] * M[6] - M[3] * M[8];
        *       double A2 = M[3] * M[7] - M[4] * M[6];
        *
        *       double det = M[0] * A0 + M[1] * A1 + M[2] * A2;
        //*/
        bool niTriMesh2dFn::OptimizeAdjacentTriangles(niTriMesh2dTopo &topology, int eIdx, int &counter)
        {
            niEdge2VF &e2vf = topology.m_e2vf_list[eIdx];
            if (e2vf.m_e2f[0] == -1 || e2vf.m_e2f[1] == -1)
                return true;

            int vIdx1 = e2vf.m_e2v[0];
            int vIdx2 = e2vf.m_e2v[1];
            int fIdx1 = e2vf.m_e2f[0];
            int fIdx2 = e2vf.m_e2f[1];
            int vIdx3, vIdx4, offset1, offset2;
            niFace2VE &f1 = topology.m_f2ve_list[fIdx1];
            niFace2VE &f2 = topology.m_f2ve_list[fIdx2];

            //Find v3
            for (offset1 = 0; offset1 < 3; ++offset1)
            {
                vIdx3 = f1.m_f2v[offset1];
                if (vIdx3 == vIdx1)
                    continue;
                if (vIdx3 == vIdx2)
                    continue;
                break;
            }

            //Find v4
            for (offset2 = 0; offset2 < 3; ++offset2)
            {
                vIdx4 = f2.m_f2v[offset2];
                if (vIdx4 == vIdx1)
                    continue;
                if (vIdx4 == vIdx2)
                    continue;
                break;
            }

            double l1 = m_trimesh.m_points[vIdx1].DistanceTo(m_trimesh.m_points[vIdx2]);
            double l2 = m_trimesh.m_points[vIdx3].DistanceTo(m_trimesh.m_points[vIdx4]);
            if (l1 <= l2)          //v1v2 is shorter than v3v4, no swap
                return true;

            if (f1.m_f2v[(offset1+1) % 3] == vIdx1)
            {
                vIdx1 += vIdx2;
                vIdx2 = vIdx1 - vIdx2;
                vIdx1 -= vIdx2;
            }

            EOrientation orientation;
            orientation = niGeomMath2d::Orientation(
                m_trimesh.m_points[vIdx2],
                m_trimesh.m_points[vIdx4],
                m_trimesh.m_points[vIdx3]);
            if (orientation == eClockwise)
                return true;
            orientation = niGeomMath2d::Orientation(
                m_trimesh.m_points[vIdx1],
                m_trimesh.m_points[vIdx3],
                m_trimesh.m_points[vIdx4]);
            if (orientation == eClockwise)
                return true;

            //Do swap, update trimesh
            m_trimesh.m_faces[fIdx1][(offset1+2) % 3] = vIdx4;
            m_trimesh.m_faces[fIdx2][(offset2+2) % 3] = vIdx3;

            //update edit edge
            e2vf.Create(vIdx4, vIdx3, fIdx1);
            e2vf.Create(vIdx3, vIdx4, fIdx2);

            //update edit faces
            f1.m_f2v[(offset1+2) % 3] = vIdx4;
            f2.m_f2v[(offset2+2) % 3] = vIdx3;


            int eIdx1, eIdx2;
            //vIdx2, vIdx3, vIdx4
            for (offset1 = 0; offset1 < 3; ++offset1)
            {
                eIdx1 = f1.m_f2e[offset1];
                if (eIdx1 == eIdx)
                    continue;
                niEdge2VF &_tmp = topology.m_e2vf_list[eIdx1];
                if (_tmp.IsEdgeEqual(vIdx2, vIdx3))
                    continue;
                if (_tmp.IsEdgeEqual(vIdx3, vIdx4))
                    continue;
                if (_tmp.IsEdgeEqual(vIdx4, vIdx2))
                    continue;
                _tmp.SetFace(vIdx1, vIdx3, fIdx2);

                break;
            }

            //vIdx1, vIdx3, vIdx4
            for (offset2 = 0; offset2 < 3; ++offset2)
            {
                eIdx2 = f2.m_f2e[offset2];
                if (eIdx2 == eIdx)
                    continue;
                niEdge2VF &_tmp = topology.m_e2vf_list[eIdx2];
                if (_tmp.IsEdgeEqual(vIdx1, vIdx3))
                    continue;
                if (_tmp.IsEdgeEqual(vIdx3, vIdx4))
                    continue;
                if (_tmp.IsEdgeEqual(vIdx4, vIdx1))
                    continue;
                _tmp.SetFace(vIdx2, vIdx4, fIdx1);
                break;
            }
            f1.m_f2e[offset1] = eIdx2;
            f2.m_f2e[offset2] = eIdx1;

            ++counter;
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Reverse
        //-----------------------------------------------------------------------------
        /**
        * Reverse the triangle
        *
        * @return      true:        success
        *              false:       failed
        */
        bool niTriMesh2dFn::Reverse()
        {
            if (!m_trimesh.IsValid())
                return false;

            int offset = m_trimesh.NumPoints() - 1;

            int _a, _c;
            std::reverse(m_trimesh.m_points.begin(), m_trimesh.m_points.end());
            int num_faces = m_trimesh.NumFaces();
            for (int i = 0; i < num_faces; ++i)
            {
                niTriFace2d &f = m_trimesh.m_faces[i];
                _a = offset - f.cIdx;
                _c = offset - f.aIdx;
                f.aIdx = _a;
                f.bIdx = offset - f.bIdx;
                f.cIdx = _c;
            }

            return true;
        }

    }
}
