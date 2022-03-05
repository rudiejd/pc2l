#ifndef VECTOR_H
#define VECTOR_H

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
 * @file Vector.h
 * @brief Definition of Vector
 * @author JD Rudie
 * @version 0.1
 * @date 2021-08-30
 * 
 */

#include <unordered_map>
#include "Worker.h"
#include "CacheManager.h"
#include "System.h"
#include "MPIHelper.h"
#include "Message.h"


// namespace pc2l {
BEGIN_NAMESPACE(pc2l);


/**
 * A distributed vector that runs across multiple machines
 * utilizing message passing through MPI. This initial  
 * implementation does not include any caching.
 */
template <typename T>
class Vector {
public:
    /**
     * The default constructor.  Currently, the constructor calls the
     * workhorse
     */
    Vector() : Vector(32000) { }

    /**
     *  Construct a vector by specifying the block size. Currently the
     *  workhorse constructor
     * @param bSize size in bytes of a block
     */
    explicit Vector(unsigned int bSize) : blockSize(bSize), siz(0), dsTag(System::get().dsCount++) { }
    /**
     * The destructor.
     */
    virtual ~Vector() = default;

    int dsTag;

    unsigned int blockSize;

    // The number of elements currently in the vector
    unsigned long long siz;

    unsigned long long size() {
        return siz;
    }

    // TODO: make it work with changes to caching scheme
    void clear() {
        for (unsigned int i = 0; i < siz; i++) {
            erase(i);
        }
    }

    // TODO: make it work with changes to caching scheme
    void erase(unsigned int index) {
        // obtain world size() and compute destination rank for deletion
        const int worldSize = System::get().worldSize();
        const int destRank = (index % (worldSize - 1)) + 1;
        CacheManager& cm = System::get().cacheManager();
        // move all of the blocks to the right of the index left by one
        for (int i = index; i < size() - 1; i++) { replace(i, at(i + 1)); }
        // clear the last index which is now junk
        MessagePtr m = Message::create(1, Message::ERASE_BLOCK, 0);
        m->dsTag = dsTag;
        m->blockTag = size() - 1;
        cm.send(m, destRank);
        // size() can now be decremented
        siz--;
        // maybe some check to see if it is successfully deleted?
    }

    T at(unsigned int index) {
        CacheManager& cm  = System::get().cacheManager();
        auto msg = Message::create(1, Message::GET_BLOCK, 0);
        msg->dsTag = dsTag;
        msg->blockTag = index*sizeof(T)  / blockSize;
        MessagePtr rec;
        // if the CacheManager's cache contains this block, just get it
        if (cm.managerCache().find(CacheWorker::getKey(msg)) != cm.managerCache().end()) {
            rec = cm.managerCache()[CacheWorker::getKey(msg)];
        } else {
            // otherwise, we have to get it from a remote cacheworker
            const unsigned int worldSize = System::get().worldSize();
            const int sourceRank = (index % (worldSize - 1)) + 1;
            cm.send(msg, sourceRank);
            rec = cm.recv(sourceRank);
        }
        // get array of concatenated T-serializations
        char* payload = rec->getPayload();
        // offset into this array and extract correct portion
        unsigned int inBlockIdx = ((index * sizeof(T)) % blockSize);
        char serializedObj[sizeof(T)];
        std::copy(&payload[inBlockIdx], &payload[inBlockIdx + sizeof(T)], &serializedObj[0]);
        auto ret = reinterpret_cast<T*>(serializedObj);
        return *ret;
    }

    void insert(unsigned int index, T value) {
        CacheManager& cm = System::get().cacheManager();
        // construct message and fill the buffer with data to insert
        MessagePtr m = Message::create(blockSize, Message::STORE_BLOCK, 0);
        m->dsTag = dsTag;
        m->blockTag = index*sizeof(T)  / blockSize;
        // always insert into the cache manager's local cache - only move to cache worker on eviction
        auto& mgrCache = System::get().cacheManager().managerCache();
        // if there is already a block in the CM cache, just put the value into that one
        if (mgrCache.find(CacheWorker::getKey(m)) != mgrCache.end()) {
            m = mgrCache[CacheWorker::getKey(m)];
        }
        char* block = m->getPayload();
        // offset into the block array of serializations and insert val
        unsigned int inBlockIdx = ((index * sizeof(T)) % blockSize);
        char* serialized = reinterpret_cast<char*>(&value);
        std::copy(&serialized[0], &serialized[sizeof(T)], &block[inBlockIdx]);
        // modified block goes back to cache manager
        mgrCache[CacheWorker::getKey(m)] = m;

        // if the insert isnt at end, we have to move all right elements to right
        siz++;
    }

    // TODO: make it work with changes to caching scheme
    void replace(unsigned int index, T value) {
        // obtain world size() and compute destination rank for replacement
        const int worldSize = System::get().worldSize();
        const int destRank = (index % (worldSize - 1)) + 1;
        CacheManager& cm = System::get().cacheManager();
        // construct message and fill the buffer with new datum
        MessagePtr m = Message::create(sizeof(T), Message::STORE_BLOCK, 0);
        T* buff = reinterpret_cast<T*>(m->payload);
        *buff = value;
        m->dsTag = dsTag;
        m->blockTag = index;
        cm.send(m, destRank);
        // size() remains the same here
    }
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif

