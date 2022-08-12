#ifndef TALRU_CACHE_WORKER_H
#define TALRU_CACHE_WORKER_H

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
// Authors:   JD Rudie          rudiejd@miamioh.edu
//---------------------------------------------------------------------
/**
 * @file PseudoLRU.h
 * @brief Definition of Least Recently Used Cache Worker which implements the
 *  Bit Pseudo-LRU algorithm
 * @author JD Rudie
 * @version 0.1
 *
 */

#include "CacheWorker.h"
#include "Utilities.h"
#include <list>

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);
class PseudoLRUCacheWorker: public virtual CacheWorker {
public:
    /**
     * Refer the key for a block to our eviction scheme
     * @param key the key to place into eviction scheme
     */
    void refer(const MessagePtr& msg) override;
private:
    /**
     * Cache items in the PLRU cache require only a flag
     * to say whether they were MRU (MRU bit)
     */
    struct CacheItem {
        MessagePtr msg;
        bool wasUsed = false;
    };

    void eraseFromCache(size_t key) override;

    void addToCache(pc2l::MessagePtr &msg) override;

    MessagePtr & getFromCache(size_t key) override;

    std::unordered_map<size_t, CacheItem> cache;

    /**
     * Count of items in the cache with MRU bit set
     */

    size_t trueCount = 0;
    /**
     * Flag to say whether the cache is full
     */
     bool full = false;
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
