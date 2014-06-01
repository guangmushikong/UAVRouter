//! \file
// \brief
// Define enum types of geometry
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niTypes_H
#define niTypes_H

#include <niGeom/niBlob.h>
#include <niGeom/niArrayT.h>

namespace ni
{
    typedef niArrayT<bool>      niBoolArray;
    typedef niArrayT<int>       niIntArray;
    typedef niArrayT<char>      niCharArray;
    typedef niArrayT<float>     niFloatArray;
    typedef niArrayT<double>    niDoubleArray;


    typedef std::string         String;
    typedef std::vector<String> StringArray;

    typedef unsigned char       Uint8;
    typedef unsigned long       Uint32;

    /**
     * \brief pair
     *
     */
    template<typename _T1, typename _T2>
    class niPairT
    {
    public:
        niPairT              (): m_v1(0), m_v2(0)
        {
        }
        niPairT              (_T1 v1, _T2 v2)
            : m_v1(v1)
            , m_v2(v2)
        {
        }

        inline void         Init(_T1 v1, _T2 v2)
        {
            m_v1 = v1;
            m_v2 = v2;
        }

        inline bool         operator==(
            const niPairT &other) const
        {
            return m_v1 == other.m_v1 && m_v2 == other.m_v2;
        }

        inline bool         operator<(
            const niPairT &other) const
        {
            if (m_v1 < other.m_v1)
                return true;
            if (m_v1 > other.m_v1)
                return false;
            return m_v2 < other.m_v2;
        }

    public:
        _T1                 m_v1;
        _T2                 m_v2;
    };

    /**
     * \brief triple
     *
     */
    template<typename _T1, typename _T2, typename _T3>
    class niTripleT
    {
    public:
        niTripleT           (): m_v1(0), m_v2(0), m_v3(0)
        {
        }
        niTripleT           (_T1 v1, _T2 v2, _T3 v3)
            : m_v1(v1)
            , m_v2(v2)
            , m_v3(v3)
        {
        }

        inline bool         operator<(
            const niTripleT &other) const
        {
            if (m_v1 < other.m_v1)
                return true;
            if (m_v1 > other.m_v1)
                return false;
            if (m_v2 < other.m_v2)
                return true;
            if (m_v2 > other.m_v2)
                return false;
            return m_v3 < other.m_v3;
        }

    public:
        _T1                 m_v1;
        _T2                 m_v2;
        _T3                 m_v3;
    };
}
#endif
