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

#include "Exception.h"
#include "Utilities.h"
#include "Worker.h"
#include <iostream>
#include <list>
#include <unordered_map>

// namespace pc2l {
BEGIN_NAMESPACE (pc2l);
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

class CacheWorker : public Worker
{
public:
  // Maximum cache size in bytes of this cacheworker
  unsigned long long cacheSize = 16000000000;
  /**
   * The default constructor.  Currently, the consructor initializes
   * some of the instance variables in this class.
   */
  CacheWorker ();

  /**
   * The destructor.
   */
  virtual ~CacheWorker () {}

  /**
   * This is the primary method of a worker.  This method overrides
   * the implementation in the derived class.  This method keeps
   * running (processing messages) until the manager process sends a
   * message to stop the worker.
   */
  virtual void run () override;

  /**
   * Method that computes hash and stores a block of cache data from
   * a given message.
   *
   * \param[in] msg The message that contains a block of cache data
   * to be stored.
   */
  void storeCacheBlock (const MessagePtr &msgIn);

  /**
   * Method that computes hash and erases a block of cache data from
   * a given message.
   *
   * \param[in] msg The message that contains information about a block that
   * needs to be erased.
   */
  void eraseCacheBlock (const MessagePtr &msg);

  /**
   * Method that computes hash and sends the requested block of
   * cache data to the process that requested the block.
   *
   * \param[in] msg The message that contains information about the
   * block of cache requested by the sender of the message.
   */
  void sendCacheBlock (const MessagePtr &msg);

  /**
   * Refer the key for a block to our eviction scheme
   * @param key the key to place into eviction scheme
   */
  virtual void refer (const MessagePtr &msg) = 0;

protected:
  /**
   * Add an item to the cache data structure. This is a pure
   * virtual method since adding items can be different
   * depending upon the data stored for different eviction
   * strategies
   * \param[in] msg reference to a message
   */
  virtual void addToCache (MessagePtr &msg) = 0;

  /**
   * Get an item from the cache. Pure virtual method needed
   * since the data structure used for the cache can vary
   * by cache eviction strategy
   * \param[in] key the key associated with the requested item
   * \return blockNotFoundMsg if not in cache, otherwise reference
   * to the MessagePtr associated with the block
   */
  virtual MessagePtr &getFromCache (size_t key) = 0;

  /**
   * Erase a given key from the cache, then decrement the current number
   * of bytes that the cache is holding
   * @param key key of message to erase
   */
  virtual void eraseFromCache (size_t key) = 0;

  /**
   * If in profiling mode: keep a counter for cache hits
   */
  PC2L_PROFILE (size_t cacheHits = 0;)

  /**
   * Profiling mode: keep a counter of total attempted accesses
   */
  PC2L_PROFILE (size_t accesses = 0;)
  /**
   * The amount of bytes currently stored in the cache manager (incremented
   * each time a message is added
   */
  unsigned int currentBytes = 0;
  /**
   * This is a convenience message that is created in the
   * constructor.  This is used to quickly send a "block-not-found"
   * response in case a block was requested and it was not found.
   * This message is reused to minimize message creation overheads.
   */
  MessagePtr blockNotFoundMsg;
};

END_NAMESPACE (pc2l);
// }   // end namespace pc2l

#endif
