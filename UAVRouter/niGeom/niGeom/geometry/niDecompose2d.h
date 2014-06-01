//! \file
// \brief
// Decompose multiply connected polygon into simply connected polygons
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-18  JiaoYi     Initial version


#ifndef niDecompose2d_H
#define niDecompose2d_H

#include <niGeom/math/niMath.h>

#include <niGeom/geometry/niGeom2dTypes.h>
#include <niGeom/geometry/niBBox2d.h>
#include <niGeom/geometry/niPolygon2d.h>

#include <niGeom/geometry/tree/niBspTree.h>
#include <niGeom/geometry/tree/niBvhTree.h>
#include <niGeom/geometry/tree/niKdTree.h>

#include <map>
#include <fstream>

namespace ni
{
    namespace geometry
    {
        enum EDecomposeMethod
        {
            eDMBasic        = 1 << 0,
            eDMDelaunay     = 1 << 1
        };

        class niPolygon2d;

        /**
         * \brief Decompose multiply connected polygon into simply connected polygons
         *
         */
        class niDecompose2d
        {
        protected:

            typedef niArrayT< niPairT<int, int> >       SplitPairs;

            typedef std::map< niPairT<int, int>, int >  SplitMaps_T;

            /**
             * \brief Record edge reference IDs start from this vert
             *
             */
            class _SVert2Edge
            {
            public:
                _SVert2Edge()
                {
                }

                inline void         AddFrom             (int e)
                {
                    m_from.push_back(e);
                }

                inline int          NumFrom             () const
                {
                    return int(m_from.size());
                }

                inline int          GetFrom             (int i) const
                {
                    return m_from[i];
                }
            protected:
                niIntArray          m_from;
            };

            /**
             * \brief Record vert reference IDs of start vert and to vert.
             *
             */
            class _SEdge2Vert
            {
            public:
                _SEdge2Vert();

                _SEdge2Vert(int s, int e);

                _SEdge2Vert         (const _SEdge2Vert &other)
                {
                    m_s     = other.m_s;
                    m_t     = other.m_t;
                    m_dirty = other.m_dirty;
                }

                inline void         Init(int s, int t)
                {
                    m_s = s;
                    m_t = t;
                    m_dirty = false;
                }

                inline bool         operator <(const _SEdge2Vert &other) const
                {
                    if (m_s < other.m_s)
                        return true;
                    if (m_s > other.m_s)
                        return false;

                    return m_t < other.m_t;
                }

                inline bool         IsDirty() const
                {
                    return m_dirty;
                }

                inline bool         IsDual(const _SEdge2Vert &other) const
                {
                    return m_s == other.m_t && m_t == other.m_s;
                }

                inline bool         IsDual(int s, int t) const
                {
                    return m_s == t && m_t == s;
                }

                inline void         SetDirty(bool dirty = true)
                {
                    m_dirty = dirty;
                }

                inline int          S() const
                {
                    return m_s;
                }

                inline int          T() const
                {
                    return m_t;
                }

            protected:
                int                 m_s;
                int                 m_t;

                bool                m_dirty;
            };

            typedef niRefPoint2d  niTopoPoint;

            /**
             * \brief Split Edge of two simply polygon
             *
             */
            class niSplitEdge
            {
            public:
                niSplitEdge(
                    const niTopoPoint &a,
                    const niTopoPoint &b,
                    double length,
                    bool bPin = false)
                {
                    Init(a, b, length, bPin);
                }

                niSplitEdge         (const niSplitEdge &other)
                {
                    m_a         = other.m_a;
                    m_b         = other.m_b;
                    m_length    = other.m_length;
                    m_bExcluded = other.m_bExcluded;
                    m_bPin      = other.m_bPin;
                }

                //Exclude the split edge
                inline void         Disable()
                {
                    m_bExcluded = true;
                }

                //Use the split edge
                inline void         Enable()
                {
                    m_bExcluded = false;
                }

                //Check the split edge is excluded
                inline bool         IsExcluded() const
                {
                    return m_bExcluded;
                }

                inline bool         IsPin() const
                {
                    return m_bPin;
                }

                //-----------------------------------------------------------------------------
                // FUNCTION Init
                //-----------------------------------------------------------------------------
                /**
                * Init a split edge
                * @param        a:              end point 1
                * @param        b:              start tangent
                * @param        length:         end point 2
                * @param        bPin:           indicate the split edge wether is pined
                * 
                */
                inline void         Init(
                    const niTopoPoint &a,
                    const niTopoPoint &b,
                    double length,
                    bool bPin = false)

