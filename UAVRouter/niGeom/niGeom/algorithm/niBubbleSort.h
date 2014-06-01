//! \file
// \brief
// bubble sort
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2013-02-17  JiaoYi     Initial version


#ifndef niBubbleSort_H
#define niBubbleSort_H

namespace ni
{
    namespace algorithm
    {
        /**
         * \brief bubble sort
         *
         */
        template<typename _T>
        class niBubbleSort
        {
        public:
            niBubbleSort            ()
            {
            }

            //-----------------------------------------------------------------------------
            // FUNCTION Sort
            //-----------------------------------------------------------------------------
            /**
            * Bubble sort
            *
            * @param        Input:          elements need to be sorted
            * @param        n:              number of elements
            * return        true:           success
            *               false:          failed
            * 
            */
            static int              Sort                    (_T Input[], const int N)
            {
                _T SWAP;
                bool needGoOn = true;
                for (int i = N-1; needGoOn && i >= 0; --i)
                {
                    needGoOn = false;
                    for (int j = 0; j < i; ++j)
                    {
                        if (Input[j+1] < Input[j])
                        {
                            SWAP        = Input[j+1];
                            Input[j+1]  = Input[j];
                            Input[j]    = SWAP;
                            needGoOn    = true;
                        }
                    }
                }
                
                return true;
            }
        };
    }
}

#endif