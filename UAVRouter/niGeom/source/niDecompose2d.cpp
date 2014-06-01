//! \file
// \brief
// Decompose multiply connected polygon into simply connected polygons
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version


#include <niGeom/geometry/niDecompose2d.h>

#include <niGeom/geometry/niBBox2d.h>
#include <niGeom/geometry/niGeomMath2d.h>
#include <niGeom/geometry/niPolygon2d.h>
#include <niGeom/geometry/niPolygon2dFn.h>

#include <niGeom/geometry/niTriMesh2d.h>
#include <niGeom/geometry/niTriMesh2dFn.h>
#include <niGeom/geometry/niTriMesh2dTopo.h>
#include <niGeom/geometry/niTriangulation2d.h>

#include <niGeom/algorithm/niBubbleSort.h>

#ifndef COORDSCALE_NDH2NDS
    #define COORDSCALE_NDH2NDS  119.30464711111111111111111111111
#endif

#ifndef COORDSCALE_WGS2NDS
    #define COORDSCALE_WGS2NDS  11930464.711111111111111111111111
#endif

#include <stack>
#include <iostream>
#include <fstream>
#include <iomanip>
namespace ni
{
    namespace geometry
    {
        const char CDuplicateMarker = 120;

        niDecompose2d::_SEdge2Vert::_SEdge2Vert() : m_s(-1), m_t(-1), m_dirty(false)
        {
        }

        niDecompose2d::_SEdge2Vert::_SEdge2Vert(int s, int t) : m_s(s), m_t(t), m_dirty(false)
        {
        }

        niDecompose2d::niTopoPoly::niTopoPoly()
            : m_IdOfPoly(-1)
            , m_IdOfGroup(-1)
            , m_hBvh4Edge(NULL)
            , m_hBsp4Point(NULL)
            , m_area(0.0)
            , m_splited(false)
        {
        }

        niDecompose2d::niTopoPoly::~niTopoPoly()
        {
            if (NULL != m_hBsp4Point)
            {
                delete m_hBsp4Point;
                m_hBsp4Point = NULL;
            }
            if (NULL != m_hBvh4Edge)
            {
                delete m_hBvh4Edge;
                m_hBvh4Edge = NULL;
            }
        }


        //-----------------------------------------------------------------------------
        // FUNCTION Construction
        //-----------------------------------------------------------------------------
        /**
        * Init niDecompose2d
        * @param        outer:          The outer polygon of the multiply connected polyon
        * @param        inners:         The inner polygons of the multiply connected polyon
        * @param        method:         Decompose method, indicate the refine method
        * @param        coordShift:     The threshold of refine parameter used to avoid coordshift error.
        *                                   If the some split edge is to close to the other inner polygon,
        *                                   coordshift operation will cause some polygon self-intersection.
        * 
        */
        niDecompose2d::niDecompose2d(
            const niPolygon2d &outer,
            const niPolygon2dArray &inners,
            EDecomposeMethod method,
            double coordShift)
            : m_method(method)
            , m_coordShift(coordShift)
            , m_inputArea(0.0)
            , m_bRevered(false)
            , m_hBvh4TopoEdge(NULL)
            , m_hBsp4Point(NULL)
            , m_hBvh4SplitEdge(NULL)
            , m_numSplitEdgeInBVH(0)
        {
            size_t numInners = inners.size();
            m_polygons.resize(1 + numInners);
            m_polygons[0] = outer;
            for (size_t i = 0; i < numInners; ++i)
            {
                m_polygons[i+1] = inners[i]; 
            }

            _CheckAndReverse();
        }