                {
                    if (a < b)
                    {
                        m_a = a;
                        m_b = b;
                    }
                    else
                    {
                        m_a = b;
                        m_b = a;
                    }
                    m_length = length;
                    m_bExcluded = false;
                    m_bPin = bPin;
                }

                //-----------------------------------------------------------------------------
                // FUNCTION copy
                //-----------------------------------------------------------------------------
                /**
                * Copy from other split edge
                * @param        other:          the other split edge
                *  return       splite edge
                * 
                */
                inline niSplitEdge& operator=(const niSplitEdge &other)
                {
                    m_a         = other.m_a;
                    m_b         = other.m_b;
                    m_length    = other.m_length;
                    m_bExcluded = other.m_bExcluded;
                    m_bPin      = other.m_bPin;
                    return *this;
                }

                //-----------------------------------------------------------------------------
                // FUNCTION less
                //-----------------------------------------------------------------------------
                /**
                * Compate with another split edge
                * @param        other:          the other split edge
                *  return       true:           if is less than the other split edge
                *               false:          is not less than the other split edge
                * 
                */
                inline bool         operator <(const niSplitEdge &other) const
                {
                    if (m_length < other.m_length)
                        return true;
                    if (m_length > other.m_length)
                        return false;
                    if (m_a < other.m_a)
                        return true;
                    if (other.m_a < m_a)
                        return false;
                    return m_b < other.m_b;
                }

                //-----------------------------------------------------------------------------
                // FUNCTION equal
                //-----------------------------------------------------------------------------
                /**
                * Compate with another split edge
                * @param        other:          the other split edge
                *  return       true:           if is equal to the other split edge
                *               false:          is not equal to the other split edge
                * 
                */
                inline bool         operator==(const niSplitEdge &other) const
                {
                    return m_a == other.m_a && m_b == other.m_b;
                }

                inline double       Length() const
                {
                    return m_length;
                }

                inline const niTopoPoint& A() const
                {
                    return m_a;
                }

                inline const niTopoPoint& B() const
                {
                    return m_b;
                }

            protected:
                niTopoPoint         m_a;
                niTopoPoint         m_b;
                double              m_length;
                bool                m_bExcluded;
                bool                m_bPin;
            };

            /**
             * \brief Topo edge of polygons
             *
             */
            class niTopoEdge
            {
            public:
                niTopoEdge          ()
                {
                }

                niTopoEdge          (const niTopoPoint &a, const niTopoPoint &b)
                {
                    m_a = a;
                    m_b = b;
                }

                inline void         Init(const niTopoPoint &a, const niTopoPoint &b)
                {
                    m_a = a;
                    m_b = b;
                }

                inline const niTopoPoint& A() const
                {
                    return m_a;
                }

                inline const niTopoPoint& B() const
                {
                    return m_b;
                }

            protected:
                niTopoPoint         m_a;
                niTopoPoint         m_b;
            };

            /**
             * \brief Topo poly of polygons
             *
             */
            class niTopoPoly
            {
            public:
                //Construction
                niTopoPoly          ();

                ~niTopoPoly         ();
                
                //-----------------------------------------------------------------------------
                // FUNCTION AddSplitPair
                //-----------------------------------------------------------------------------
                /**
                * Add a split pair(PointID of A polygon, PolygonId of B polygon)
                * Record the split pair to prevent split edge from one point to the same polygon,
                * @param        splitPair:      PointID of A polygon, PolygonId of B polygon
                * 
                */
                inline void         AddSplitPair(
                    const niPairT<int, int> &splitPair)
                {
                    size_t n = m_splitPairs.size();
                    for (size_t i = 0; i < n; ++i)
                    {
                        if (splitPair == m_splitPairs[i])
                            return;
                    }
                    m_splitPairs.push_back( splitPair );
                }

                //-----------------------------------------------------------------------------
                // FUNCTION RemoveSplitPair
                //-----------------------------------------------------------------------------
                /**
                * Remove a split pair(PointID of A polygon, PolygonId of B polygon)
                * @param        splitPair:      PointID of A polygon, PolygonId of B polygon
                * 
                */
                inline void         RemoveSplitPair(
                    const niPairT<int, int> &splitPair)
                {
                    SplitPairs::iterator iter = m_splitPairs.begin();
                    for (; iter != m_splitPairs.end(); ++iter)
                    {
                        if (splitPair == *iter)
                        {
                            m_splitPairs.erase(iter);
                            break;
                        }
                    }
                }

                //-----------------------------------------------------------------------------
                // FUNCTION IsPointExist
                //-----------------------------------------------------------------------------
                /**
                * Check a point is a split point
                * @param        pointId:        PointID
                * 
                */
                inline bool         IsPointExist(int pointId) const
                {
                    size_t n = m_splitPairs.size();
                    for (size_t i = 0; i < n; ++i)
                    {
                        if (pointId == m_splitPairs[i].m_v1)
                            return true;
                    }
                    return false;
                }

