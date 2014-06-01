//! \file
// \brief
// Binary insert sort
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-17  JiaoYi     Initial version

#ifndef niBinaryInsertSort_H
#define niBinaryInsertSort_H

#include <niGeom/niTypes.h>

namespace ni
{
    namespace algorithm
    {
        /**
         * \brief binary insert sort
         *
         */
        template<typename _T>
        class niBinaryInsertSort
        {
        public:
            niBinaryInsertSort      () : N(0), L(-1), R(-1)
            {
            }
            ~niBinaryInsertSort     ()
            {
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Init
            //-----------------------------------------------------------------------------
            /**
            * Calc the distance from bbox to bbox
            *
            * @param      n:            number of elements
            * @return     true:         sucess
            *             false:        failed
            * 
            */
            inline bool             Init                    (const int n)
            {
                if (n < 1)
                    return false;
                N = n;
                MEM.resize(N * 2 + 1);
                L = R = -1;
                return true;
            }

            //-----------------------------------------------------------------------------
            // FUNCTION InsertFirst
            //-----------------------------------------------------------------------------
            /**
            * Insert the first value
            *
            * @param      V:            value
            * 
            */
            inline void             InsertFirst             (const _T &V)
            {
                L = R = N;
                MEM[L] = V;
            }

            
            //-----------------------------------------------------------------------------
            // FUNCTION Insert
            //-----------------------------------------------------------------------------
            /**
            * Find the right position, then insert it
            *
            * @param      V:            value
            * 
            */
            inline void             Insert                  (const _T &V)
            {
                int _l = L, _r = R, _m;
                while (_l < _r)
                {
                    _m = (_l + _r) / 2;
                    if (V < MEM[_m])
                    {
                        _r = _m-1;
                    }
                    else
                    {
                        _l = _m + 1;
                    }
                }
                _m = _l;
                if (V >= MEM[_m])
                    ++_m;

                if (_m - L < R - _m)
                {
                    for (int k = L; k < _m; ++k)
                    {
                        MEM[k-1] = MEM[k];
                    }
                    MEM[_m-1] = V;
                    --L;
                }
                else
                {
                    for (int k = R; k >= _m; --k)
                    {
                        MEM[k+1] = MEM[k];
                    }
                    MEM[_m] = V;
                    ++R;
                }
            }

            
            //-----------------------------------------------------------------------------
            // FUNCTION Insert
            //-----------------------------------------------------------------------------
            /**
            * Find the right position, then insert it
            *
            * @param        Input:          elements need to be sorted
            * @param        n:              number of elements
            * return        true:           success
            *               false:          failed
            * 
            */
            bool                    Sort                    (const _T Input[], const int n)
            {
                bool bStat = Init(n);
                if (!bStat)
                {
                    return false;
                }

                InsertFirst(Input[0]);

                for (int i = 1; i < N; ++i)
                {
                    Insert( Input[i] );
                }
                return true;
            }

        protected:
            niArrayT<_T>            MEM;
            int                     N;
            int                     L;
            int                     R;
        };
    }
}

#endif