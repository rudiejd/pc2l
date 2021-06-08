#ifndef ALLOCATOR_H
#define ALLOCATOR_H
//---------------------------------------------------------------------
//  ____ 
// |  _ \    This file is part of  PC2L:  A Parallel & Cloud Computing 
// | |_) |   Library <http://www.pc2lab.cec.miamioh.edu/pc2l>. PC2L is 
// |  __/    free software: you can  redistribute it and/or  modify it
// |_|       under the terms of the GNU  General Public License  (GPL)
//           as published  by  the   Free  Software Foundation, either
//           version 3 (GPL v3), or  (at your option) a later version.
//    
//   ____    PC2L  is distributed in the hope that it will  be useful,
//  / ___|   but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// | |       WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// | |___    PURPOSE.
//  \____| 
//            Miami University and  the PC2Lab development team make no
//            representations  or  warranties  about the suitability of
//  ____      the software,  either  express  or implied, including but
// |___ \     not limited to the implied warranties of merchantability,
//   __) |    fitness  for a  particular  purpose, or non-infringement.
//  / __/     Miami  University and  its affiliates shall not be liable
// |_____|    for any damages  suffered by the  licensee as a result of
//            using, modifying,  or distributing  this software  or its
//            derivatives.
//
//  _         By using or  copying  this  Software,  Licensee  agree to
// | |        abide  by the intellectual  property laws,  and all other
// | |        applicable  laws of  the U.S.,  and the terms of the  GNU
// | |___     General  Public  License  (version 3).  You  should  have
// |_____|    received a  copy of the  GNU General Public License along
//            with MUSE.  If not,  you may  download  copies  of GPL V3
//            from <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------
// Authors:   JD Rudie  rudiejd@miamioh.edu
//---------------------------------------------------------------------
#include <cstddef>
#include "Utilities.h"
BEGIN_NAMESPACE(pc2l);

template <class T>
class Allocator {
    public:
        using value_type = T;

        using pointer = T*;
        using const_pointer = const T*;

        using void_pointer = void*;
        using const_void_pointer = const void *;

        using difference_type = std::ptrdiff_t;

        Allocator() {
            count = 0;
        }

        template<class U>
        Allocator(const Allocator<U> &other) {}

        ~Allocator() = default;

        pointer allocate(size_t numObjects) {
            return static_cast<T*>(operator new(sizeof(T) * numObjects));
        }

        void deallocate(pointer p, size_t numObjects) {
            operator delete(p);
        }
        

        size_t allo_count() const {
            return count;
        }


    private:
        size_t count;
};


template <class T, class U>
constexpr bool operator== (const Allocator<T>&, const Allocator<U>&) noexcept;

template <class T, class U>
constexpr bool operator!= (const Allocator<T>&, const Allocator<U>&) noexcept; 













END_NAMESPACE(pc2l);
#endif