        //-----------------------------------------------------------------------------
        // FUNCTION DeConstruction
        //-----------------------------------------------------------------------------
        /**
        * Destory niDecompose2d
        * 
        */
        niDecompose2d::~niDecompose2d()
        {
            _Destory();
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Process
        //-----------------------------------------------------------------------------
        /**
        * Do decompose
        * @param        polygons:       used to store the decomposed simply polygons.
        * @param        error_msg:      error messages.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::Process(niPolygon2dArray &polygons, String &error_msg)
        {
            polygons.clear();
            bool bStat = _Init();
            if (!bStat)
            {
                error_msg = "Init Failed! Terminated!";
                return false;
            }

            if (eDMDelaunay == m_method)
            {
                bStat = _CalcSplitEdges();
                if (!bStat)
                {
                    error_msg = "Using eDMDelaunay method! CalcSplitEdges Failed! Terminated!";
                    return false;
                }

                _RemoveDuplicateSplitEdges();
                bStat = _RefineSplitEdges();
                if (!bStat)
                {
                    error_msg = "Using eDMDelaunay method! RefineSplitEdges Failed! Terminated!";
                    return false;
                }
                bStat = _CalcDelaunaySplit();
            }
            else
            {
                bStat = _CalcSplitEdges();
                if (!bStat)
                {
                    error_msg = "Using default method! CalcSplitEdges Failed! Terminated!";
                    return false;
                }
            }

            bStat = _BuildPolygons(polygons);
            if (!bStat)
            {
                std::cout<<"\t\t_BuildPolygons ERROR"<<std::endl;
            }

            return bStat;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Construction
        //-----------------------------------------------------------------------------
        /**
        * Init polygon builder
        * @param        points:         points of multiply connected polygon
        * @param        topoEdges:      topo edges of multiply connected polygon
        * @param        splitEdges:     split edges
        * 
        */
        niDecompose2d::niPolygonBuilder::niPolygonBuilder(
            const niPoint2dArray &points,
            const niArrayT<niTopoEdge>& topoEdges,
            const niArrayT<niSplitEdge>& splitEdges) : m_points(points)
        {
            Init(topoEdges, splitEdges);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Init
        //-----------------------------------------------------------------------------
        /**
        * Init polygon builder
        * @param        topoEdges:      topo edges of multiply connected polygon
        * @param        splitEdges:     split edges
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::niPolygonBuilder::Init(
            const niArrayT<niTopoEdge>& topoEdges,
            const niArrayT<niSplitEdge>& splitEdges)
        {

            size_t numPoints    = m_points.size();
            size_t numTopoEdges = topoEdges.size();
            size_t numSplits    = splitEdges.size();
            size_t numEdges     = numTopoEdges + numSplits * 2;

            try
            {
                m_v2es.resize(numPoints);
                m_e2vs.reserve(numEdges);
            }
            catch (std::bad_alloc)
            {
                return false;
            }

            _SEdge2Vert e2v;
            for (size_t i = 0; i < numTopoEdges; ++i)
            {
                const niTopoEdge &te = topoEdges[i];
                e2v.Init( te.A().m_IdOfPoint, te.B().m_IdOfPoint );
                m_e2vs.push_back( e2v );
            }
            for (size_t i = 0; i < numSplits; ++i)
            {
                const niSplitEdge &splitEdge = splitEdges[i];
                if (splitEdge.IsExcluded())
                    continue;
                e2v.Init( splitEdge.A().m_IdOfPoint, splitEdge.B().m_IdOfPoint );
                m_e2vs.push_back( e2v );
                e2v.Init( splitEdge.B().m_IdOfPoint, splitEdge.A().m_IdOfPoint );
                m_e2vs.push_back( e2v );
            }

            numEdges = m_e2vs.size();
            for (size_t eIdx = 0; eIdx < numEdges; ++eIdx)
            {
                m_v2es[ m_e2vs[ eIdx ].S() ].AddFrom( eIdx );
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION Build
        //-----------------------------------------------------------------------------
        /**
        * Build simply polygons
        * @param        pointRefRings:      point reference ids of simply polygons
        * @param        duplicatePointIds:  record duplicate points reference ids
        * return        true:               success
        *               false:              failed
        * 
        */
        bool niDecompose2d::niPolygonBuilder::Build(
            niArrayT<niIntArray> &pointRefRings,
            niIntArray &duplicatePointIds)
        {
            niIntArray pointRefRing;

            niPoint2dArray points;

            size_t numPoints = m_points.size();
            niIntArray marker;
            marker.resize(numPoints, 0);

            size_t numEdges = m_e2vs.size();
            for (size_t firstEdgeId = 0; firstEdgeId < numEdges; ++firstEdgeId)
            {
                _SEdge2Vert &currEdge = m_e2vs[ firstEdgeId ]; 
                if ( currEdge.IsDirty() )
                    continue;

                int lastVertId = currEdge.S();
                int currVertId = currEdge.T();

                pointRefRing.clear();

                m_e2vs[ firstEdgeId ].SetDirty(true);

                pointRefRing.push_back( lastVertId );

                bool bStat = true;

                while (bStat)
                {
                    pointRefRing.push_back( currVertId );

                    int nextEdgeId = -1;
                    int nextVertId = -1;
                    bStat = FindNextEdge(
                        firstEdgeId,
                        lastVertId,
                        currVertId,
                        nextVertId,
                        nextEdgeId);

                    if (bStat && nextEdgeId != -1)
                    {
                        m_e2vs[ nextEdgeId ].SetDirty(true);

                        lastVertId = currVertId;
                        currVertId = nextVertId;
                    }
                }
                size_t numVerts = pointRefRing.size() - 1;
                if (numVerts >= 3)
                {
                    points.clear();

                    for (size_t i = 0; i < numVerts; ++i)
                    {
                        int vIdx = pointRefRing[ i ];
                        marker[vIdx]++;
                    }
                    for (size_t i = 0; i < numVerts; ++i)
                    {
                        int vIdx = pointRefRing[ i ];
                        if (marker[vIdx] != 1)
                        {
                            duplicatePointIds.push_back(vIdx);
                        }
                        marker[vIdx] = 0;
                    }

                    pointRefRings.push_back( pointRefRing );
                }
                else
                {
                    std::cout<<"ERROR HERE : numVerts < 3"<<std::endl;
                    return false;
                }
            }

            return pointRefRings.size() > 1;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION FindNextEdge
        //-----------------------------------------------------------------------------
        /**
        * Find the next edge to build a polygon.
        * @param        firstEdgeId:    the first edge id of a new decomposed polygon
        * @param        lastVertId:     the last vert id
        * @param        currVertId:     the current vert id
        * @param        nextVertId:     the next vert id
        * return        true:           success
        *               false:          failed
        *
        * lastVertId = P1
        * currVertId = P2
        * Because the angle P1-P2-P4 < P1-P2-P4, so:
        * currVertId = P4
        * nextVertId = b
        *                  P4
        *                 / 
        *                / b
        *        a      /
        *  P1---->-----P2
        *               \ c
        *                \
        *                 \
        *                  P3
        * 
        */
        bool niDecompose2d::niPolygonBuilder::FindNextEdge(
            int firstEdgeId,
            int lastVertId,
            int currVertId,
            int &nextVertId,
            int &nextEdgeId)
        {
            _SVert2Edge &currVert = m_v2es[ currVertId ];
            int numFrom = currVert.NumFrom();
            if (1 == numFrom)
            {
                nextEdgeId = currVert.GetFrom(0);
                if (nextEdgeId == firstEdgeId)
                    return false;

                if (m_e2vs[ nextEdgeId ].IsDirty())
                    return false;

                nextVertId = m_e2vs[ nextEdgeId ].T();

                return true;
            }
            else if (numFrom > 1)
            {
                nextVertId = -1;
                nextEdgeId = -1;
                bool bFound = false;
                double minAngle = 1e300;
                for (int i = 0; i < numFrom; ++i)
                {
                    int edgeId = currVert.GetFrom(i);
                    if (edgeId == firstEdgeId)
                        return false;
                    _SEdge2Vert &nextEdge = m_e2vs[ edgeId ];
                    if (nextEdge.IsDirty())
                        continue;
                    int vertId = nextEdge.T();
                    double angle = ni::geometry::niGeomMath2d::Angle(
                        m_points[ currVertId ],
                        m_points[ vertId ],
                        m_points[ lastVertId ]);

                    if (angle < minAngle)
                    {
                        if ( nextEdge.IsDual( lastVertId, currVertId ) )
                        {
                            //Do nothing
                        }
                        else if (angle < ZEROD)
                        {
                            //Do nothing
                        }
                        else
                        {
                            minAngle    = angle;
                            nextEdgeId  = edgeId;
                            nextVertId  = vertId;
                            bFound      = true;
                        }
                    }
                }
                return bFound;
            }
            else
            {
                return false;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _AddToSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Add split edge
        * @param        splitEdge:      split edge
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_AddToSplitEdges(
            niSplitEdge &splitEdge)
        {
            m_splitEdges.push_back( splitEdge );

            int IdOfPoint1, IdOfPoint2, IdOfPoly1, IdOfPoly2;
            IdOfPoint1 = splitEdge.A().m_IdOfPoint;
            IdOfPoint2 = splitEdge.B().m_IdOfPoint;
            IdOfPoly1 = splitEdge.A().m_IdOfPoly;
            IdOfPoly2 = splitEdge.B().m_IdOfPoly;

            niTopoPoly &topoPoly1 = m_topoPolys[ IdOfPoly1 ];
            niTopoPoly &topoPoly2 = m_topoPolys[ IdOfPoly2 ];

            niPairT<int, int> splitPair;
            SplitMaps_T::iterator findPairIter;
            
            splitPair.Init(IdOfPoint1, IdOfPoly2);

            topoPoly1.AddSplitPair(splitPair);
            findPairIter = m_splitMap4Point.find( splitPair );
            if (m_splitMap4Point.end() == findPairIter)
            {
                m_splitMap4Point.insert( std::make_pair( splitPair, 1 ) );
            }
            else if (0 == findPairIter->second)
            {
                ++findPairIter->second;
            }
            else
            {
                std::cout<<"_AddToSplitEdges    ERROR   AAA"<<std::endl;
            }

            splitPair.Init(IdOfPoint2, IdOfPoly1);
            topoPoly2.AddSplitPair(splitPair);
            findPairIter = m_splitMap4Point.find( splitPair );
            if (m_splitMap4Point.end() == findPairIter)
            {
                m_splitMap4Point.insert( std::make_pair( splitPair, 1 ) );
            }
            else if (0 == findPairIter->second)
            {
                ++findPairIter->second;
            }
            else
            {
                std::cout<<"_AddToSplitEdges    ERROR   AAA"<<std::endl;
            }
            
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _AddToSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Add split edge
        * @param        splitEdges:     split edges
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_AddToSplitEdges(
            niArrayT<niSplitEdge> &splitEdges)
        {
            size_t numSplits = splitEdges.size();
            if (numSplits < 1)
                return false;

            std::sort(splitEdges.begin(), splitEdges.end());

            niSplitEdge lastSplitEdge = splitEdges[0];

            _AddToSplitEdges(lastSplitEdge);

            for (size_t i = 1; i < numSplits; ++i)
            {
                bool bOverlap = false;
                niSplitEdge &splitEdge1 = splitEdges[i];
                if (lastSplitEdge == splitEdge1)
                {
                    bOverlap = true;
                }
                else
                {
                    for (size_t j = 0; j < i; ++j)
                    {
                        niSplitEdge &splitEdge2 = splitEdges[j];
                        bOverlap = niGeomMath2d::LineOverlapLine(
                            *splitEdge1.A().m_pointRef,
                            *splitEdge1.B().m_pointRef,
                            *splitEdge2.A().m_pointRef,
                            *splitEdge2.B().m_pointRef);
                        if (bOverlap)
                            break;
                    }
                }
                if (!bOverlap)
                {
                    _AddToSplitEdges(splitEdge1);
                    lastSplitEdge = splitEdge1;
                }
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _BuildPolygons
        //-----------------------------------------------------------------------------
        /**
        * Build decomposed simply polygons
        * @param        polygons:       store decomposed simply polygons
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_BuildPolygons(niPolygon2dArray &polygons)
        {
            polygons.clear();

            niPolygonBuilder builder(
                m_fixedPoints,
                m_topoEdges,
                m_splitEdges);

            niArrayT<niIntArray> pointRefRings;

            niIntArray duplicatePointIds;
            bool bStat = builder.Build(pointRefRings, duplicatePointIds);

            if (!bStat || duplicatePointIds.size() > 0)
            {
                std::cout<<"\t_BuildPolygons ERROR!"<<std::endl;
                return false;
            }

            bStat = _TestByArea(pointRefRings);
            if (!bStat)
            {
                std::cout<<"\t_BuildPolygons ERROR!"<<std::endl;
                return false;
            }

            niPolygon2d polygon;
            niPoint2dArray points;
            size_t numRings = pointRefRings.size();
            for (size_t i = 0; i < numRings; ++i)
            {
                points.clear();
                niIntArray &pointRefRing = pointRefRings[i];
                size_t numPoints = pointRefRing.size();
                for (size_t vi = 0; vi <numPoints; ++vi)
                {
                    int refId = pointRefRing[ vi ];
                    points.push_back( m_origalPoints[ refId ] );
                }

                polygon.MakePolygon(points);
                if (polygon.IsValid())
                {
                    polygons.push_back(polygon);
                }
            }

            if (polygons.size() != pointRefRings.size())
            {
                std::cout<<"\tpolygons.size() != pointRefRings.size()"<<std::endl;
                return false;
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcTopoPolyInfo
        //-----------------------------------------------------------------------------
        /**
        * If the inner polygons is connect to each other by split edges, set them as a
        * group by group id.
        * @param        splitEdges:     split edges
        * @param        groups:         store connected inner polygons' ids
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcTopoPolyInfo(
            niArrayT<niSplitEdge>& splitEdges,
            niArrayT< niIntArray > &groups)
        {
            size_t numPolygons = m_topoPolys.size();
            groups.resize(numPolygons);

            for (size_t i = 1; i < numPolygons; ++i)
            {
                int idOfGroup = m_topoPolys[i].m_IdOfGroup;
                groups[idOfGroup].push_back( i );
            }

            size_t numSplitEdges = splitEdges.size();
            for (size_t i = 0; i < numSplitEdges; ++i)
            {
                niSplitEdge &sp = splitEdges[i];
                int idOfPoly1 = sp.A().m_IdOfPoly;
                int idOfPoly2 = sp.B().m_IdOfPoly;

                int idOfGroup1 = m_topoPolys[idOfPoly1].m_IdOfGroup;
                int idOfGroup2 = m_topoPolys[idOfPoly2].m_IdOfGroup;

                if (0 == idOfGroup1 || 0 == idOfGroup2)
                    continue;

                if (idOfGroup1 > idOfGroup2)
                {
                    idOfGroup1 += idOfGroup2;
                    idOfGroup2 = idOfGroup1 - idOfGroup2;
                    idOfGroup1 -= idOfGroup2;
                }
                niIntArray &group1 = groups[idOfGroup1];
                niIntArray &group2 = groups[idOfGroup2];
                size_t num = group2.size();
                for (size_t i = 0; i < num; ++i)
                {
                    int polyId = group2[i];
                    m_topoPolys[ polyId ].m_IdOfGroup = idOfGroup1;
                }
                group1.insert(group1.end(), group2.begin(), group2.end());
                group2.clear();
            }
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcOuterBBox
        //-----------------------------------------------------------------------------
        /**
        * Calc the bbox of the orignal outer polygon
        * 
        */
        void niDecompose2d::_CalcOuterBBox()
        {
            const niPoint2dArray &points = m_polygons[0].GetPoints();
            m_originalBBox.Reset();
            size_t numPoints = points.size();

            for (size_t i = 0; i < numPoints; ++i)
            {
                m_originalBBox.Append( points[i] );
            }
        }


        //-----------------------------------------------------------------------------
        // FUNCTION _CalcDelaunaySplit
        //-----------------------------------------------------------------------------
        /**
        * Use the gravity of inner polygons to represent inner polygons, then calc the
        *   delaunay trimesh of all gravityies. Each border of the delaunay trimesh mean
        *   there should be a new split edge to split the two inner polygons.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcDelaunaySplit()
        {
            niArrayT< niTripleT<double, int, int> > splitTriples;
            niIntArray convexHullIds;
            bool bStat = _InitDelaunaySplit(splitTriples, convexHullIds);
            if (!bStat)
            {
                return false;
            }

            niTopoPoint topoPoint1;
            niTopoPoint topoPoint2;
            double length;

            size_t numSplits = splitTriples.size();
            for (size_t i = 0; i < numSplits; ++i)
            {
                int IdOfPoly1 = splitTriples[i].m_v2;
                int IdOfPoly2 = splitTriples[i].m_v3;
                niTopoPoly &topoPoly1 = m_topoPolys[ IdOfPoly1 ];
                niTopoPoly &topoPoly2 = m_topoPolys[ IdOfPoly2 ];
                //if (topoPoly1.NumSplitPolys() > 1 &&
                  //  topoPoly2.NumSplitPolys() > 1)
                    //continue;

                niPairT<int, int> splitPair;
                if (IdOfPoly1 < IdOfPoly2)
                    splitPair.Init(IdOfPoly1, IdOfPoly2);
                else
                    splitPair.Init(IdOfPoly2, IdOfPoly1);

                if (m_splitMap4Edge.end() == m_splitMap4Edge.find(splitPair))
                {
                    length = splitTriples[i].m_v1;

                    bool bStat = _FindNearstEdge(
                        topoPoly1.m_hBsp4Point->Root(),
                        topoPoly2.m_hBsp4Point->Root(),
                        topoPoint1,
                        topoPoint2,
                        length);
                    if (bStat)
                    {
                        niSplitEdge newSplitEdge(topoPoint1, topoPoint2, length);
                        _AddToSplitEdges( newSplitEdge );
                        m_splitMap4Edge.insert( std::make_pair(splitPair, 1) );
                    }
                }
            }

            size_t numConvexHulls = convexHullIds.size();
            for (size_t i = 0; i < numConvexHulls; ++i)
            {
                int IdOfPoly1 = 0;
                int IdOfPoly2 = convexHullIds[i];

                niTopoPoly &topoPoly1 = m_topoPolys[ IdOfPoly1 ];
                niTopoPoly &topoPoly2 = m_topoPolys[ IdOfPoly2 ];

                niPairT<int, int> splitPair;
                if (IdOfPoly1 < IdOfPoly2)
                    splitPair.Init(IdOfPoly1, IdOfPoly2);
                else
                    splitPair.Init(IdOfPoly2, IdOfPoly1);

                if (m_splitMap4Edge.end() == m_splitMap4Edge.find(splitPair))
                {
                    length = 1e300;

                    bool bStat = _FindNearstEdge(
                        topoPoly1.m_hBsp4Point->Root(),
                        topoPoly2.m_hBsp4Point->Root(),
                        topoPoint1,
                        topoPoint2,
                        length);
                    if (bStat)
                    {
                        niSplitEdge newSplitEdge(topoPoint1, topoPoint2, length);
                        _AddToSplitEdges( newSplitEdge );
                        m_splitMap4Edge.insert( std::make_pair(splitPair, 1) );
                    }
                }
            }
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcNearestSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Calc nearest split edge for each inner polygon.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcNearestSplitEdges()
        {
            niTopoPoint topoPoint1, topoPoint2;
            size_t numPolygons = m_topoPolys.size();
            niBoolArray visitMaker;
            visitMaker.resize(numPolygons, false);
            for (size_t i = 1; i < numPolygons; ++i)
            {
                if (visitMaker[i])
                    continue;
                double length = 1e300;
                bool bFound = _FindNearstEdge(
                    m_topoPolys[i].m_hBsp4Point->Root(),
                    this->m_hBsp4Point->Root(),
                    topoPoint1,
                    topoPoint2,
                    length);
                if (bFound)
                {
                    niSplitEdge splitEdge(topoPoint1, topoPoint2, length, true);
                    _AddToSplitEdges(splitEdge);
                    visitMaker[ topoPoint1.m_IdOfPoly ] = true;
                    visitMaker[ topoPoint2.m_IdOfPoly ] = true;
                }
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * For each inner polygon:
        * first: scan left, find a split edge start from the left point of polygon to 
        *           another polygon, until reach outer polygon or other splited inner 
        *           polygon, and record all visited polygons for this loop.
        * second: scan right, find a split edge start from the left point of polygon to
        *           another polygon, until reach outer polygon or other splited inner 
        *           polygon, and record all visited polygons for this loop.
        * Then: all visited polygons are splited.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcSplitEdges()
        {
            double length;
            bool bFound;
            niTopoPoint topoPoint2;
            niTopoPoly* topoPolyPtr;
            niBoolArray visitMarker;
            niIntArray  splitedPolyIds;

            size_t numSplited;
            size_t numPolygons = m_topoPolys.size();

            niArrayT< niTopoPoly* > sortedTopoPolyRefs;
            sortedTopoPolyRefs.reserve(numPolygons-1);
            for (size_t i = 1; i < numPolygons; ++i)
            {
                sortedTopoPolyRefs.push_back( &m_topoPolys[i] );
            }

            std::sort(sortedTopoPolyRefs.begin(), sortedTopoPolyRefs.end(), niTopoPoly::_compare_);

            visitMarker.resize(numPolygons, false);
            for (size_t polyIter = 0; polyIter < numPolygons-1; ++polyIter)
            {
                topoPolyPtr = sortedTopoPolyRefs[polyIter];
                if (topoPolyPtr->IsSplited())
                    continue;

                //Scan left
                while (1)
                {
                    length = 1e300;
                    bFound = _CalcSplitEdges(
                        topoPolyPtr->m_lTopoPoint,
                        topoPoint2,
                        visitMarker,
                        length);
                    if (!bFound)
                    {
                        numSplited = splitedPolyIds.size();
                        for (size_t i = 0; i < numSplited; ++i)
                        {
                            int idx = splitedPolyIds[i];
                            visitMarker[ idx ] = false;
                        }
                        splitedPolyIds.clear();
                        break;
                    }
                    else
                    {
                        niSplitEdge splitEdge(topoPolyPtr->m_lTopoPoint, topoPoint2, length);
                        _AddToSplitEdges( splitEdge );

                        splitedPolyIds.push_back(topoPolyPtr->m_IdOfPoly);
                        visitMarker[ topoPolyPtr->m_IdOfPoly ] = true;

                        niTopoPoly *nextTopoPoly = &m_topoPolys[ topoPoint2.m_IdOfPoly ];

                        if (nextTopoPoly->IsSplited() || nextTopoPoly->m_IdOfPoly == 0)
                            break;
                        topoPolyPtr = nextTopoPoly;
                    }
                }
                numSplited = splitedPolyIds.size();
                for (size_t i = 0; i < numSplited; ++i)
                {
                    int idx = splitedPolyIds[i];
                    visitMarker[ idx ] = false;
                }

                //Scan right
                topoPolyPtr = sortedTopoPolyRefs[polyIter];
                while (1)
                {
                    length = 1e300;
                    bFound = _CalcSplitEdges(
                        topoPolyPtr->m_rTopoPoint,
                        topoPoint2,
                        visitMarker,
                        length);
                    if (!bFound)
                    {
                        numSplited = splitedPolyIds.size();
                        for (size_t i = 0; i < numSplited; ++i)
                        {
                            int idx = splitedPolyIds[i];
                            visitMarker[ idx ] = false;
                        }
                        splitedPolyIds.clear();
                        break;
                    }
                    else
                    {
                        niSplitEdge splitEdge(topoPolyPtr->m_rTopoPoint, topoPoint2, length);
                        _AddToSplitEdges( splitEdge );

                        splitedPolyIds.push_back(topoPolyPtr->m_IdOfPoly);
                        visitMarker[ topoPolyPtr->m_IdOfPoly ] = true;

                        niTopoPoly *nextTopoPoly = &m_topoPolys[ topoPoint2.m_IdOfPoly ];

                        if (nextTopoPoly->IsSplited() || nextTopoPoly->m_IdOfPoly == 0)
                            break;
                        topoPolyPtr = nextTopoPoly;
                    }
                }
                numSplited = splitedPolyIds.size();
                for (size_t i = 0; i < numSplited; ++i)
                {
                    int idx = splitedPolyIds[i];

                    m_topoPolys[ idx ].UpdateSplitDirty(true);

                    visitMarker[ idx ] = false;
                }
                splitedPolyIds.clear();
            }
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Find a nearest split edge start from the first topoPoint
        * @param        topoPoint1:     the first topopoint
        * @param        topoPoint2:     the second topopoint need to be found.
        * @param        visitMarker:    used to indicate wether a polygon is visited,
        *                                   if is visited, it will be skiped.
        * @param        length:         store the length of the nearest split edge.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcSplitEdges(
            const niTopoPoint &topoPoint1,
            niTopoPoint &topoPoint2,
            niBoolArray &visitMarker,
            double &length)
        {
            bool bResult = false;
            bool bFound = false;

            double distance1 = niGeomMath2d::DistanceOfPoint2BBox(
                *topoPoint1.m_pointRef,
                m_hBsp4Point->Root()->l_child->m_bbox);
            double distance2 = niGeomMath2d::DistanceOfPoint2BBox(
                *topoPoint1.m_pointRef,
                m_hBsp4Point->Root()->r_child->m_bbox);

            if (distance1 < distance2)
            {
                if (distance1 <= length)
                {
                    bFound = _CalcSplitEdges(
                        m_hBsp4Point->Root()->l_child,
                        topoPoint1,
                        topoPoint2,
                        visitMarker,
                        length);
                    if (bFound)
                        bResult = true;
                }

                if (distance2 <= length)
                {
                    bFound = _CalcSplitEdges(
                        m_hBsp4Point->Root()->r_child,
                        topoPoint1,
                        topoPoint2,
                        visitMarker,
                        length);
                    if (bFound)
                        bResult = true;
                }
            }
            else
            {
                if (distance2 <= length)
                {
                    bFound = _CalcSplitEdges(
                        m_hBsp4Point->Root()->r_child,
                        topoPoint1,
                        topoPoint2,
                        visitMarker,
                        length);
                    if (bFound)
                        bResult = true;
                }

                if (distance1 <= length)
                {
                    bFound = _CalcSplitEdges(
                        m_hBsp4Point->Root()->l_child,
                        topoPoint1,
                        topoPoint2,
                        visitMarker,
                        length);
                    if (bFound)
                        bResult = true;
                }
            }

            return bResult;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Find a nearest split edge start from the first topoPoint
        * @param        polyBspNode:    bsp node
        * @param        topoPoint1:     the first topopoint
        * @param        topoPoint2:     the second topopoint need to be found.
        * @param        visitMarker:    used to indicate wether a polygon is visited,
        *                                   if is visited, it will be skiped.
        * @param        length:         store the length of the nearest split edge.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcSplitEdges(
            const tree::niBspTree::niBspNode* polyBspNode,
            const niTopoPoint &topoPoint1,
            niTopoPoint &topoPoint2,
            niBoolArray &visitMarker,
            double &length)
        {
            bool bResult = false;
            bool bFound = false;

            if (polyBspNode->IsLeaf())
            {
                const tree::niBspTree::niBspLeaf *polyBspLeaf =
                    dynamic_cast<const tree::niBspTree::niBspLeaf*>(polyBspNode);
                bResult = _CalcSplitEdges(
                    polyBspLeaf,
                    topoPoint1,
                    topoPoint2,
                    visitMarker,
                    length);
                return bResult;
            }
            else
            {
                double distance1 = niGeomMath2d::DistanceOfPoint2BBox(
                    *topoPoint1.m_pointRef,
                    polyBspNode->l_child->m_bbox);
                double distance2 = niGeomMath2d::DistanceOfPoint2BBox(
                    *topoPoint1.m_pointRef,
                    polyBspNode->r_child->m_bbox);

                if (distance1 < distance2)
                {
                    if (distance1 <= length)
                    {
                        bFound = _CalcSplitEdges(
                            polyBspNode->l_child,
                            topoPoint1,
                            topoPoint2,
                            visitMarker,
                            length);
                        if (bFound)
                            bResult = true;
                    }

                    if (distance2 <= length)
                    {
                        bFound = _CalcSplitEdges(
                            polyBspNode->r_child,
                            topoPoint1,
                            topoPoint2,
                            visitMarker,
                            length);
                        if (bFound)
                            bResult = true;
                    }
                }
                else
                {
                    if (distance2 <= length)
                    {
                        bFound = _CalcSplitEdges(
                            polyBspNode->r_child,
                            topoPoint1,
                            topoPoint2,
                            visitMarker,
                            length);
                        if (bFound)
                            bResult = true;
                    }

                    if (distance1 <= length)
                    {
                        bFound = _CalcSplitEdges(
                            polyBspNode->l_child,
                            topoPoint1,
                            topoPoint2,
                            visitMarker,
                            length);
                        if (bFound)
                            bResult = true;
                    }
                }
                return bResult;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CalcSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Find a nearest split edge start from the first topoPoint
        * @param        polyBspLeaf:    bsp leaf
        * @param        topoPoint1:     the first topopoint
        * @param        topoPoint2:     the second topopoint need to be found.
        * @param        visitMarker:    used to indicate wether a polygon is visited,
        *                                   if is visited, it will be skiped.
        * @param        length:         store the length of the nearest split edge.
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_CalcSplitEdges(
            const tree::niBspTree::niBspLeaf* polyBspLeaf,
            const niTopoPoint &topoPoint1,
            niTopoPoint &topoPoint2,
            niBoolArray &visitMarker,
            double &length)
        {
            size_t numPoints = polyBspLeaf->m_refPoints.size();
            niArrayT< niPairT<double, int> > distancePairs;
            distancePairs.reserve( numPoints );

            int IdOfPoint1 = topoPoint1.m_IdOfPoint;
            int IdOfPoly1 = topoPoint1.m_IdOfPoly;
            int IdOfPoint2, IdOfPoly2;

            
            niPairT<int, int> splitPair;
            SplitMaps_T::iterator findSplitPair;

            const niPoint2d &p1 = *topoPoint1.m_pointRef;
            double distance;
            for (size_t i = 0; i < numPoints; ++i)
            {
                IdOfPoint2 = polyBspLeaf->m_refPoints[i].m_IdOfPoint;
                IdOfPoly2 = polyBspLeaf->m_refPoints[i].m_IdOfPoly;
                if (IdOfPoly1 == IdOfPoly2)
                    continue;
                if (IdOfPoly2 > 0 && visitMarker[IdOfPoly2])
                    continue;
                if (CDuplicateMarker == m_pointsDirty[ IdOfPoint2 ])
                    continue;

                splitPair.Init(IdOfPoint1, IdOfPoly2);
                findSplitPair = m_splitMap4Point.find(splitPair);
                if (m_splitMap4Point.end() != findSplitPair &&
                    findSplitPair->second > 0)
                {
                    continue;
                }

                splitPair.Init(IdOfPoint2, IdOfPoly1);
                findSplitPair = m_splitMap4Point.find(splitPair);
                if (m_splitMap4Point.end() != findSplitPair &&
                    findSplitPair->second > 0)
                {
                    continue;
                }

                distance = p1.DistanceTo(*polyBspLeaf->m_refPoints[i].m_pointRef);
                if (distance <= length)
                {
                    distancePairs.push_back( niPairT<double, int>(distance, i) );
                }
            }

            std::sort(distancePairs.begin(), distancePairs.end());

            bool bOverlap;
            bool bFound = false;

            size_t numPairs = distancePairs.size();
            for (size_t i = 0; i < numPairs; ++i)
            {
                int yi = distancePairs[i].m_v2;

                const niPoint2d &p2 = *polyBspLeaf->m_refPoints[ yi ].m_pointRef;

                bOverlap = _IsOverlapEdge(p1, p2);
                if (bOverlap)
                {
                    continue;
                }

                topoPoint2 = polyBspLeaf->m_refPoints[ yi ];
                length = distancePairs[i].m_v1;
                bFound = true;
                break;
            }

            return bFound;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CheckAndReverse
        //-----------------------------------------------------------------------------
        /**
        * Detect the orientation of outer and inner polygons, to ensure the outer polygon
        *   is CCW, and the inner polygons is CW.
        * 
        */
        void niDecompose2d::_CheckAndReverse()
        {
            m_bRevered = false;
            niPolygon2dFn fn1(m_polygons[0]);
            if (fn1.Area() < 0.0)
            {
                m_bRevered = true;
                fn1.Reverse();
            }

            size_t numPolygons = m_polygons.size();
            for (size_t i = 1; i < numPolygons; ++i)
            {
                niPolygon2dFn fn2( m_polygons[i] );
                if (fn2.Area() > 0.0)
                {
                    fn2.Reverse();
                }
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _ClearSplitStatus
        //-----------------------------------------------------------------------------
        /**
        * Clear split status
        * 
        */
        void niDecompose2d::_ClearSplitStatus()
        {
            size_t numPolygons = m_topoPolys.size();
            for (size_t i = 0; i < numPolygons; ++i)
            {
                niTopoPoly &topoPoly = m_topoPolys[i];
                topoPoly.UpdateSplitDirty(false);
                topoPoly.m_splitPairs.clear();
            }

            if (NULL != m_hBvh4SplitEdge)
            {
                delete m_hBvh4SplitEdge;
                m_hBvh4SplitEdge = NULL;
            }
            m_numSplitEdgeInBVH = 0;

            m_splitMap4Point.clear();
            m_splitMap4Edge.clear();
            m_splitEdges.clear();
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CreateEdgeBvhTree
        //-----------------------------------------------------------------------------
        /**
        * Create bvh tree for topo edges
        * @param        topoEdges:      topo edges
        * @param        bboxes:         bbox of each topoedge
        * return        bvh tree
        * 
        */
        tree::niBvhTree* niDecompose2d::_CreateEdgeBvhTree(
            niArrayT<niTopoEdge> &topoEdges,
            niArrayT<niBBox2d> &bboxes)
        {
            niArrayT<niRefGeom2d> refEdges;
            size_t numEdges = topoEdges.size();
            try
            {
                refEdges.resize(numEdges);
            }
            catch (std::bad_alloc)
            {
                return NULL;
            }
            for (size_t i = 0; i < numEdges; ++i)
            {
                refEdges[i].m_geomRef   = (void*)(&topoEdges[i]);
                refEdges[i].m_IdOfGeom  = i;
            }
            return new tree::niBvhTree(refEdges, bboxes);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _CreateEdgeBvhTree
        //-----------------------------------------------------------------------------
        /**
        * Create bvh tree for split edges
        * @param        splitEdges:     split edge
        * return        bvh tree
        * 
        */
        tree::niBvhTree* niDecompose2d::_CreateEdgeBvhTree(
            niArrayT<niSplitEdge> &splitEdges)
        {
            niArrayT<niRefGeom2d> refEdges;
            niArrayT<niBBox2d> bboxes;
            size_t numEdges = splitEdges.size();
            try
            {
                refEdges.resize(numEdges);
                bboxes.resize(numEdges);
            }
            catch (std::bad_alloc)
            {
                return NULL;
            }
            for (size_t i = 0; i < numEdges; ++i)
            {
                bboxes[i].Append( splitEdges[i].A().m_pointRef );
                bboxes[i].Append( splitEdges[i].B().m_pointRef );

                refEdges[i].m_geomRef = (void*)(&splitEdges[i]);
                refEdges[i].m_IdOfGeom = i;
            }
            return new tree::niBvhTree(refEdges, bboxes);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _Destory
        //-----------------------------------------------------------------------------
        /**
        * Free memory
        * 
        */
        void niDecompose2d::_Destory()
        {
            if (NULL != m_hBvh4TopoEdge)
            {
                delete m_hBvh4TopoEdge;
                m_hBvh4TopoEdge = NULL;
            }
            if (NULL != m_hBvh4SplitEdge)
            {
                delete m_hBvh4SplitEdge;
                m_hBvh4SplitEdge = NULL;
                m_numSplitEdgeInBVH = 0;
            }
            if (NULL != m_hBsp4Point)
            {
                delete m_hBsp4Point;
                m_hBsp4Point = NULL;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _FindNearstEdge
        //-----------------------------------------------------------------------------
        /**
        * Find nearest split edge
        * @param        polyBspNode1:   bsp node 1
        * @param        polyBspNode2:   bsp node 2
        * @param        topoPoint1:     store topo point 1 of nearest split edge
        * @param        topoPoint2:     store topo point 2 of nearest split edge
        * @param        length:         store the length of nearest split edge
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_FindNearstEdge(
            const tree::niBspTree::niBspNode* polyBspNode1,
            const tree::niBspTree::niBspNode* polyBspNode2,
            niTopoPoint &topoPoint1,
            niTopoPoint &topoPoint2,
            double &length)
        {
            bool bResult = false;
            bool bFound = false;

            double distance;

            bool isLeaf1 = polyBspNode1->IsLeaf();
            bool isLeaf2 = polyBspNode2->IsLeaf();
            if (isLeaf1 && isLeaf2)
            {
                distance = niGeomMath2d::DistanceOfBBox2BBox(
                    polyBspNode1->m_bbox, polyBspNode2->m_bbox);
                if (distance > length)
                    return false;

                const tree::niBspTree::niBspLeaf *polyBspLeaf1 =
                    dynamic_cast<const tree::niBspTree::niBspLeaf*>(polyBspNode1);
                const tree::niBspTree::niBspLeaf *polyBspLeaf2 =
                    dynamic_cast<const tree::niBspTree::niBspLeaf*>(polyBspNode2);
                bFound = _FindNearstEdge(
                    polyBspLeaf1,
                    polyBspLeaf2,
                            topoPoint1,
                            topoPoint2,
                            length);
                if (bFound)
                    bResult = true;
            }
            else
            {
                int N = 0;
                niTripleT<double, const tree::niBspTree::niBspNode*, const tree::niBspTree::niBspNode*> triples[4];
                if (isLeaf1)
                {
                    N = 2;
                    triples[0].m_v2 = polyBspNode1;
                    triples[0].m_v3 = polyBspNode2->l_child;
                    triples[1].m_v2 = polyBspNode1;
                    triples[1].m_v3 = polyBspNode2->r_child;
                }
                else if (isLeaf2)
                {
                    N = 2;
                    triples[0].m_v2 = polyBspNode1->l_child;
                    triples[0].m_v3 = polyBspNode2;
                    triples[1].m_v2 = polyBspNode1->r_child;
                    triples[1].m_v3 = polyBspNode2;
                }
                else
                {
                    N = 4;
                    triples[0].m_v2 = polyBspNode1->l_child;
                    triples[0].m_v3 = polyBspNode2->l_child;
                    triples[1].m_v2 = polyBspNode1->l_child;
                    triples[1].m_v3 = polyBspNode2->r_child;
                    triples[2].m_v2 = polyBspNode1->r_child;
                    triples[2].m_v3 = polyBspNode2->l_child;
                    triples[3].m_v2 = polyBspNode1->r_child;
                    triples[3].m_v3 = polyBspNode2->r_child;
                    }

                for (int i = 0; i < N; ++i)
                {
                    triples[i].m_v1 = niGeomMath2d::DistanceOfBBox2BBox(
                        triples[i].m_v2->m_bbox, triples[i].m_v3->m_bbox);
                }

                //Bubble sort
                algorithm::niBubbleSort< niTripleT<double, const tree::niBspTree::niBspNode*, const tree::niBspTree::niBspNode*> >::Sort(triples, N);

                for (int i = 0; i < N; ++i)
                {
                    if (triples[i].m_v1 > length)
                    {
                        break;
                    }

                    bFound = _FindNearstEdge(
                        triples[i].m_v2,
                        triples[i].m_v3,
                        topoPoint1,
                        topoPoint2,
                        length);
                    if (bFound)
                    {
                        bResult = true;
                    }
                }
            }

            return bResult;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _FindNearstEdge
        //-----------------------------------------------------------------------------
        /**
        * Find nearest split edge
        * @param        polyBspNode1:   bsp leaf 1
        * @param        polyBspNode2:   bsp leaf 2
        * @param        topoPoint1:     store topo point 1 of nearest split edge
        * @param        topoPoint2:     store topo point 2 of nearest split edge
        * @param        length:         store the length of nearest split edge
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_FindNearstEdge(
            const tree::niBspTree::niBspLeaf* polyBspLeaf1,
            const tree::niBspTree::niBspLeaf* polyBspLeaf2,
            niTopoPoint &topoPoint1,
            niTopoPoint &topoPoint2,
            double &length)
        {
            size_t numPoint1 = polyBspLeaf1->m_refPoints.size();
            size_t numPoint2 = polyBspLeaf2->m_refPoints.size();
            niArrayT< niTripleT<double, int, int> >distanceTriples;
            distanceTriples.reserve(numPoint1 * numPoint2);

            niPairT<int, int> splitPair;
            SplitMaps_T::iterator findSplitPair;

            size_t xi, yi;
            int IdOfPoint1, IdOfPoint2, IdOfPoly1, IdOfPoly2;
            double distance;
            for (xi = 0; xi < numPoint1; ++xi)
            {
                IdOfPoint1 = polyBspLeaf1->m_refPoints[xi].m_IdOfPoint;
                if (CDuplicateMarker == m_pointsDirty[ IdOfPoint1 ])
                    continue;

                IdOfPoly1 = polyBspLeaf1->m_refPoints[xi].m_IdOfPoly;

                //niTopoPoly &topPoly1 = m_topoPolys[IdOfPoly1];

                //if (topPoly1.IsPointExist(IdOfPoint1))
                  //  continue;

                const niPoint2d &pnt = *polyBspLeaf1->m_refPoints[xi].m_pointRef;
                for (yi = 0; yi < numPoint2; ++yi)
                {
                    IdOfPoint2 = polyBspLeaf2->m_refPoints[yi].m_IdOfPoint;
                    if (CDuplicateMarker == m_pointsDirty[ IdOfPoint2 ])
                    {
                        continue;
                    }

                    IdOfPoly2 = polyBspLeaf2->m_refPoints[yi].m_IdOfPoly;
                    if (IdOfPoly1 == IdOfPoly2)
                    {
                        continue;
                    }

                    splitPair.Init(IdOfPoint1, IdOfPoly2);
                    findSplitPair = m_splitMap4Point.find(splitPair);
                    if (m_splitMap4Point.end() != findSplitPair &&
                        findSplitPair->second > 0)
                    {
                        continue;
                    }

                    splitPair.Init(IdOfPoint2, IdOfPoly1);
                    findSplitPair = m_splitMap4Point.find(splitPair);
                    if (m_splitMap4Point.end() != findSplitPair &&
                        findSplitPair->second > 0)
                    {
                        continue;
                    }

                    //niTopoPoly &topPoly2 = m_topoPolys[IdOfPoly2];

                    //if (topPoly2.IsPointExist(IdOfPoint2))
                      //  continue;

                    distance = pnt.DistanceTo(*polyBspLeaf2->m_refPoints[yi].m_pointRef);
                    if (distance <= length)
                    {
                        distanceTriples.push_back( niTripleT<double, int, int>(distance, xi, yi) );
                    }
                }
            }

            std::sort(distanceTriples.begin(), distanceTriples.end());

            bool bOverlap, bCloseToOtherpolygon;
            bool bFound = false;

            size_t numTriples = distanceTriples.size();
            for (size_t i = 0; i < numTriples; ++i)
            {
                xi = distanceTriples[i].m_v2;
                yi = distanceTriples[i].m_v3;
                const niPoint2d &p1 = *polyBspLeaf1->m_refPoints[xi].m_pointRef;
                const niPoint2d &p2 = *polyBspLeaf2->m_refPoints[yi].m_pointRef;

                bOverlap = _IsOverlapEdge(p1, p2);
                if (bOverlap)
                {
                    continue;
                }

                if (m_coordShift > ZEROF)
                {
                    bCloseToOtherpolygon = _IsColseToOtherPolygon(
                        p1,
                        p2,
                        polyBspLeaf1->m_refPoints[xi].m_IdOfPoly,
                        polyBspLeaf2->m_refPoints[yi].m_IdOfPoly,
                        m_coordShift);
                    if (bCloseToOtherpolygon)
                    {
                        continue;
                    }
                }

                length = distanceTriples[i].m_v1;
                topoPoint1 = polyBspLeaf1->m_refPoints[xi];
                topoPoint2 = polyBspLeaf2->m_refPoints[yi];

                bFound = true;
                break;
            }

            return bFound;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _Init
        //-----------------------------------------------------------------------------
        /**
        * Init topo verts / edges / polygons
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_Init()
        {
            _Destory();

            _CalcOuterBBox();

            //Add up all point number
            size_t numOfAllPoints = 0;
            size_t numPolygons = m_polygons.size();
            for (size_t i = 0; i < numPolygons; ++i)
            {
                numOfAllPoints += m_polygons[i].NumPoints();
            }

            //Allocate memory;
            try
            {
                m_origalPoints.reserve(numOfAllPoints);
                m_fixedPoints.reserve(numOfAllPoints);
                m_topoPoints.resize(numOfAllPoints);
                m_pointsDirty.resize(numOfAllPoints, 0);
                m_edgeBBoxes.resize(numOfAllPoints);
                m_topoEdges.reserve(numOfAllPoints);
                m_topoEdges.resize(numOfAllPoints);
                m_topoPolys.resize(numPolygons);
                m_polyBBoxes.resize(numPolygons);
                //Get a large memory, to avoid resize operation,
                // because it will cause m_hBvh4SplitEdge error
                m_splitEdges.reserve(numPolygons * 16);
            }
            catch (std::bad_alloc)
            {
                _Destory();
                return false;
            }

            bool bStat = _InitTopoPolys();
            if (!bStat)
            {
                return false;
            }



            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _InitDelaunaySplit
        //-----------------------------------------------------------------------------
        /**
        * Init delaunay connection of inner polygons
        * @param        splitTriples:   store length and point start/end ids of delaunay
        *                               trimesh edges.
        * @param        convexHullIds:  store reference point ids of convex hull
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_InitDelaunaySplit(
            niArrayT< niTripleT<double, int, int> > &splitTriples,
            niIntArray &convexHullIds)
        {
            splitTriples.clear();
            niTripleT<double, int, int> delaunaySplit;

            size_t numInners = m_topoPolys.size() - 1;
            niPoint2dArray innerCenters;
            innerCenters.resize(numInners);
            for (size_t i = 0; i < numInners; ++i)
            {
                innerCenters[i] = m_topoPolys[i+1].m_center;
            }

            if (numInners < 2)
            {
                for (size_t i = 0; i < numInners; ++i)
                {
                    convexHullIds.push_back(i+1);
                }
                return true;
            }
            else
            {
                ni::geometry::niTriMesh2d trimesh;
                ni::geometry::niTriMesh2dFn fn(trimesh);

                bool bStat = fn.BuildFromPoints(innerCenters, convexHullIds);
                if (bStat)
                {
                    ni::geometry::niTriMesh2dTopo topoMesh2d;
                    bStat = topoMesh2d.BuildTopology(trimesh, false);
                    if (bStat)
                    {
                        fn.OptimizeTriMesh(topoMesh2d);
                        const niEdge2vfArray &v2efs = topoMesh2d.GetEdge2VF();
                        size_t numSplits = v2efs.size();
                        splitTriples.resize( numSplits );
                        for (size_t i = 0; i < numSplits; ++i)
                        {
                            const niEdge2VF &v2ef = v2efs[i];
                            splitTriples[i].m_v1 = innerCenters[ v2ef.m_e2v[0] ].DistanceTo( innerCenters[ v2ef.m_e2v[1] ] );
                            splitTriples[i].m_v2 = v2ef.m_e2v[0] + 1;
                            splitTriples[i].m_v3 = v2ef.m_e2v[1] + 1;
                        }

                        std::sort(splitTriples.begin(), splitTriples.end());

                        size_t numCHs = convexHullIds.size();
                        for (size_t i = 0; i < numCHs; ++i)
                        {
                            ++convexHullIds[i];
                        }
                    }
                }
                return bStat;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _InitTopoPoly
        //-----------------------------------------------------------------------------
        /**
        * Init topo polygon
        * @param        poly:           polygon
        * @param        topoPoly:       topo polygon
        * @param        IdOfPoly:       id of polygon
        * @param        iGlobalPntId:   global point id
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_InitTopoPoly(
            niPolygon2d &poly,
            niTopoPoly &topoPoly,
            int IdOfPoly,
            int &iGlobalPntId)
        {
            const int iBasePntId = iGlobalPntId;

            int NUM = poly.NumPoints();
            niPoint2dArray fixedPoints = poly.GetPoints();
            m_origalPoints.insert(m_origalPoints.end(), fixedPoints.begin(), fixedPoints.end());

            niPoint2d boxCenter = (m_originalBBox.P1 + m_originalBBox.P2) * 0.5;

            niGeomMath2d::AvoidAbnormalData(COORDSCALE_NDH2NDS, boxCenter, fixedPoints);

            m_fixedPoints.insert(m_fixedPoints.end(), fixedPoints.begin(), fixedPoints.end());

            niGeomMath2d::Center(fixedPoints, topoPoly.m_center);

            double area = niGeomMath2d::Area(fixedPoints);
            m_inputArea += area;

            topoPoly.m_topoPoints.resize(NUM);
            topoPoly.m_topoEdges.resize(NUM);
            topoPoly.m_IdOfPoly     = IdOfPoly;
            topoPoly.m_IdOfGroup    = IdOfPoly;
            topoPoly.m_area         = -area;

            niArrayT<const niBBox2d*> bboxRefs;
            bboxRefs.resize(NUM);

            niArrayT<niRefGeom2d> refGeomes;
            refGeomes.resize(NUM);

            std::map<niPoint2d, int> pointMap;

            for (int n = 0; n < NUM; ++n)
            {
                topoPoly.m_bbox.Append( fixedPoints[n] );
                niTopoPoint &topoPoint      = m_topoPoints[ iGlobalPntId ];
                topoPoint.m_pointRef        = &m_fixedPoints[ iGlobalPntId ];
                topoPoint.m_IdOfPoint       = iGlobalPntId;
                topoPoint.m_IdOfPoly        = IdOfPoly;
                topoPoly.m_topoPoints[n]    = topoPoint;
                std::map<niPoint2d, int>::iterator findPoint = pointMap.find( fixedPoints[n] );
                if (findPoint != pointMap.end())
                {
                    m_pointsDirty[ findPoint->second ] = CDuplicateMarker;
                    m_pointsDirty[ iGlobalPntId ] = CDuplicateMarker;
                }
                else
                {
                    pointMap.insert( std::make_pair( fixedPoints[n], iGlobalPntId ) );
                }
                ++iGlobalPntId;
            }

            iGlobalPntId = iBasePntId;
            for (int n = 0; n < NUM-1; ++n)
            {
                m_topoEdges[iGlobalPntId].Init(
                    m_topoPoints[ iGlobalPntId ],
                    m_topoPoints[ iGlobalPntId+1 ]);

                topoPoly.m_topoEdges[n]         = m_topoEdges[iGlobalPntId];

                niBBox2d &bbox = m_edgeBBoxes[ iGlobalPntId ];
                bbox.Append(m_topoPoints[ iGlobalPntId ].m_pointRef);
                bbox.Append(m_topoPoints[ iGlobalPntId+1 ].m_pointRef);
                bboxRefs[n]                     = &bbox;

                refGeomes[n].m_geomRef          = (void*)&m_topoEdges[iGlobalPntId];
                refGeomes[n].m_IdOfGeom         = iGlobalPntId;
                ++iGlobalPntId;
            }

            m_topoEdges[iGlobalPntId].Init(
                m_topoPoints[ iGlobalPntId ],
                m_topoPoints[ iBasePntId ]);
            topoPoly.m_topoEdges[NUM-1]         = m_topoEdges[iGlobalPntId];

            niBBox2d &bbox = m_edgeBBoxes[ iGlobalPntId ];
            bbox.Append(m_topoPoints[ iGlobalPntId ].m_pointRef);
            bbox.Append(m_topoPoints[ iBasePntId ].m_pointRef);
            bboxRefs[NUM-1]                     = &bbox;
            refGeomes[NUM-1].m_geomRef          = (void*)&m_topoEdges[iGlobalPntId];
            refGeomes[NUM-1].m_IdOfGeom         = iGlobalPntId;
            ++iGlobalPntId;

            topoPoly.m_hBsp4Point               = new tree::niBspTree(topoPoly.m_topoPoints);

            topoPoly.m_hBvh4Edge                = new tree::niBvhTree(refGeomes, bboxRefs);

            for (int i = 0; i < NUM; ++i)
            {
                niTopoPoint &topoPoint = topoPoly.m_topoPoints[i];
                if (CDuplicateMarker == m_pointsDirty[ topoPoint.m_IdOfPoint ])
                    continue;
                if (topoPoly.m_lTopoPoint.m_pointRef == NULL)
                {
                    topoPoly.m_lTopoPoint = topoPoint;
                }
                else
                {
                    if (*topoPoint.m_pointRef < *topoPoly.m_lTopoPoint.m_pointRef)
                        topoPoly.m_lTopoPoint = topoPoint;
                }
                if (topoPoly.m_rTopoPoint.m_pointRef == NULL)
                {
                    topoPoly.m_rTopoPoint = topoPoint;
                }
                else
                {
                    if (*topoPoly.m_rTopoPoint.m_pointRef < *topoPoint.m_pointRef)
                        topoPoly.m_rTopoPoint = topoPoint;
                }
            }

            return (NULL != topoPoly.m_hBsp4Point) && (NULL != topoPoly.m_hBvh4Edge) && (iGlobalPntId == iBasePntId+NUM);
        }


        //-----------------------------------------------------------------------------
        // FUNCTION _InitTopoPolys
        //-----------------------------------------------------------------------------
        /**
        * Init delaunay connection of inner polygons
        * @param        poly:           polygon
        * @param        topoPoly:       topo polygon
        * @param        IdOfPoly:       id of polygon
        * @param        iGlobalPntId:   global point id
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_InitTopoPolys()
        {
            int iGlobalPntId = 0;
            bool bStat;

            m_inputArea = 0.0;

            size_t numPolygons = m_polygons.size();

            for (size_t IdOfPoly = 0; IdOfPoly < numPolygons; ++IdOfPoly)
            {
                niPolygon2d &poly       = m_polygons[IdOfPoly];
                niTopoPoly &topoPoly    = m_topoPolys[IdOfPoly];

                bStat = _InitTopoPoly(poly, topoPoly, IdOfPoly, iGlobalPntId);
                if (!bStat)
                {
                    return false;
                }

                m_polyBBoxes[IdOfPoly]         = topoPoly.m_bbox;
            }

            m_hBvh4TopoEdge = _CreateEdgeBvhTree(m_topoEdges, m_edgeBBoxes);

            m_hBsp4Point = new tree::niBspTree(m_topoPoints);
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsColseToOtherPolygon
        //-----------------------------------------------------------------------------
        /**
        * Check the distance of split edge to other polygon
        * @param        p0:             first point
        * @param        p1:             second point
        * @param        IdOfPoly1:      polygon id of the first point
        * @param        IdOfPoly2:      polygon id of the second point
        * @param        threshold:      threshold of distance
        * return        true:           if the distance >= threshold
        *               false:          if the distance < threshold
        * 
        */
        bool niDecompose2d::_IsColseToOtherPolygon(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const int IdOfPoly1,
            const int IdOfPoly2,
            const double threshold)
        {
            return _IsColseToOtherPolygon(
                m_hBsp4Point->Root(),
                p0,
                p1,
                IdOfPoly1,
                IdOfPoly2,
                threshold);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsColseToOtherPolygon
        //-----------------------------------------------------------------------------
        /**
        * Check the distance of split edge to other polygon
        * @param        polyBspNode:    bsp node
        * @param        p0:             first point
        * @param        p1:             second point
        * @param        IdOfPoly1:      polygon id of the first point
        * @param        IdOfPoly2:      polygon id of the second point
        * @param        threshold:      threshold of distance
        * return        true:           if the distance >= threshold
        *               false:          if the distance < threshold
        * 
        */
        bool niDecompose2d::_IsColseToOtherPolygon(
            const tree::niBspTree::niBspNode* polyBspNode,
            const niPoint2d &p0,
            const niPoint2d &p1,
            const int IdOfPoly1,
            const int IdOfPoly2,
            const double threshold)
        {
            niBBox2d bbox = polyBspNode->m_bbox;
            bbox.Extent(threshold, threshold);

            bool bOverlap = niGeomMath2d::IsLineOverlapBox(p0, p1, bbox);
            if (!bOverlap)
                return false;

            if (polyBspNode->IsLeaf())
            {
                const tree::niBspTree::niBspLeaf *polyBspLeaf =
                    dynamic_cast<const tree::niBspTree::niBspLeaf*>(polyBspNode);
                return _IsColseToOtherPolygon(
                    polyBspLeaf,
                    p0,
                    p1,
                    IdOfPoly1,
                    IdOfPoly2,
                    threshold);
            }
            else
            {
                bool bClose = _IsColseToOtherPolygon(
                    polyBspNode->l_child,
                    p0,
                    p1,
                    IdOfPoly1,
                    IdOfPoly2,
                    threshold);
                if (bClose)
                {
                    return true;
                }
                
                return _IsColseToOtherPolygon(
                    polyBspNode->r_child,
                    p0,
                    p1,
                    IdOfPoly1,
                    IdOfPoly2,
                    threshold);
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsColseToOtherPolygon
        //-----------------------------------------------------------------------------
        /**
        * Check the distance of split edge to other polygon
        * @param        polyBspNode:    bsp leaf
        * @param        p0:             first point
        * @param        p1:             second point
        * @param        IdOfPoly1:      polygon id of the first point
        * @param        IdOfPoly2:      polygon id of the second point
        * @param        threshold:      threshold of distance
        * return        true:           if the distance >= threshold
        *               false:          if the distance < threshold
        * 
        */
        bool niDecompose2d::_IsColseToOtherPolygon(
            const tree::niBspTree::niBspLeaf* polyBspLeaf,
            const niPoint2d &p0,
            const niPoint2d &p1,
            const int IdOfPoly1,
            const int IdOfPoly2,
            const double threshold)
        {
            double height = 0.0;
            size_t numPoints = polyBspLeaf->m_refPoints.size();
            for (size_t i = 0; i < numPoints; ++i)
            {
                const niTopoPoint &topoPoint = polyBspLeaf->m_refPoints[i];
                if (topoPoint.m_IdOfPoly == IdOfPoly1 ||
                    topoPoint.m_IdOfPoly == IdOfPoly2)
                {
                    continue;
                }

                height = niGeomMath2d::HeightOfPoint2Line(
                    *topoPoint.m_pointRef, p0, p1);

                if (height < threshold)
                {
                    return true;
                }
            }
            return false;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsOverlapEdge
        //-----------------------------------------------------------------------------
        /**
        * Check a split edge is overlap other topo edges / split edges
        * @param        p0:             first point
        * @param        p1:             second point
        * return        true:           Overlap
        *               false:          not overlap
        * 
        */
        bool niDecompose2d::_IsOverlapEdge(
            const niPoint2d &p0,
            const niPoint2d &p1)
        {
            bool bOverlap = _IsOverlapTopoEdge(p0, p1);
            if (bOverlap)
            {
                return bOverlap;
            }

            return _IsOverlapSplitEdge(p0, p1);
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsOverlapTopoEdge
        //-----------------------------------------------------------------------------
        /**
        * Check a split edge is overlap other topo edges
        * @param        p0:             first point
        * @param        p1:             second point
        * return        true:           Overlap
        *               false:          not overlap
        * 
        */
        bool niDecompose2d::_IsOverlapTopoEdge(
            const niPoint2d &p0,
            const niPoint2d &p1)
        {
            return _IsOverlapTopoEdge(p0, p1, m_hBvh4TopoEdge->Root());
        }

        bool niDecompose2d::_IsOverlapTopoEdge(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const tree::niBvhTree::niBvhNode *edgeBvhNode)
        {
            bool bOverlap = niGeomMath2d::IsLineOverlapBox(p0, p1, edgeBvhNode->m_bbox);
            if (!bOverlap)
                return false;

            if (edgeBvhNode->IsLeaf())
            {
                const tree::niBvhTree::niBvhLeaf *leaf = 
                    dynamic_cast<const tree::niBvhTree::niBvhLeaf*>(edgeBvhNode);
                const niTopoEdge* te = static_cast<const niTopoEdge*>(leaf->m_refGeom.m_geomRef);
                bOverlap = niGeomMath2d::LineOverlapLine(
                    p0, p1, *te->A().m_pointRef, *te->B().m_pointRef);

                return bOverlap;
            }
            else
            {
                if (edgeBvhNode->l_child)
                {
                    bOverlap = _IsOverlapTopoEdge(p0, p1, edgeBvhNode->l_child);
                    if (bOverlap)
                    {
                        return true;
                    }
                }
                if (edgeBvhNode->r_child)
                {
                    bOverlap = _IsOverlapTopoEdge(p0, p1, edgeBvhNode->r_child);
                    if (bOverlap)
                    {
                        return true;
                    }
                }
                return false;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsOverlapSplitEdge
        //-----------------------------------------------------------------------------
        /**
        * Check a split edge is overlap other split edges
        * @param        p0:             first point
        * @param        p1:             second point
        * return        true:           Overlap
        *               false:          not overlap
        * 
        */
        bool niDecompose2d::_IsOverlapSplitEdge(
            const niPoint2d &p0,
            const niPoint2d &p1)
        {
            size_t numSplitEdges = m_splitEdges.size();
            bool bOverlap = false;

            if (numSplitEdges - m_numSplitEdgeInBVH > 128)
            {
                if (NULL != m_hBvh4SplitEdge)
                {
                    delete m_hBvh4SplitEdge;
                    m_hBvh4SplitEdge = NULL;
                    m_numSplitEdgeInBVH = 0;
                }
                m_hBvh4SplitEdge = _CreateEdgeBvhTree(m_splitEdges);
                m_numSplitEdgeInBVH = numSplitEdges;
                return _IsOverlapSplitEdge(
                    p0,
                    p1,
                    m_hBvh4SplitEdge->Root());
            }

            if (NULL != m_hBvh4SplitEdge)
            {
                bOverlap = _IsOverlapSplitEdge(
                    p0,
                    p1,
                    m_hBvh4SplitEdge->Root());
                if (bOverlap)
                {
                    return bOverlap;
                }
            }

            for (size_t i = m_numSplitEdgeInBVH; i < numSplitEdges; ++i)
            {
                niSplitEdge &splitEdge = m_splitEdges[i];
                if (splitEdge.IsExcluded())
                {
                    continue;
                }

                bOverlap = niGeomMath2d::LineOverlapLine(
                    p0,
                    p1,
                    *splitEdge.A().m_pointRef,
                    *splitEdge.B().m_pointRef);
                if (bOverlap)
                {
                    return bOverlap;
                }
            }
            return bOverlap;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _IsOverlapSplitEdge
        //-----------------------------------------------------------------------------
        /**
        * Check a split edge is overlap other split edges
        * @param        p0:             first point
        * @param        p1:             second point
        * @param        edgeBvhNode:    bvh node
        * return        true:           Overlap
        *               false:          not overlap
        * 
        */
        bool niDecompose2d::_IsOverlapSplitEdge(
            const niPoint2d &p0,
            const niPoint2d &p1,
            const tree::niBvhTree::niBvhNode *edgeBvhNode)
        {
            bool bOverlap = niGeomMath2d::IsLineOverlapBox(p0, p1, edgeBvhNode->m_bbox);
            if (!bOverlap)
                return false;

            if (edgeBvhNode->IsLeaf())
            {
                const tree::niBvhTree::niBvhLeaf *leaf = 
                    dynamic_cast<const tree::niBvhTree::niBvhLeaf*>(edgeBvhNode);
                const niSplitEdge* se = static_cast<const niSplitEdge*>(leaf->m_refGeom.m_geomRef);
                if (se->IsExcluded())
                {
                    return false;
                }

                bOverlap = niGeomMath2d::LineOverlapLine(
                    p0, p1, *se->A().m_pointRef, *se->B().m_pointRef);
                return bOverlap;
            }
            else
            {
                if (edgeBvhNode->l_child)
                {
                    bOverlap = _IsOverlapSplitEdge(p0, p1, edgeBvhNode->l_child);
                    if (bOverlap)
                        return true;
                }
                if (edgeBvhNode->r_child)
                {
                    bOverlap = _IsOverlapSplitEdge(p0, p1, edgeBvhNode->r_child);
                    if (bOverlap)
                        return true;
                }
                return false;
            }
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _RefineSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * Refine split edge, to avoid coordshift error
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_RefineSplitEdges()
        {
            bool bStat = false;
            int IdOfPoint1, IdOfPoint2, IdOfPoly1, IdOfPoly2;

            niPairT<int, int> splitPair1, splitPair2;
            SplitMaps_T::iterator findSplitPair1, findSplitPair2;

            niTopoPoint topoPoint1, topoPoint2;
            size_t numSplitEdges = m_splitEdges.size();
            for (size_t i = 0; i < numSplitEdges; ++i)
            {
                niSplitEdge &splitEdge = m_splitEdges[i];
                if (splitEdge.IsExcluded())
                    continue;

                if (splitEdge.IsPin())
                    continue;
                
                IdOfPoint1 = splitEdge.A().m_IdOfPoint;
                IdOfPoint2 = splitEdge.B().m_IdOfPoint;
                IdOfPoly1 = splitEdge.A().m_IdOfPoly;
                IdOfPoly2 = splitEdge.B().m_IdOfPoly;

                splitPair1.Init(IdOfPoint1, IdOfPoly2);
                findSplitPair1 = m_splitMap4Point.find( splitPair1 );
                --findSplitPair1->second;

                splitPair2.Init(IdOfPoint2, IdOfPoly1);
                findSplitPair2 = m_splitMap4Point.find( splitPair2 );
                --findSplitPair2->second;

                splitEdge.Disable();

                double length = splitEdge.Length();
                bStat = _FindNearstEdge(
                    m_topoPolys[IdOfPoly1].m_hBsp4Point->Root(),
                    m_topoPolys[IdOfPoly2].m_hBsp4Point->Root(),
                    topoPoint1,
                    topoPoint2,
                    length);
                if (bStat)
                {
                    m_topoPolys[IdOfPoly1].RemoveSplitPair(splitPair1);
                    m_topoPolys[IdOfPoly2].RemoveSplitPair(splitPair2);

                    niSplitEdge newSplitEdge(topoPoint1, topoPoint2, length);
                    _AddToSplitEdges( newSplitEdge );
                    m_topoPolys[IdOfPoly1].UpdateSplitDirty(true);
                    m_topoPolys[IdOfPoly2].UpdateSplitDirty(true);
                }
                else
                {
                    ++findSplitPair1->second;
                    ++findSplitPair2->second;
                    splitEdge.Enable();
                }
            }

            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _RemoveDuplicateSplitEdges
        //-----------------------------------------------------------------------------
        /**
        * remove duplicate split edge
        * return        true:           success
        *               false:          failed
        * 
        */
        bool niDecompose2d::_RemoveDuplicateSplitEdges()
        {
            niArrayT<niSplitEdge> splitEdges = m_splitEdges;

            _ClearSplitStatus();

            std::sort(splitEdges.begin(), splitEdges.end());

            int IdOfPoly1, IdOfPoly2;
            size_t numSplitEdges = splitEdges.size();

            //If more than one split edges start from one point to the same one polygon ,
            // then only the shortest split edge  is kept, the others are skipped.
            /*
            for (size_t i = 0; i < numSplitEdges; ++i)
            {
                niSplitEdge &splitEdge = splitEdges[i];
                if (splitEdge.IsExcluded())
                    continue;

                IdOfPoly1 = splitEdge.A().m_IdOfPoly;
                IdOfPoly2 = splitEdge.B().m_IdOfPoly;

                niPairT<int, int> splitPair1(splitEdge.A().m_IdOfPoint, IdOfPoly2);
                niPairT<int, int> splitPair2(splitEdge.B().m_IdOfPoint, IdOfPoly1);
                SplitMaps_T::iterator findSplitPair1, findSplitPair2;

                findSplitPair1 = m_splitMap4Point.find(splitPair1);
                findSplitPair2 = m_splitMap4Point.find(splitPair2);

                if (m_splitMap4Point.end() == findSplitPair1 &&
                    m_splitMap4Point.end() == findSplitPair2)
                {
                    m_splitMap4Point.insert( std::make_pair(splitPair1, 1) );
                    m_splitMap4Point.insert( std::make_pair(splitPair2, 1) );
                }
                else
                {
                    splitEdge.Disable();
                }
            }
            //*/

            //First keep the shortest split edge between the two polygons.
            numSplitEdges = splitEdges.size();
            for (size_t i = 0; i < numSplitEdges; ++i)
            {
                niSplitEdge &splitEdge = splitEdges[i];
                if (splitEdge.IsExcluded())
                    continue;
                if (splitEdge.IsPin())
                {
                    IdOfPoly1 = splitEdge.A().m_IdOfPoly;
                    IdOfPoly2 = splitEdge.B().m_IdOfPoly;
                    niPairT<int, int> splitPair;
                    if (IdOfPoly1 < IdOfPoly2)
                        splitPair.Init(IdOfPoly1, IdOfPoly2);
                    else
                        splitPair.Init(IdOfPoly2, IdOfPoly1);
                    SplitMaps_T::iterator findSplitPair = m_splitMap4Edge.find(splitPair);
                    if (findSplitPair == m_splitMap4Edge.end())
                    {
                        m_splitMap4Edge.insert(std::make_pair(splitPair, 1));
                    }
                    else
                    {
                        ++findSplitPair->second;
                    }

                    _AddToSplitEdges( splitEdge );
                    splitEdge.Disable();
                    continue;
                }

                IdOfPoly1 = splitEdge.A().m_IdOfPoly;
                IdOfPoly2 = splitEdge.B().m_IdOfPoly;
                niPairT<int, int> splitPair;
                if (IdOfPoly1 < IdOfPoly2)
                    splitPair.Init(IdOfPoly1, IdOfPoly2);
                else
                    splitPair.Init(IdOfPoly2, IdOfPoly1);

                SplitMaps_T::iterator findSplitPair = m_splitMap4Edge.find(splitPair);
                if (findSplitPair == m_splitMap4Edge.end())
                {
                    m_splitMap4Edge.insert(std::make_pair(splitPair, 1));
                    _AddToSplitEdges( splitEdge );
                    splitEdge.Disable();
                }
            }

            //Second, keep the second split edge between the two polygons.
            for (size_t i = 0; i < numSplitEdges; ++i)
            {
                niSplitEdge &splitEdge = splitEdges[i];
                if (splitEdge.IsExcluded())
                    continue;

                IdOfPoly1 = splitEdge.A().m_IdOfPoly;
                IdOfPoly2 = splitEdge.B().m_IdOfPoly;

                niPairT<int, int> splitPair;
                if (IdOfPoly1 < IdOfPoly2)
                    splitPair.Init(IdOfPoly1, IdOfPoly2);
                else
                    splitPair.Init(IdOfPoly2, IdOfPoly1);

                SplitMaps_T::iterator findSplitPair = m_splitMap4Edge.find(splitPair);
                if (findSplitPair != m_splitMap4Edge.end())
                {
                    if (findSplitPair->second == 1)
                    {
                        _AddToSplitEdges( splitEdge );
                    }
                    splitEdge.Disable();
                    ++findSplitPair->second;
                }
            }
            return true;
        }

        //-----------------------------------------------------------------------------
        // FUNCTION _TestByArea
        //-----------------------------------------------------------------------------
        /**
        * Test decompose result by area, the area of input mutiply connected polygon should 
        *   be equal to the one of all decomposed simply polygons.
        * @param        pointRefRings:  split rings.
        * return        true:           input area equal to the output area
        *               false:          not equal.
        * 
        */
        bool niDecompose2d::_TestByArea(
            niArrayT<niIntArray> &pointRefRings)
        {
            double outputArea = 0.0;
            niPolygon2d polygon;
            niPoint2dArray points;
            size_t numRings = pointRefRings.size();
            for (size_t i = 0; i < numRings; ++i)
            {
                points.clear();
                niIntArray &pointRefRing = pointRefRings[i];
                size_t numPoints = pointRefRing.size();
                for (size_t vi = 0; vi <numPoints; ++vi)
                {
                    int refId = pointRefRing[ vi ];
                    points.push_back( m_fixedPoints[ refId ] );
                }

                polygon.MakePolygon(points);

                niPolygon2dFn fn( polygon );
                double area = fn.Area();
                if (area < 0.0)
                {
                    std::cout
                        <<std::endl
                        <<"    _TestByArea    ERROR    area < 0.0"
                        <<std::endl;
                    return false;
                }

                outputArea += area;
            }

            double diffArea = fabs(m_inputArea - outputArea);

            bool bStat = (0 == niMath::_compare_(outputArea, m_inputArea));

            if (!bStat)
            {
                std::cout
                    <<std::endl
                    <<"    _TestByArea    ERROR    outputArea != m_inputArea"
                    <<std::endl;

                std::cout<<std::setprecision(16)<<"\t\t\toutputArea = "<<outputArea<<std::endl;
                std::cout<<std::setprecision(16)<<"\t\t\tm_inputArea = "<<m_inputArea<<std::endl;
                std::cout<<std::setprecision(16)<<"\t\t\tdiffArea = "<<diffArea<<std::endl;

                return false;
            }
            return bStat;
        }
    }
}
