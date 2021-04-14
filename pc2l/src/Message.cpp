#ifndef MESSAGE_CPP
#define MESSAGE_CPP

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

#include <algorithm>
#include "Message.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

// Create a message from scratch using dynamic memory
MessagePtr
Message::create(const int dataSize, const MsgTag tag,
                const int srcRank) {
    // First create a dynamic memory block for this message, even
    // though we are going to return it as if it were an object.
    char* rawBuf = new char[dataSize + sizeof(Message)];
    // Now use placement new to initialize the message
    Message *msg = new(rawBuf) Message(tag, srcRank,
                                       dataSize + sizeof(Message), true,
                                       rawBuf   + sizeof(Message));
    // Return the newly created object
    return MessagePtr(msg, MessageDeleter());
}

// Make a message from a given buffer
MessagePtr
Message::create(char *buffer) {
    // First simply reinterpret cast the buffer into a message.
    Message* msg = reinterpret_cast<Message*>(buffer);
    // Next setup the payload and ownBuf correctly
    msg->ownBuf  = false;
    msg->payload = buffer + sizeof(Message);
    // Return the newly created object
    return MessagePtr(msg, MessageDeleter());
}

MessagePtr
Message::create(const Message& src) {
    // First create a message from the source.
    MessagePtr msg = Message::create(src.getPayloadSize(),
                                     src.tag, src.srcRank);
    // Copy the data from source to the newly created message
    std::copy_n(src.getPayload(), src.getPayloadSize(), msg->getPayload());
    // Return the newly created msg
    return msg;
}

END_NAMESPACE(pc2l);
// }   // end namespace pc2l


#endif
