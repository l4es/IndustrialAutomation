// An efficient find_first_of implementation for 
// one-byte elements only (specialization)

#include <cstring>

namespace dpx {

    template<typename InputIter, typename ForwardIter>
        InputIter find_first_of(InputIter first1, 
                InputIter last1, ForwardIter first2, ForwardIter last2)
        {
            unsigned char MapArray[256];
            memset(MapArray, 0, 256);

            for (ForwardIter iter = first2; iter != last2; ++iter)
                MapArray[(unsigned char) *iter] = 1;

            for ( ; first1 != last1; ++first1)
            {
                if (MapArray[(unsigned char) *first1] != 0)
                    return first1;
            }

            return last1;
        }

}

/*
 *  Copyright (c) 2007
 *  Jim Xochellis
 *
 *  Permission to use, copy, modify, distribute and 
 *  sell this software for any purpose
 *  is hereby granted without fee, provided that the 
 *  above copyright notice appears in
 *  all copies and that both that copyright notice 
 *  and this permission notice appear in
 *  supporting documentation. Jim Xochellis makes 
 *  no representations about the suitability
 *  of this software for any purpose. It is provided 
 *  "as is" without express or implied
 *  warranty.
 *
 */
