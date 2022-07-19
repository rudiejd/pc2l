#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

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
// Authors:   JD Rudie               rudiejd@miamioh.edu
//---------------------------------------------------------------------
/**
 * @file UnorderedMap.h
 * @brief Definition of UnorderedMap
 * @author JD Rudie
 * @version 0.1
 * @date 2021-08-30
 * 
 */

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "Worker.h"
#include "CacheManager.h"
#include "System.h"
#include "MPIHelper.h"
#include "Message.h"
#include "Vector.h"


// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

/**
 * A distributed vector that runs across multiple machines
 * utilizing message passing through MPI. This initial
 * implementation does not include any caching.
 */
 // T is key type, U is value type
 // associative map
template <typename KeyType, typename ValueType>
class Map {
public:


    struct MapPair {
        KeyType key;
        ValueType value;
        MapPair(const KeyType& key, const ValueType& value) : key(key), value(value) {}
        friend std::ostream& operator<<(std::ostream& output, const MapPair& mp) {
            return output << mp.key << ": " << mp.value;
        }
        virtual ValueType& val() {
            return value;
        }
    };

     typedef typename pc2l::Vector<MapPair>::Iterator iterator;
    // underlying vector representation of the hashmap
    Vector<MapPair> vec;

    /**
     * The default constructor.  Currently, the consructor initializes
     * some of the instance variables in this class.
     */
    Map() {
    }

    /**
     * The destructor.
     */
    virtual ~Map() {}

    iterator insert(KeyType key, ValueType value) {
        iterator i(std::lower_bound(vec.begin(), vec.end(), key, [](const MapPair& lhs, const KeyType& rhs) {
            return lhs.key < rhs;
        }));

        if (i == vec.end() || std::less<KeyType>{}(key, (*i).key)) {
            i = vec.insert(i, MapPair(key, value));
        }
        return i;
    }

    iterator find(const KeyType& k)
    {
        iterator i(std::lower_bound(vec.begin(), vec.end(), k, [](const MapPair& lhs, const KeyType& rhs)-> bool {
            return lhs.key < rhs;
        }));
        if (i != vec.end() && std::less<KeyType>{}(k, (*i).key)) {
            i = vec.end();
        }
        return i;
    }

    ValueType& operator[](const KeyType& key) {
        auto it = find(key);
        if (it == vec.end()) {
            ValueType val{1};
            it = insert(key, val);
        }
        return (*it).val();
    }
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif

