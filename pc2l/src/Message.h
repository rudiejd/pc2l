#ifndef MESSAGE_H
#define MESSAGE_H

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

#include <memory>
#include "Utilities.h"
#include "MPIHelper.h"

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

class Message;
using MessagePtr = std::shared_ptr<Message>;

/**
 * A class that encapsulates information being exchange between the
 * distributed processes in the system. The messages are
 * flat/contiguous pieces of information, but with varying sized
 * payloads.  Sending/receiving messages can take some CPU time,
 * particularly due to memory-allocation, and memory-copying
 * overheads.  Hence, this class has been designed to minimize the
 * overheads in the following ways: <ol>
 *
 * <li>Each message is created (see the overloaded create messages)
 * with a single allocation, to contain both the message-header (fixed
 * size) and varying-size payload.  That way a second allocation for
 * the payload is not needed.  Moreover, it ensures that all the
 * information associated with a message is contiguous in memory.</li>
 *
 * <li>Having the message-header and the payload in a contiguous block
 * enables messages to be sent and received in a single operation.</li>
 *
 * <li>A Message can also be created from a fixed backing buffer to
 * further streamline receiving and processing messages.</li>
 *
 *  </ol>
 *
 * The above features come with the following caveats when working
 * with a Message:
 *
 * <ol>
 *
 * <li>Due to their non-standard memory model, messages are not
 * movable or copyable using the traditional copy-constructor or
 * move-constructor.</li>
 *
 * <li>Enabling buffer reuse requires a non-standard approach for
 * memory deallocation.  Hence, it is important that a message pointer
 * is never deleted.  Instead, let the MessagePtr (a std::shared_ptr)
 * automatically handle deletion of messages. </li>
 *
 * </ol>
 */
class Message {
    friend class MessageDeleter;
public:
    /**
     * Enumeration to define the different types of messages that can
     * be sent/received by various classes in PC2L
     */
    enum MsgTag : int {
        STORE_BLOCK = 1, /**< Binary blob with cache data */
        GET_BLOCK,       /**< Send requested cache block back */
        BLOCK_NOT_FOUND, /**< Requested block not found in cache */ 
        FINISH,          /**< Message to ask the worker to finish */
        INVALID_MSG      /**< Just a placeholder */
    };

    /**
     * Messages constructed via the overloaded \c create method (in
     * this class) have a non-standard memory model to ease sending,
     * receiving, and processing messages.  Hence, messages cannot be
     * copied using a standard copy-constructor.  Use an appropriate
     * call to the \c create method to clone/copy a message.
     */
    Message(const Message&) = delete;

    /**
     * Messages constructed via the overloaded \c create method (in
     * this class) have a non-standard memory model to ease sending,
     * receiving, and processing messages.  Hence, messages cannot be
     * moved using a standard move-constructor.  Instead use the \c
     * MessagePtr (a std::shared_ptr) to pass messages to other
     * methods.
     */
    Message(Message &&) = delete;    
    
    /**
     * The destructor to free the memory owned by this message, if any
     */
    ~Message() {}
    
    /**
     * Convenience method to create a message to send data to another
     * process.
     *
     * \param[in] dataSize The size of the data that will be sent with
     * this message.  This value is used to create a flat message with
     * necessary space at the end to send the data.  Use the getData()
     * method to update the raw reference.
     *
     * \param[in] tag The tag to be set for this message.  The tag can
     * always be changed at anytime.
     *
     * \param[in] srcRank The rank of the source process (if any) from
     * where the message was received.  Note that this value can be
     * changed at any time.
     *
     * \return A newly created message object whose payload can be
     * populated.
     */
    static MessagePtr create(const int dataSize, const MsgTag tag,
                             const int srcRank = MPI_ANY_SOURCE);

    /**
     * Convenience method to create a message from a buffer containing
     * data received from another process.  This method essentially
     * just type-casts the supplied buffer and sets the data pointer
     * to point back into the buffer.
     *
     * \note The lifetime of the supplied buffer should exceed the
     * lifetime of the Message object returned by this method.  If you
     * want to make a true-copy then use the create(const Message&)
     * method.
     *
     * \param[in] buffer The buffer to be used as the backing store to
     * the message returned by this method.
     * sent with this message.  This value is used to create a flat
     * message with necessary space at the end to send the data.  Use
     * the getData() method to update the raw reference.
     *
     * \return A message object which is essentially jsut a type-cast
     * of the supplied buffer.
     */
    static MessagePtr create(char* buffer);

