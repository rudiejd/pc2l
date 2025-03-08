#ifndef WORKER_H
#define WORKER_H

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
 * @file Worker.h
 * @author Dhananjai M. Rao (raodm@miamioh.edu)
 * @brief Definition of base class for all workers in PC2L
 * @version 0.1
 * @date 2021-04-23
 */

#include "Message.h"
#include <variant>
#include <vector>

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

/**
 * A simple base class for all of the distributed workers in PC2L. The
 * workers are typically run on processes whose MPI-rank is not
 * zero. The workers collaborate with the manager (process with
 * rank-zero) to handle various distributed operations.
 */
class Worker {
public:
  /**
   * The required polymorphic destructor for the worker class.
   */
  virtual ~Worker() {}

  /**
   * Perform any initialization operations that are needed for the
   * worker, for a given run of PC2L system.  This method is called
   * once, before the \c run method in this class is called.  The
   * base class method does nothing.
   *
   * \note If the user decides to repeat operations (i.e., perform
   * multiple sequential runs) then this method is called once for
   * each of the runs.
   */
  virtual void initialize() {}

  /**
   * This is the primary method of a worker.  This method is
   * overridden in the derived class to keep the worker running
   * until the manager process sends a message to stop the worker.
   * The default implementation in this class just throws an
   * exception when this method is called.
   *
   * \note If the user decides to repeat operations (i.e., perform
   * multiple sequential runs) then this method is called once for
   * each of the runs.
   */
  virtual void run();

  /**
   * Perform any finalization operations that are needed for the
   * worker, for a given run of PC2L system.  This method is called
   * once, after the \c run method in this class is called.  The
   * base class method does nothing.
   *
   * \note If the user decides to repeat operations (i.e., perform
   * multiple sequential runs) then this method is called once for
   * each of the runs.
   */
  virtual void finalize() {}

  /**
   * Helper method to send a message (binary blob) to a given
   * destination process.
   *
   * \param[in] msgPtr Pointer to the message to be sent. If there isn't a
   * message in he supplied pointer then this method does not perform any
   * operations.
   *
   * \param[in] destRank The destination rank to where the message
   * is to be sent.
   */
  void send(MessagePtr msgPtr, const int destRank = 0);

  /**
   * Waits on a request to come back then returns pointer to data with result
   * @param req MPI_Request to wait on
   * @return resulting message
   */
  MessagePtr wait(MPI_Request req);

  /**
   * Helper method to receive a message (binary blob), optionaly
   * from a given source-rank.
   *
   * \param[in] srcRank The rank from where a message must be
   * received.  If this parameter is \c MPI_ANY_SOURCE, then the
   * first revived message is typically returned.
   *
   * \param[in] tag Only read a message with a specific tag.  If
   * this tag is \c MPI_ANY_TAG then messages are read ignoring the
   * their tag values.
   *
   * \return The message received, if any.
   */
  MessagePtr recv(const int srcRank = MPI_ANY_SOURCE,
                  const int tag = MPI_ANY_TAG);

  /**
   * Helper method to receive a message (binary blob),
   * from a given source-rank in a non blocking fashion
   * This initiates the non-blocking receive. The wait
   * method ensures that it has concluded, and it must
   * be passed the MPI_Request from this method
   *
   * \param[in] srcRank The rank from where a message must be
   * received.  If this parameter is \c MPI_ANY_SOURCE, then the
   * first revived message is typically returned.
   *
   * \param[in] tag Only read a message with a specific tag.  If
   * this tag is \c MPI_ANY_TAG then messages are read ignoring the
   * their tag values.
   *
   * \return Request resulting from recv
   */
  MPI_Request startReceiveNonblocking(const int srcRank = MPI_ANY_SOURCE,
                                      const int tag = MPI_ANY_TAG);

protected:
  /**
   * The default constructor is made protected to ensure that this
   * class is never directly instantiated.  Instead, instantiate and
   * work with one of the derived classes.
   *
   */
  Worker() {}

private:
  /**
   * A reused buffer that is used to receive messages. This buffer
   * is reused to minimize the memory allocation overheads and
   * possibly improve CPU-caching.
   *
   */
  std::vector<char> recvBuf;
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