                //-----------------------------------------------------------------------------
                // FUNCTION UpdateSplitDirty
                //-----------------------------------------------------------------------------
                /**
                * To update the split status of a polygon
                * If the polygon is splited by more than different split edges from at least two
                *   points, the polygon will be marked as splited.
                * @param        bSplited:       split status
                * 
                */
                inline void         UpdateSplitDirty(bool bSplited = true)
                {
                    m_splited = false;
                    if (!bSplited || m_splitPairs.size() < 2)
                        return;

                    size_t n = m_splitPairs.size();
                    int pointId1 = m_splitPairs[0].m_v1;
                    for (size_t i = 1; i < n; ++i)
                    {
                        if (pointId1 != m_splitPairs[i].m_v1)
                        {
                            m_splited = true;
                            break;
                    }
                    }
                }

                //-----------------------------------------------------------------------------
                // FUNCTION IsSplited
                //-----------------------------------------------------------------------------
                /**
                * Return the split status
                * 
                */
                inline bool         IsSplited() const
                {
                    return m_splited;
                }

                //-----------------------------------------------------------------------------
                // FUNCTION _compare_
                //-----------------------------------------------------------------------------
                /**
                * Compare the two polygons
                * @param        topoPoly1:      first topopoly
                * @param        topoPoly2:      second topopoly
                * return        true:           less than the second topopoly
                * 
                */
                static bool         _compare_(
                    const niTopoPoly* topoPoly1,
                    const niTopoPoly* topoPoly2)
                {
                    if (topoPoly1->m_area < topoPoly2->m_area)
                        return true;
                    if (topoPoly1->m_area > topoPoly2->m_area)
                        return false;
                    return topoPoly1->m_IdOfPoly < topoPoly2->m_IdOfPoly;
                }

            public:
                niTopoPoint             m_lTopoPoint;
                niTopoPoint             m_rTopoPoint;

                niArrayT<niTopoPoint>   m_topoPoints;
                niArrayT<niTopoEdge>    m_topoEdges;
                niBBox2d                m_bbox;
                niPoint2d               m_center;
                
                SplitPairs              m_splitPairs;

                int                     m_IdOfPoly;
                int                     m_IdOfGroup;

                tree::niBvhTree*        m_hBvh4Edge;
                tree::niBspTree*        m_hBsp4Point;
                double                  m_area;

            protected:
                bool                    m_splited;
            };

            /**
             * \brief Build polygon
             *
             */
            class niPolygonBuilder
            {
            public:
                niPolygonBuilder(
                    const niPoint2dArray &points,
                    const niArrayT<niTopoEdge>& topoEdges,
                    const niArrayT<niSplitEdge>& splitEdges);

                bool                Build(
                    niArrayT<niIntArray> &pointRefRings,
                    niIntArray &duplicatePointIds);

            protected:
                bool                Init(
                    const niArrayT<niTopoEdge>& topoEdges,
                    const niArrayT<niSplitEdge>& splitEdges);

                bool                FindNextEdge(
                    int firstEdgeId,
                    int lastVertId,
                    int currVertId,
                    int &nextVertId,
                    int &nextEdgeId);

                const niPoint2dArray&   m_points;
                niArrayT<_SVert2Edge>   m_v2es;
                niArrayT<_SEdge2Vert>   m_e2vs;
            };


        public:
            niDecompose2d(
                const niPolygon2d &outer,
                const niPolygon2dArray &inners,
                EDecomposeMethod method,
                double coordShift);

            ~niDecompose2d          ();

            bool                    Process(
                niPolygon2dArray &polygons,
                String &error_msg);

        protected:

            bool                    _AddToSplitEdges(
                niSplitEdge& splitEdge);

            bool                    _AddToSplitEdges(
                niArrayT<niSplitEdge>& splitEdges);

            bool                    _BuildPolygons(
                niPolygon2dArray &polygons);
            
            bool                    _CalcDelaunaySplit();

            //bool                    _CalcDelaunaySplit_M2();

            bool                    _CalcNearestSplitEdges();

            bool                    _CalcSplitEdges();

            bool                    _CalcSplitEdges(
                const niTopoPoint &topoPoint1,
                niTopoPoint &topoPoint2,
                niBoolArray &visitMarker,
                double &length);

            bool                    _CalcSplitEdges(
                const tree::niBspTree::niBspNode* polyBspNode,
                const niTopoPoint &topoPoint1,
                niTopoPoint &topoPoint2,
                niBoolArray &visitMarker,
                double &length);

