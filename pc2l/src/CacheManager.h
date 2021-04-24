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

#include "CacheWorker.h"

/**
 * @file CacheManager.h
 * @brief Definition of CacheManager class which manages cache entries
 * in conjunction with worker processes.
 * @author Dhananjai M. Rao
 * @version 0.1
 * @date 2020-04-23
 * 
 */
// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

/**
 * A centralized manager that manages cache entries in coordination
 * with zero-or-more worker processes.  In a distributed run of PC2L,
 * a CacheManager is run only on the manager-processes, i.e.,
 * processes with MPI-rank == 0.  The manager process is responsible
 * for maintaining a local cache and updating caches on distributed
 * worker processes.
 */
class CacheManager : public CacheWorker {
public:
    /**
     * The default constructor.  Currently, the consructor does not
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
     * access information in the manager.  Hence, this run method is
     * intentionally blank.
     */
    void run() override {}

    /**
     * The finalize method sends finish messages to all of the workers
     * to let them know they need to wind-up their operation.
     */
    void finalize() override;
};


END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
