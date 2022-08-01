#ifndef CACHE_WORKER_H
#define CACHE_WORKER_H

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
 * @brief Definition of CacheWorker class which manages cache entries
 * @author Dhananjai M. Rao
 * @version 0.1
 * @date 2020-04-23
 * 
 */

#include <unordered_map>
#include <list>
#include "Worker.h"

#include <iostream>
#include "Utilities.h"
#include "Exception.h"
#include "System.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
enum EvictionStrategy {
    LeastRecentlyUsed = 1,
    MostRecentlyUsed,
    TimeAwareLeastRecentlyUsed,
    PseudoLeastRecentlyUsed,
    LowInterReferenceRecencySet
};
/**
 * A convenience synonym for caching data associated with different
 * data structures.  The key in the map is computed as:
 * (pc2l::Message::dsTag << 32) | pc2l::Message::blockTag to ease hash
 * computation.  This limits the number of blocks associatable with a
 * data structure to a maximum of 4GiB blocks, but each block can be
 * arbitrarly large.
 */
using DataCache = std::unordered_map<size_t, MessagePtr>;

/**
 * A distributed worker that manages cache entries on a given process.
 * In a distributed run of PC2L, a CacheWorker is run on all worker
 * processes, i.e., processes with MPI-rank > 0.  These nodes are
 * responsible for sending and receiving cache blocks to-and-from
 * other processes, including the manager-process (i.e., MPI-rank ==
 * 0). Each worker process is run on an independen compute-node so as
 * to effectively utlize the main-memory/RAM.
 */

template<EvictionStrategy Strategy = LeastRecentlyUsed>
class CacheWorker : public Worker {
public:
    // Maximum cache size in bytes of this cacheworker
    unsigned long long cacheSize = 16000000000;
    /**
     * The default constructor.  Currently, the consructor initializes
     * some of the instance variables in this class.
     */
    CacheWorker() {
        // Do not perform MPI-related operation in the constructor.
        // Instead do them in the initialize method.
        cacheSize = System::get().cacheManager().cacheSize;
        blockNotFoundMsg = Message::create(0, Message::BLOCK_NOT_FOUND);
    }

    /**
     * The destructor.
     */
    virtual ~CacheWorker() {}
    
    /**
     * This is the primary method of a worker.  This method overrides
     * the implementation in the derived class.  This method keeps
     * running (processing messages) until the manager process sends a
     * message to stop the worker.
     */
    virtual void run() override {
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

    /**
     * Method that computes hash and stores a block of cache data from
     * a given message.
     *
     * \param[in] msg The message that contains a block of cache data
     * to be stored.
     */
    void storeCacheBlock(const MessagePtr& msgIn) {
        PC2L_DEBUG_START_TIMER()
        MessagePtr msg = msgIn;
        // Clone this message for storing into our cache if it resides in a temporary buffer
        if (!msg->ownBuf) {
            msg = Message::create(*msg);
        }
        // Refer to our eviction structure
        refer(msg);
        // Increment current bytes that worker is holding if the block is new
        if (cache.find(msg->key) == cache.end()) {
            currentBytes += msg->getSize();
        }
        // Put a clone of the message in the cache
        cache[msg->key] = msg;
        PC2L_DEBUG_STOP_TIMER("storeCacheBlock() on node " << MPI_GET_RANK() << " ")
    }

    /**
     * Method that computes hash and erases a block of cache data from
     * a given message.
     *
     * \param[in] msg The message that contains information about a block that needs to be erased.
     */
    void eraseCacheBlock(const MessagePtr& msg) {
        // Get entry for key, if present in the cache
        const auto entry = cache.find(msg->key);
        // If the entry is found, delete the entry
        if (entry != cache.end()) {
            // Decrement current bytes that worker is holding
            currentBytes -= entry->second->getSize();
            cache.erase(entry);
        }
        //     // When control drops here, that means the requested block was
        //     // not found in cache.  In this situation, we send a
        //     // block-not-found message back.
        //     blockNotFoundMsg->dsTag    = msg->dsTag;
        //     blockNotFoundMsg->blockTag = msg->blockTag;
        //     send(blockNotFoundMsg, msg->srcRank);
        // }
    }

    /**
     * Method that computes hash and sends the requested block of
     * cache data to the process that requested the block.
     *
     * \param[in] msg The message that contains information about the
     * block of cache requested by the sender of the message.
     */
    void sendCacheBlock(const MessagePtr& msg) {
        PC2L_DEBUG_START_TIMER()
        // Get entry for key, if present in the cache
        const auto entry = cache.find(msg->key);
        // If the entry is found, send it back to the requestor
        if (entry != cache.end()) {
            refer(entry->second);
            send(entry->second, msg->srcRank);
        }
        //     // When control drops here, that means the requested block was
        //     // not found in cache.  In this situation, we send a
        //     // block-not-found message back.
        //     blockNotFoundMsg->dsTag    = msg->dsTag;
        //     blockNotFoundMsg->blockTag = msg->blockTag;
        //     send(blockNotFoundMsg, msg->srcRank);
        // }
        PC2L_DEBUG_STOP_TIMER("sendCacheBlock() on node " << MPI_GET_RANK() << " ")
    }

    /**
     * Refer the key for a block to our eviction scheme
     * @param key the key to place into eviction scheme
     */
    void refer(const MessagePtr& msg) {
        // for now, only do eviction stuff on MANAGER
        if (MPI_GET_RANK() != 0) return;
        const auto key = msg->key;
        if (cache.find(key) == cache.end()) {
            // Use eviction strategy if cache is overfull
            if (msg->getSize() + currentBytes > cacheSize) {
                MessagePtr evicted;
                if constexpr(Strategy == LeastRecentlyUsed) {
                    auto last = lruBlock.back();
                    evicted = cache[last];
                    lruBlock.pop_back();
                    placeInQ.erase(last);
                    eraseCacheBlock(cache[last]);
                } else if constexpr(Strategy == MostRecentlyUsed) {
                    auto first = lruBlock.front();
                    evicted = cache[first];
                    lruBlock.pop_front();
                    placeInQ.erase(first);
                    eraseCacheBlock(cache[first]);
                }
                // send evicted block to remote cacheworker
                const int destRank = (evicted->blockTag % (System::get().worldSize() - 1)) + 1;
                send(evicted, destRank);
            }
        }else {
            // If the block is present in the cache, we need to update its place in the queue
            lruBlock.erase(placeInQ[key]);
        }
        // Update the reference in the order queue
        if constexpr(Strategy == LeastRecentlyUsed) {
            lruBlock.push_front(key);
            placeInQ[key] = lruBlock.begin();
        } else if constexpr(Strategy == MostRecentlyUsed) {
            lruBlock.push_back(key);
            placeInQ[key] = lruBlock.end();
        }
    }
protected:
    /**
     * The in-memory data cache managed by this worker process.
     */
    DataCache cache;

    // Queue: front is block to remove
    std::list<size_t> lruBlock;

    // Stores a key and a reference to that key's place in the LRU/MRU/etc queue
    std::unordered_map<size_t, std::list<size_t>::iterator> placeInQ;
private:
    /**
     * This is a convenience message that is created in the
     * constructor.  This is used to quickly send a "block-not-found"
     * response in case a block was requested and it was not found.
     * This message is reused to minimize message creation overheads.
     */
    MessagePtr blockNotFoundMsg;
    /**
     * The amount of bytes currently stored in the cache manager (incremented each time a message is
     * added
     */
    unsigned int currentBytes = 0;
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