            bool                    _CalcSplitEdges(
                const tree::niBspTree::niBspLeaf* polyBspLeaf,
                const niTopoPoint &topoPoint1,
                niTopoPoint &topoPoint2,
                niBoolArray &visitMarker,
                double &length);

            bool                    _CalcTopoPolyInfo(
                niArrayT<niSplitEdge>& splitEdges,
                niArrayT< niIntArray > &groups);

            void                    _CalcOuterBBox();

            void                    _CheckAndReverse();

            void                    _ClearSplitStatus();

            tree::niBvhTree*        _CreateEdgeBvhTree(
                niArrayT<niTopoEdge> &topoEdges,
                niArrayT<niBBox2d> &bboxes);

            tree::niBvhTree*        _CreateEdgeBvhTree(
                niArrayT<niSplitEdge> &splitEdges);

            void                    _Destory();

            bool                    _FindNearstEdge(
                const tree::niBspTree::niBspNode* polyBspNode1,
                const tree::niBspTree::niBspNode* polyBspNode2,
                niTopoPoint &topoPoint1,
                niTopoPoint &topoPoint2,
                double &length);

            bool                    _FindNearstEdge(
                const tree::niBspTree::niBspLeaf* polyBspLeaf1,
                const tree::niBspTree::niBspLeaf* polyBspLeaf2,
                niTopoPoint &topoPoint1,
                niTopoPoint &topoPoint2,
                double &length);

            bool                    _Init();

            bool                    _InitTopoPolys();

            bool                    _InitTopoPoly(
                niPolygon2d &poly,
                niTopoPoly &topoPoly,
                int IdOfPoly,
                int &iGlobalPntId);

            bool                    _InitDelaunaySplit(
                niArrayT< niTripleT<double, int, int> > &splitTriples,
                niIntArray &convexHullIds);
            
            bool                    _IsColseToOtherPolygon(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const int IdOfPoly1,
                const int IdOfPoly2,
                const double threshold);

            
            bool                    _IsColseToOtherPolygon(
                const tree::niBspTree::niBspNode* polyBspNode,
                const niPoint2d &p0,
                const niPoint2d &p1,
                const int IdOfPoly1,
                const int IdOfPoly2,
                const double threshold);

            bool                    _IsColseToOtherPolygon(
                const tree::niBspTree::niBspLeaf* polyBspLeaf,
                const niPoint2d &p0,
                const niPoint2d &p1,
                const int IdOfPoly1,
                const int IdOfPoly2,
                const double threshold);

            bool                    _IsOverlapEdge(
                const niPoint2d &p0,
                const niPoint2d &p1);

            bool                    _IsOverlapTopoEdge(
                const niPoint2d &p0,
                const niPoint2d &p1);

            bool                    _IsOverlapTopoEdge(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const tree::niBvhTree::niBvhNode *edgeBvhNode);

            bool                    _IsOverlapSplitEdge(
                const niPoint2d &p0,
                const niPoint2d &p1);

            bool                    _IsOverlapSplitEdge(
                const niPoint2d &p0,
                const niPoint2d &p1,
                const tree::niBvhTree::niBvhNode *edgeBvhNode);
            
            bool                    _RefineSplitEdges();

            bool                    _RemoveDuplicateSplitEdges();

            bool                    _TestByArea(
                niArrayT<niIntArray> &pointRefRings);

        protected:
            EDecomposeMethod        m_method;
            double                  m_coordShift;

            niPolygon2dArray        m_polygons;

            niBBox2d                m_originalBBox;

            double                  m_inputArea;
            bool                    m_bRevered;

            niPoint2dArray          m_origalPoints;
            niPoint2dArray          m_fixedPoints;
            niCharArray             m_pointsDirty;

            niArrayT<niTopoPoint>   m_topoPoints;
            niArrayT<niTopoEdge>    m_topoEdges;
            niArrayT<niBBox2d>      m_edgeBBoxes;


            niArrayT<niTopoPoly>    m_topoPolys;
            niArrayT<niBBox2d>      m_polyBBoxes;

            tree::niBvhTree*        m_hBvh4Poly;
            tree::niBvhTree*        m_hBvh4TopoEdge;
            tree::niBspTree*        m_hBsp4Point;

            niArrayT<niSplitEdge>   m_splitEdges;
            tree::niBvhTree*        m_hBvh4SplitEdge;
            int                     m_numSplitEdgeInBVH;

            SplitMaps_T             m_splitMap4Point;
            SplitMaps_T             m_splitMap4Edge;
        };
    }
}


#endif
