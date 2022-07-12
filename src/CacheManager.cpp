#ifndef CACHE_MANAGER_CPP
#define CACHE_MANAGER_CPP

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
#include <thread>
#include "CacheManager.h"
#include "Exception.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

void
CacheManager::finalize() {
    const auto workers = MPI_GET_SIZE();
    auto finMsg = Message::create(0, Message::FINISH);
    // Send finish message to all of the worker-processes
    for (int rank = 1; (rank < workers); rank++) {
        send(finMsg, rank);
    }
}

MessagePtr CacheManager::getBlock(size_t dsTag, size_t blockTag) {
    // see if this is something we've prefetched. if so, just wait on the request
    if (prefetchMsg != nullptr &&
        dsTag == prefetchMsg->dsTag && blockTag == prefetchMsg->dsTag) {
        // ask worker to wait on the result then return it
        return wait(prefetchReq);
    }
    size_t key = Message::getKey(dsTag, blockTag);
    try {
        return cache.at(key);
    } catch (const std::out_of_range& e) {
        return nullptr;
    }
}

MessagePtr CacheManager::getBlockFallbackRemote(size_t dsTag, size_t blockTag) {
    MessagePtr ret = getBlock(dsTag, blockTag);
    if (ret == nullptr) {
        // otherwise, we have to get it from a remote cacheworker
        // if we're in profiling mode, note this
        if (System::get().profile) {
            std::cout << "miss," << dsTag << ',' << blockTag << std::endl;
        }
        const unsigned long long worldSize = System::get().worldSize();
        const int storedRank = (blockTag % (worldSize - 1)) + 1;
        ret = Message::create(0, Message::GET_BLOCK, 0, dsTag, blockTag);
        send(ret, storedRank);
        ret = recv(storedRank);
        // then put the object at retrieved index into cache
        storeCacheBlock(ret);
    }
    refer(cache[ret->key]);

    return cache[ret->key];
}

void CacheManager::getRemoteBlockNonblocking(size_t dsTag, size_t blockTag) {
    const unsigned long long worldSize = System::get().worldSize();
    const int storedRank = (blockTag % (worldSize - 1)) + 1;
    MessagePtr reqMsg = Message::create(0, Message::GET_BLOCK, 0, dsTag, blockTag);
    send(reqMsg, storedRank);
    // do a non-blocking receive call and store the request as a member
    prefetchReq = startReceiveNonblocking(storedRank);
}


    void CacheManager::run() {
        // bgWorker = std::thread(CacheManager::runBackgroundWorker);
}

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
