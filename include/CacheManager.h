#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

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
#include "CacheWorker.h"
#include "LeastRecentlyUsedCacheWorker.h"
#include "MostRecentlyUsedCacheWorker.h"
#include "PseudoLRUCacheWorker.h"
#include "LeastFrequentlyUsedCacheWorker.h"

/**
 * @file CacheManager.h
 *
 * @brief Definition of abstract CacheManager class which manages cache entries
 * in conjunction with worker processes. Also includes trivial definitions
 * for cache managers with different eviction strategies
 * @author Dhananjai M. Rao, JD Rudie
 * @version 0.1
 * @date 2020-04-23
 */
// namespace pc2l {
#include <queue>
BEGIN_NAMESPACE(pc2l);

/**
 * A centralized manager that manages cache entries in coordination
 * with zero-or-more worker processes.  In a distributed run of PC2L,
 * a CacheManager is run only on the manager-processes, i.e.,
 * processes with MPI-rank == 0.  The manager process is responsible
 * for maintaining a local cache and updating caches on distributed
 * worker processes.
 */
class CacheManager : public virtual CacheWorker {
public:
    /**
     * The default constructor.  Currently, the constructor does not
     * have much to do but is present for future extensions.
     */
    CacheManager() {}

    /**
     * The polymorphic destructor.  The destructor does not have much
     * to do but is present for future extensions (if any).
     */
    virtual ~CacheManager() {}

    /**
     * The manager does not have a specific task in the run method.
     * The operations of a manager are triggered by data structures to
     * access information in the manager. However, in order to utilize the
     * memory of the manager as well, we run a typical 
     * CacheWorker thread in the background on the manager node.
     * 
     */
    void run() override;

    /**
     * The finalize method sends finish messages to all of the workers
     * to let them know they need to wind-up their operation.
     */
    void finalize() override;

    /**
     * Retrieve a block from the manager cache. If it's not there, return nullptr
     * \param[in] dsTag the data structure tag associated with this message
     * \param[in] blockTag the block tag associated with this message
     * \return message associated with the key formed by combining these tags
     */
    MessagePtr getBlock(size_t dsTag, size_t blockTag);

    /**
     * Retrieve a block from the manager cache. If it's not there, fallback to remote CacheWorker
     * \param[in] dsTag the data structure tag associated with this block
     * \param[in] blockTag the block tag associated with this block
     * \return message containing block requested
     */
    MessagePtr getBlockFallbackRemote(size_t dsTag, size_t blockTag);

    /**
     *  Retrieve a block from a remote CacheWorker in a non-blocking fashion
     * \param[in] dsTag the data structure tag associated with this block
     * \param[in] blockTag the block tag associated with this block
     *
     */
    void getRemoteBlockNonblocking(size_t dsTag, size_t blockTag);

    /**
     * Gives a reference to the manager's cache for use in insertion
     * logic
     * @return reference to manager's cache
     */
    DataCache& managerCache() {
        return cache;
    }

private:
    MPI_Request prefetchReq;
    MessagePtr prefetchMsg;
};

/**
 * CacheManager which implements the Least Recently Used (LRU) cache eviction algorithm
 */
class LeastRecentlyUsedCacheManager : public LeastRecentlyUsedCacheWorker, public CacheManager{};

/**
 * CacheManager which implements the Most Recently Used (MRU) cache eviction algorithm
 */
class MostRecentlyUsedCacheManager : public MostRecentlyUsedCacheWorker, public CacheManager{};

/**
 * CacheManager which implements the Least Frequently Used (LFU) cache eviction algorithm
 */
class LeastFrequentlyUsedCacheManager : public LeastFrequentlyUsedCacheWorker, public CacheManager{};

/**
 * CacheManager which implements the Pseudo-LRU cache eviction algorithm
 */
class PseudoLRUCacheManager : public PseudoLRUCacheWorker, public CacheManager{};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