    /**
     * Create a clone aka deep-copy of a given message.  This is the
     * only way to create a copy of a method, in case copies are
     * needed.  This method is a convenience method that internally
     * creates a new message and copies the payload information from
     * src.
     *
     * \param[in] src The source message to be cloned.
     *
     * \return The cloned message object.
     */
    static MessagePtr create(const Message& src);
    
    /**
     * Obtain the size of the data associated with this message.
     *
     * \return The size of the data contained in this message.
     */
    int getPayloadSize() const noexcept { return size - sizeof(Message); }

    /**
     * Obtain an immutable reference to the raw data associated with
     * this message.
     *
     * \note Never delete the pointer returned by this method!
     *
     * \return The raw binary data associated with this message.
     */
    const char* getPayload() const noexcept { return payload; }

    /**
     * Obtain an mutable reference to the raw data associated with
     * this message.
     *
     * \note Never delete the pointer returned by this method!
     *
     * \return The raw binary data associated with this message.
     */
    char* getPayload() { return payload; }

    /**
     * Obtain the full size of this message.
     *
     * \return The size of this message.  The size includes size of
     * payload and this class.
     */
    int getSize() const noexcept { return size; }

    /** The tag associated with this message. This tag is useful to
     * distinguish between different types of messages.
     */
    MsgTag tag = INVALID_MSG;
    
    /**
     * The source rank from where the message was recived, if any.
     */
    int srcRank = MPI_ANY_SOURCE;

    /**
     * A unique identifier for each data structure that PC2L is
     * working with. This value is used to associate messages with
     * specific data structures.  Furthermore,  the pair of values
     * {dsTag, blockTag} is used to uniquely identify a cache block
     * in a given data structure.
     */
    unsigned int dsTag = -1U;

    /**
     * A uique identifier for a block of data associated with a given
     * data structure.  The pair of values {dsTag, blockTag} is used
     * to uniquely identify a cache block in a given data structure.
     * This limits the number of blocks associatable with a data
     * structure to a maximum of 4GiB blocks, but each block can be
     * arbitrarly large.
     */
    unsigned int blockTag = -1U;
    
protected:
    /**
     * The constructor is made protected to ensure that this class is
     * not directly instantiated.  Instead use one of the static
     * create methods to create a message class.
     *
     * The constructor is used by the overloaded create method(s) in
     * this class to suitably initialize a message object.
     *
     * \param[in] tag The tag to be set for this message.
     *
     * \param[in] rank The source rank for this message.
     *
     * \param[in] size The total size for this message, including the
     * payload size.
     *
     * \param[in] ownBuf Flag to indicate if this message owns this
     * buffer.
     *
     * \param[in] payload A pointer to the payload associated with
     * this message.
     */
    Message(MsgTag tag, int rank, int size, bool ownBuf, char* payload) :
        tag(tag), srcRank(rank), size(size), ownBuf(ownBuf), payload(payload) {}

    /**
     * The overall size of this message including the memory
     * associated with the payload.  This value is set when a message
     * is created.
     */
    int size; 

private:
    /**
     * Flag to indicate if this message was created from an external
     * buffer or from a buffer that must be deleted when this object
     * goes out of scope.
     *
     *
     * This value is is set to <ul>
     *
     * <li>\c true: The above buffer if the message was constructed
     * via the \c create(int) method in this class and it should be
     * deleted in the destructor.</li>
     *
     * <li>Or, it points to an external buffer using which this
     * message was created via the create(std::vector<char>) class.
     * So the buffer should not be deleted.</li>
     *
     * </ul>
     */
    bool ownBuf = true;

    /**
     * A buffer that contains the binary-data blob associated with
     * this message.  This object either points to the buffer in this
     * class or points to: <ul>
     *
     * <li>The above buffer if the message was constructed via the
     * \c create(int) method in this class.</li>
     *
     * <li>Or, it points to an external buffer using which this
     * message was created via the create(std::vector<char>) class.
     *
     * </ul>
     */
    char* payload = nullptr; 

    /** A custom deleter class to correctly delete Message objects.
     * This class is used inconjunction with the shared_ptr to
     * correctly delete a message created via one of the \c create
     * methods in this class.
     */
    class MessageDeleter {
    public:
        /**
         * @brief  Deletes message object if message owns this buffer 
         * 
         * @param msg Message object to be deleted 
         */
        void operator()(Message* msg) {
            if (msg->ownBuf) {
                delete[] reinterpret_cast<char*>(msg);
            }
        }
    };
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l


#endif
