#ifndef LRU_CACHE_WORKER_H
#define LRU_CACHE_WORKER_H

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
 * @brief Definition of Least Recently Used Cache Worker which implements the
 * Least Recently Used (LRU) algorithm
 * @author JD Rudie
 * @version 0.1
 *
 */

#include "CacheWorker.h"
#include "Utilities.h"
#include <list>
#include <map>

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
class LeastRecentlyUsedCacheWorker : public virtual CacheWorker {
public:
  /**
   * Refer the key for a block to our eviction scheme
   * @param key the key to place into eviction scheme
   */
  void refer(const MessagePtr &msg) override;

protected:
  void addToCache(MessagePtr &msg) override;

  MessagePtr &getFromCache(size_t key) override;

  void eraseFromCache(size_t key) override;

  /**
   * Keys of blocks in queue in their removal order under LRU
   * Note that we use  a std::list here for both complexity (O(1) insertion and
   * erasure since it's a doubly linked list) but also because of its unique
   * properties for iterator invalidation; insertion leaves all iterators
   * unaffected AND erasing only affects the erased iterator See:
   * http://kera.name/articles/2011/06/iterator-invalidation-rules-c0x/
   */
  std::list<size_t> queue;
  /**
   * An internal structure that stores an item in this cache. It consists
   * of a MessagePtr and an iterator for this MessagePtr's current place in
   * the LRU queue, and can be bracket initialized
   */
  struct CacheItem {
    MessagePtr msg;
    std::list<size_t>::iterator placeInQueue;
  };

  std::unordered_map<size_t, CacheItem> cache;

private:
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
