#ifndef MRU_CACHE_WORKER_CPP
#define MRU_CACHE_WORKER_CPP

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
// Authors:   Dhananjai M. Rao, JD Rudie          {raodm, rudiejd}@miamioh.edu
//---------------------------------------------------------------------

#include "Exception.h"
#include "MostRecentlyUsedCacheWorker.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
void MostRecentlyUsedCacheWorker::refer(const MessagePtr& msg) {
    if (MPI_GET_RANK() != 0) return;
    const auto key = msg->key;
    if (auto entry = cache.find(key); entry == cache.end()) {
        // Use eviction strategy if cache is overfull
        if (currentBytes + msg->getSize() > cacheSize) {
            // Get the most recently used and erase it
            auto first = queue.front();
            queue.pop_front();
            // send evicted block to remote cacheworker
            MessagePtr evicted = getFromCache(first);
            eraseCacheBlock(evicted);
            const int destRank = (evicted->blockTag % (System::get().worldSize() - 1)) + 1;
            send(evicted, destRank);
        }
    }else {
        // If the block is present in the cache, we need to update its place in the queue
        queue.erase(entry->second.placeInQueue);
    }
    // new block goes to the beginning (it is mru)
    queue.push_front(key);
}
END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
