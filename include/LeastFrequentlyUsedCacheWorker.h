#ifndef LFU_CACHE_WORKER_H
#define LFU_CACHE_WORKER_H

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
// Authors:   Dhananjai M. Rao          raodm@miamioh.edu
//---------------------------------------------------------------------
/**
 * @file CacheWorker.h
 * @brief Definition of Most Recently Used Cache Worker which implements the
 * Least Frequently Used (LFU) algorithm
 * @author JD Rudie
 * @version 0.1
 */

#include "CacheWorker.h"
#include "Utilities.h"
#include <list>

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
class LeastFrequentlyUsedCacheWorker: public virtual CacheWorker {
public:
    /**
     * Refer the key for a block to our eviction scheme. If the cache is full,
     * this removes the least frequently used block in the cache. If there is a tie
     * for the least frequency, it removes the least recently used item with the least
     * frequency.
     * @param key the key to place into eviction scheme
     */
    void refer(const MessagePtr& msg) override;
private:
    struct CacheItem {
        CacheItem(size_t key) : key(key) {};
        size_t key;
        size_t frequency = 1;
    };
    /**
     * Unordered map for finding position of the block in one of the frequency queues
     */
     std::unordered_map<size_t, std::list<CacheItem>::iterator> placeInQueue;
    /**
     * Ordered map with a separate queue for every frequency in the range of access
     * frequencies for each block currently stored in the cache. Note that we use
     * a std::list here for both complexity (O(1) insertion and erasure since it's a doubly linked list)
     * but also because of its unique properties for iterator invalidation;
     * insertion leaves all iterators unaffected AND erasing only affects the erased
     * iterator See: http://kera.name/articles/2011/06/iterator-invalidation-rules-c0x/
     */
    std::map<int, std::list<CacheItem>> queues;
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
