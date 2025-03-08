#ifndef LFU_CACHE_WORKER_CPP
#define LFU_CACHE_WORKER_CPP

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
// Authors:  JD Rudie                   <rudiejd@miamioh.edu>
//---------------------------------------------------------------------

#include "LeastFrequentlyUsedCacheWorker.h"
#include "Exception.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

void LeastFrequentlyUsedCacheWorker::addToCache(pc2l::MessagePtr &msg) {
  // this only is needed if it's a new insert. If it's not, refer already
  // updated the cache stuff
  if (placeInQueue.find(msg->key) == placeInQueue.end()) {
    CacheItem ci{msg, msg->key};
    queues[1].push_front(ci);
    placeInQueue[msg->key] = queues[1].begin();
  }
}

void LeastFrequentlyUsedCacheWorker::eraseFromCache(size_t key) {
  // by the time this is called, refer has already fixed cache details
  placeInQueue.erase(key);
}

MessagePtr &LeastFrequentlyUsedCacheWorker::getFromCache(size_t key) {
  if (placeInQueue.find(key) != placeInQueue.end()) {
    return placeInQueue[key]->msg;
  } else {
    return blockNotFoundMsg;
  }
}

void LeastFrequentlyUsedCacheWorker::refer(const MessagePtr &msg) {
  if (MPI_GET_RANK() != 0)
    return;
  const auto key = msg->key;
  if (auto msgPlace = placeInQueue.find(key); msgPlace == placeInQueue.end()) {
    // Use eviction strategy if cache is at capacity
    if (currentBytes + msg->getSize() > cacheSize) {
      // Get the queue for the smallest frequency (always at the beginning
      // of the map of queues)
      auto &smallestFreqQueue = queues.begin()->second;
      // Item at the end of this queue is the LRU
      auto evictedItem = smallestFreqQueue.back();
      smallestFreqQueue.pop_back();
      // if this frequency queue is now empty, erase the frequency from the
      // map of all freq. queues
      if (smallestFreqQueue.empty()) {
        queues.erase(evictedItem.frequency);
      }
      // send evicted block to remote cacheworker
      MessagePtr evicted = evictedItem.msg;
      eraseCacheBlock(evicted);
      const int destRank =
          (evicted->blockTag % (System::get().worldSize() - 1)) + 1;
      send(evicted, destRank);
    }
  } else {
    // If the block is present in the cache, we need to update which
    // frequency queue it is in
    auto item = *placeInQueue[key];
    auto &itemQueue = queues[item.frequency];
    itemQueue.erase(placeInQueue[key]);
    // if this frequency queue is now empty, erase the frequency from the map
    // of all freq. queues
    if (itemQueue.empty()) {
      queues.erase(item.frequency);
    }
    item.frequency++;
    queues[item.frequency].push_front(item);
    placeInQueue[key] = queues[item.frequency].begin();
  }
}
END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
