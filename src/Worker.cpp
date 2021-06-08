#ifndef WORKER_CPP
#define WORKER_CPP

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
#include "Utilities.h"
#include "Exception.h"
#include "Worker.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

// Sending a message is relatively simple
void
Worker::send(MessagePtr msgPtr, const int destRank) {
    // Send message only if the pointer is set
    if (msgPtr) {
        MPI_SEND(msgPtr.get(), msgPtr->getSize(), MPI_CHAR, destRank,
                 msgPtr->tag);
    }
}

// Recieve a message using our recv buffer
MessagePtr
Worker::recv(const int srcRank, const bool blocking,
             const int tag) {
    // First poll and find out the size of the message to read
    MPI_STATUS status;
    try {
        if (!blocking && !MPI_IPROBE(srcRank, tag, status)) {
            // No pending message.
            return MessagePtr(nullptr);
        } else if (blocking) {
            // Wait until we get a valid message to read.
            MPI_PROBE(srcRank, tag, status);
        }
    } catch (CONST_EXP MPI_EXCEPTION& e) {
        // Rethrow MPI exception as a pc2l::Exception
        throw PC2L_EXP(e.Get_error_string(), "MPI_PROBE error (can't do much)");
    }
    
    // Figure out the size of the size we need.
    const int msgSize = MPI_GET_COUNT(status, MPI_TYPE_CHAR);
    recvBuf.resize(msgSize);
    // Read the actual string data.
    try {
        MPI_RECV(recvBuf.data(), msgSize, MPI_CHAR, status.MPI_SOURCE,
                 status.MPI_TAG, status);
    } catch (CONST_EXP MPI_EXCEPTION& e) {
        // Rethrow MPI exception as a pc2l::Exception
        throw PC2L_EXP(e.Get_error_string(), "MPI_RECV error (can't do much)");
    }

    // Return our buffer as if it is a message
    return Message::create(recvBuf.data());
}

void
Worker::run() {
    throw PC2L_EXP("Not implemented",
                   "Need to override run() method in derived class.");
}

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
