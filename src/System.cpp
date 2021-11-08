#ifndef SYSTEM_CPP
#define SYSTEM_CPP

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

#include "MPIHelper.h"
#include "System.h"
#include "Exception.h"
#include "CacheManager.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

// The process-wide singleton object for further use
System System::system;

/**
 * The class that operates as the manager.  There is only 1
 * manager-process in the distributed system that coordinates all the
 * activities.  Ideally, this should be inside the System class.
 * However, that requires exposing some of the internal
 * implementations in the world-facing header.  To avoid having to do
 * that, the manager instance is defined here. Not a perfect design,
 * but a practical one.
 */
CacheManager manager;
    

void
System::initialize(int& argc, char *argv[], bool initMPI) {
    // Check an iniitalize MPI
    if (initMPI) {
        MPI_INIT(argc, argv);
    }
    size = MPI_GET_SIZE();
}

void
System::start(const OpMode mode) {
    // Next, based on our operation mode, perform different initialization.
    switch (mode) {
    case OneWriter_DistributedCache:
        oneWriterDistribCache();
        break;
    case InvalidMode:
    default:
        throw PC2L_EXP("Invalid OpMode in initMPI %d",
                       "Ensure OpMode is valid", mode);
    }    
}

int System::worldSize() {
    return size;
}

void
System::stop() {
    // If this is the manager process, then send finish messages to
    // all the workers to let them them know they need to stop running.
    if (MPI_GET_RANK() == 0) {
        manager.finalize();
    }
}

void
System::finalize(bool finMPI) noexcept {
    if (finMPI) {
        MPI_FINALIZE();
    }
}

void
System::oneWriterDistribCache() {
    if (MPI_GET_RANK() == 0) {
        // We assume this process is the manager.
        manager.initialize();
    } else {
        // Here this process is running as a worker.  So perform the
        // worker's lifecycle activities here.
        CacheWorker worker;
        worker.initialize();  // Initalize
        worker.run();         // This method runs until manager send finish
        worker.finalize();    // Do any clean-ups for this run
    }
}

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
