#ifndef LRU_CACHE_WORKER_CPP
#define LRU_CACHE_WORKER_CPP

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

#include "Exception.h"
#include "PseudoLRUCacheWorker.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
    void PseudoLRUCacheWorker::eraseFromCache(size_t key) {
        cache.erase(key);
    }

    void PseudoLRUCacheWorker::addToCache(pc2l::MessagePtr &msg) {
       cache[msg->key] = {msg};
    }

    MessagePtr &PseudoLRUCacheWorker::getFromCache(size_t key) {
        if (cache.find(key) != cache.end()) {
            return cache[key].msg;
        } else {
            return blockNotFoundMsg;
        }
    }

void PseudoLRUCacheWorker::refer(const MessagePtr& msg) {
    if (MPI_GET_RANK() != 0) return;
    const auto key = msg->key;
    if (auto entry = cache.find(key); entry == cache.end()) {
        // Use eviction strategy if cache is overfull
        if (currentBytes + msg->getSize() > cacheSize) {
            // the cache is full now and we can start resetting the mru bits
            full = true;
            // the first cache item without MRU bit set is removed
            MessagePtr evicted;
            for (auto e : cache) {
                if (!e.second.wasUsed) {
                    evicted = e.second.msg;
                    break;
                }
            }
            eraseCacheBlock(evicted);
            const int destRank = (evicted->blockTag % (System::get().worldSize() - 1)) + 1;
            send(evicted, destRank);
        }
    }else {
        // If the block is present in the cache, we need to update its MRU bit
        entry->second.wasUsed = true;
        trueCount++;
        if (trueCount == cache.size() && full) {
            for (auto e : cache) {
                e.second.wasUsed = false;
            }
            // reset the count of MRU bools set
            trueCount = 0;
        }
    }
}
END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
