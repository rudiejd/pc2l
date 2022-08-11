#ifndef CACHE_WORKER_CPP
#define CACHE_WORKER_CPP

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

#include "CacheWorker.h"
#include "Exception.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
    CacheWorker::CacheWorker() {
        // Do not perform MPI-related operation in the constructor.
        // Instead do them in the initialize method.
        cacheSize = System::get().cacheSize;
        blockNotFoundMsg = Message::create(0, Message::BLOCK_NOT_FOUND);
    }

    void CacheWorker::run() {
    // Keep processing messages until we get a message with finish tag.
        for (MessagePtr msg = recv(); msg->tag != Message::FINISH; msg = recv()) {
            switch (msg->tag) {
            case Message::STORE_BLOCK:
            storeCacheBlock(msg);
            break;
            case Message::GET_BLOCK:
            sendCacheBlock(msg);
            break;
            case Message::ERASE_BLOCK:
            eraseCacheBlock(msg);
            break;
            default:
            throw PC2L_EXP("Received unhandled message. Tag=%d",
                           "Need to implement?", msg->tag);
            }
        }
    }

    void CacheWorker::storeCacheBlock(const MessagePtr& msgIn) {
        PC2L_DEBUG_START_TIMER()
        MessagePtr msg = msgIn;
        // Clone this message for storing into our cache if it resides in a temporary buffer
        if (!msg->ownBuf) {
                msg = Message::create(*msg);
        }
        // Refer to our eviction structure
        refer(msg);
        // Bring bytes that cache holds up to date
        const auto& existingEntry = getFromCache(msg->key);
        if (existingEntry != blockNotFoundMsg) {
            currentBytes -= existingEntry->getSize();
        }
        currentBytes += msg->getSize();
        // Put a clone of the message in the cache
//        cache[msg->key] = msg;
        addToCache(msg);
        PC2L_DEBUG_STOP_TIMER("storeCacheBlock() on node " << MPI_GET_RANK() << " ")
    }

    void CacheWorker::eraseCacheBlock(const MessagePtr& msg) {
        // If the cache block found, delete it
        if (getFromCache(msg->key) != blockNotFoundMsg) {
            PC2L_PROFILE(cacheHits++;)
            // Decrement current bytes that worker is holding
            eraseFromCache(msg->key);
//            currentBytes -= entry->second->getSize();
//            cache.erase(entry);

        }
        PC2L_PROFILE(accesses++;)
        //     // When control drops here, that means the requested block was
        //     // not found in cache.  In this situation, we send a
        //     // block-not-found message back.
        //     blockNotFoundMsg->dsTag    = msg->dsTag;
        //     blockNotFoundMsg->blockTag = msg->blockTag;
        //     send(blockNotFoundMsg, msg->srcRank);
        // }
    }

    void CacheWorker::sendCacheBlock(const MessagePtr& msg) {
        PC2L_DEBUG_START_TIMER()
        // Get entry for key, if present in the cache
        const auto& entry = getFromCache(msg->key);
        // If the entry is found, send it back to the requestor
        if (entry != blockNotFoundMsg) {
            PC2L_PROFILE(cacheHits++;)
            refer(entry);
            send(entry, msg->srcRank);
        }
        PC2L_PROFILE(accesses++;)
        //     // When control drops here, that means the requested block was
        //     // not found in cache.  In this situation, we send a
        //     // block-not-found message back.
        //     blockNotFoundMsg->dsTag    = msg->dsTag;
        //     blockNotFoundMsg->blockTag = msg->blockTag;
        //     send(blockNotFoundMsg, msg->srcRank);
        // }
        PC2L_DEBUG_STOP_TIMER("sendCacheBlock() on node " << MPI_GET_RANK() << " ")
    }
END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
