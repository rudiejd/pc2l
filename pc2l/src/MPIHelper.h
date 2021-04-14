#ifndef MPI_HELPER_H
#define MPI_HELPER_H

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

/** \file MPIHelper.h

    \brief File and macros to enable conditional compilation of MPI
    code so that MUSE can be built with and without MPI.

    This header file has been introduced to permit MUSE to compile
    with and without the presence of MPI. The code is designed in such
    a way to enable the use of MPI without hurting the readability and
    development of MUSE.
*/

#include "Utilities.h"
#include <string>

#ifndef _WINDOWS
#include "config.h"
#endif

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

#ifdef HAVE_LIBMPI

// We have MPI. In this case the standard/default operation of the
// system is good to go. Supress unused parameter warnings under GCC
// just for this header.

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#define OMPI_SKIP_MPICXX 1
#include <mpi.h>

#ifdef __GNUC__
#pragma GCC diagnostic warning "-Wunused-parameter"
#endif

#endif

/** \def MPI_GET_RANK

    \brief A simple, convenience macro to conditionally call
    MPI_Comm_rank and return the rank.

    <p>This macro provides a convenient wrapper around MPI to provide
    functionality similar to MPI::COMM_WORD.Get_rank().  If MPI is
    available/enabled, this macro reduces to calling the actual
    MPI_Comm_rank() function. However, if MPI is not available, this
    macro reduces to the constant 0 (one) making it appear as if there
    is only one process to work with. </p>
   
    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	int myRank = MPI_GET_RANK();
        // ... more code goes here ..
    }

    \endcode
*/
#ifdef HAVE_LIBMPI
// We have MPI enabled
inline int MPI_GET_RANK() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
}
#else
// We don't have MPI
#define MPI_GET_RANK() 0
#endif

/** \def MPI_GET_SIZE

    \brief A simple, convenience macro to conditionally call
    MPI_Comm_size() function.

    <p>This macro provides a convenient wrapper around MPI to provide
    functionality similar to MPI::COMM_WORD.Get_size(). If MPI is
    available/enabled, this macro reduces to calling the actual
    MPI_Comm_size() method. However, if MPI is not available, this
    macro reduces to the constant 1 (one) making it appear as if there
    is only one process to work with. </p>
   
    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	int workerCount = MPI_GET_SIZE();
        // ... more code goes here ..
    }

    \endcode
*/
#ifdef HAVE_LIBMPI
// We have MPI enabled
inline int MPI_GET_SIZE() {
    int size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return size;
}
#else
// We don't have MPI
#define MPI_GET_SIZE() 1
#endif

#ifndef HAVE_LIBMPI
/** \def MPI_ANY_SOURCE

    \brief If MPI is not available, this macro defines a dummy
    MPI_ANY_SOURCE.
*/
#define MPI_ANY_SOURCE -1

/** \def MPI_ANY_TAG

    \brief If MPI is not available, this macro defines a dummy
    MPI_ANY_TAG
*/
#define MPI_ANY_TAG -1

#endif

/** \brief A simple, convenience macro to conditionally provide a
    suitable definition for MPI::Status data structure depending on
    whether MPI is available (or not).

    <p>This macro provides a convenient mechanism to work with
    MPI::Status data structure.  If MPI is available, then it defaults
    to MPI::Status.  On the other hand, if MPI is disabled then this
    macro provides a suitable definition so that the core code base
    does not get cluttered with unnecessary logic.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
		MPI_STATUS msgInfo;
        MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        // ... more code goes here ..
    }

    \endcode
*/
#ifdef HAVE_LIBMPI
/** \def MPI_STATUS

    \brief Compile time macro to map to MPI_Status
*/
#define MPI_STATUS MPI_Status
#else
// We don't have MPI. So provide a suitable definition for MPI_Status
/** A custom class to mimic the operation of MPI_Status when MPI
    library is not available. */
class MPI_STATUS {
public:
    /// The source rank.  This value is always zero whem MPI is unavailable.
    int MPI_SOURCE = 0;
    /// The message tag.  This value is always zero whem MPI is unavailable.
    int MPI_TAG    = 0;
};
#endif

/** \def CONST_EXP

    \brief A simple, convenience macro to conditionally enable/disable
    constant MPI exceptions depending on whether MPI::Exception class
    supports a constant Get_error_string() method.

    Open-MPI has a constant version of the method while mavaphic does
    not.  This macro works around this different by selectively
    permiting the use of const keyword when catching MPI::Exception.

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        try {
            MPI_SEND(data, msg->getSize(), MPI::CHAR, destRank, GVT_MESSAGE);
        } catch (CONST_EXP MPI_EXCEPTION& e) {
            std::cerr << "MPI ERROR (sendMessage): ";
            std::cerr << e.Get_error_string() << std::endl;
        }
        // ... more code goes here ..
    }

    \endcode
*/
#if USE_CONST_MPI_EXP == 1
#define CONST_EXP const
#else
#define CONST_EXP
#endif

/** \brief A simple, convenience exception class to throw exceptions
    when errors are encountered.

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        try {
            MPI_SEND(data, msg->getSize(), MPI::CHAR, destRank, GVT_MESSAGE);
        } catch (MPI_CONST MPI_EXCEPTION& e) {
            std::cerr << "MPI ERROR (sendMessage): ";
            std::cerr << e.Get_error_string() << std::endl;
        }
        // ... more code goes here ..
    }

    \endcode
*/
class MPI_EXCEPTION {
public:
    std::string Get_error_string() const { return "MPI error occurred"; }
};

// Ensure that MPI_CONST is also defined
/** \def MPI_CONST */
#define MPI_CONST const

/** \def MPI_TYPE_UNSIGNED

    \brief Macro to map MPI_TYPE_UNSIGNED to MPI::UNSIGNED (if MPI is
    enabled) or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::UNSIGNED enumerated constant. If MPI is
    available, then MPI_TYPE_UNSIGNED defaults to MPI::UNSIGNED.  On
    the other hand, if MPI is disabled then this macro simply reduces
    to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	MPI_STATUS msgInfo;
        MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        const int dataSize = msgInfo.Get_count(MPI_TYPE_UNSIGNED);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_TYPE_UNSIGNED MPI_UNSIGNED
#else
// MPI is not available
#define MPI_TYPE_UNSIGNED 0
#endif

/** \def MPI_TYPE_INT

    \brief Macro to map MPI_TYPE_INT to MPI::INT (if MPI is enabled)
    or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::INT enumerated constant. If MPI is available,
    then MPI_TYPE_INT defaults to MPI::INT.  On the other hand, if MPI
    is disabled then this macro simply reduces to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	MPI_STATUS msgInfo;
        MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        const int dataSize = msgInfo.Get_count(MPI_TYPE_INT);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_TYPE_INT MPI_INT
#else
// MPI is not available
#define MPI_TYPE_INT 0
#endif

/** \def MPI_TYPE_CHAR

    \brief Macro to map MPI_TYPE_CHAR to MPI::CHAR (if MPI is enabled)
    or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::CHAR enumerated constant. If MPI is available,
    then MPI_TYPE_CHAR defaults to MPI::CHAR.  On the other hand, if
    MPI is disabled then this macro simply reduces to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	MPI_STATUS msgInfo;
        MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        const int dataSize = msgInfo.Get_count(MPI_TYPE_CHAR);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_TYPE_CHAR MPI_CHAR
#else
// MPI is not available
#define MPI_TYPE_CHAR 0
#endif

/** \def MPI_TYPE_2INT

    \brief Macro to map MPI_TYPE_2INT to MPI::TWOINT (if MPI is
    enabled) or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::TWOINT enumerated constant. If MPI is available,
    then MPI_TYPE_2INT defaults to MPI::TWOINT.  On the other hand, if
    MPI is disabled then this macro simply reduces to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	MPI_STATUS msgInfo;
        MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        const int dataSize = msgInfo.Get_count(MPI_TYPE_2INT);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_TYPE_2INT MPI_TWOINT
#else
// MPI is not available
#define MPI_TYPE_2INT 0
#endif

/** \def MPI_TYPE_DOUBLE

    \brief Macro to map MPI_TYPE_DOUBLE to MPI::DOUBLE (if MPI is
    enabled) or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::DOUBLE enumerated constant. If MPI is
    available, then MPI_TYPE_DOUBLE defaults to MPI::DOUBLE.  On
    the other hand, if MPI is disabled then this macro simply reduces
    to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
        // ... some code goes here ..
	MPI_STATUS msgInfo;
        MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        const int dataSize = msgInfo.Get_count(MPI_TYPE_DOUBLE);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_TYPE_DOUBLE MPI_DOUBLE
#else
// MPI is not available
#define MPI_TYPE_DOUBLE 0
#endif

/** \def MPI_OP_SUM

    \brief Macro to map MPI_OP_SUM to MPI::SUM (if MPI is enabled)
    or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::SUM enumerated constant. If MPI is available,
    then MPI_OP_SUM defaults to MPI::SUM.  On the other hand, if
    MPI is disabled then this macro simply reduces to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
	    // ... some code goes here ..
		int localCount = smList.size();
		int totalCount = 0;
		MPI_ALL_REDUCE(&localCount, &totalCount, 1, MPI_TYPE_INT, MPI_OP_SUM);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_OP_SUM MPI_SUM
#else
// MPI is not available
#define MPI_OP_SUM 0
#endif

/** \def MPI_OP_MAXLOC

    \brief Macro to map MPI_OP_MAXLOC to MPI::MAXLOC (if MPI is enabled)
    or 0 if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::MAXLOC enumerated constant. If MPI is available,
    then MPI_OP_MAXLOC defaults to MPI::MAXLOC.  On the other hand, if
    MPI is disabled then this macro simply reduces to 0.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    void someMethod() {
	    // ... some code goes here ..
		int localCount    = smList.size();
		int totalCount[2] = {0, 0};
		MPI_ALL_REDUCE(&localCount, &totalCount, 1, MPI_TYPE_2INT, MPI_OP_MAXLOC);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_OP_MAXLOC MPI_MAXLOC
#else
// MPI is not available
#define MPI_OP_MAXLOC 0
#endif

/** \def MPI_INIT(argc, argv)

    \brief Macro to map MPI_INIT to MPI::Init (if MPI is enabled) or
    an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI_Init function. If MPI is available, then MPI_INIT
    defaults to MPI_Init.  On the other hand, if MPI is disabled then
    this macro simply reduces to a blank method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
	MPI_INIT(argc, argv);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_INIT(argc, argv) MPI_Init(&argc, &argv)
#else
// MPI is not available
void MPI_INIT(int argc, char* argv[]);
#endif

/** \def MPI_FINALIZE

    \brief Macro to map MPI_FINALIZE to MPI::Finalize (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::Finalize method. If MPI is available, then
    MPI_FINALIZE defaults to MPI::Finalize.  On the other hand, if MPI
    is disabled then this macro simply reduces to a blank method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
	MPI_INIT(argc, argv);
        // ... more code goes here ..
	MPI_FINALIZE();
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_FINALIZE() MPI_Finalize()
#else
// MPI is not available
#define MPI_FINALIZE()
#endif

/** \def MPI_PROBE(src, tag, status)

    \brief Macro to map MPI_PROBE to MPI::COMM_WORLD.Probe (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::COMM_WORLD.Probe method. If MPI is available,
    then MPI_PROBE defaults to MPI::Probe.  On the other hand, if MPI
    is disabled then this macro simply reduces to a blank method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_PROBE(MPI_ANY_SOURCE, AGENT_LIST, status);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_PROBE(src, tag, status) MPI_Probe(src, tag, MPI_COMM_WORLD, &status)
#else
// MPI is not available
#define MPI_PROBE(src, tag, status)
#endif

/** \brief A function to map MPI_PROBE to similar functionality
    MPI::COMM_WORLD.IProbe (if MPI is enabled) or an empty method call
    if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI_IProbe function. If MPI is available, then
    MPI_IPROBE defaults to MPI::IProbe.  On the other hand, if MPI is
    disabled then this macro simply reduces to a blank method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_IPROBE(MPI_ANY_SOURCE, AGENT_LIST, status);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
inline bool MPI_IPROBE(int src, int tag, MPI_STATUS& status) {
    int flag = 0;
    MPI_Iprobe(src, tag, MPI_COMM_WORLD, &flag, &status);
    return (flag == 1);
}
#else
// MPI is not available
bool MPI_IPROBE(int src, int tag, MPI_STATUS status);
#endif

/** \def MPI_BCAST

    \brief Macro to map MPI_BCAST to MPI::COMM_WORLD.Bcast (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::COMM_WORLD.Bcast method. If MPI is available,
    then MPI_BCAST defaults to MPI::COMM_WORLD.Bcast.  On the other
    hand, if MPI is disabled then this macro simply reduces to a blank
    method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_BCAST(&flatAgentMapSize, 1, MPI_TYPE_INT, ROOT_KERNEL);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_BCAST(data, size, type, src) MPI_Bcast(data, size, type, \
                                                   src, MPI_COMM_WORLD)
#else
// MPI is not available
#define MPI_BCAST(data, size, type, src)
#endif

/** \def MPI_RECV

    \brief Macro to map MPI_RECV to MPI_Recv (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI_Recv method. If MPI is available, then MPI_BCAST
    defaults to MPI_Recv.  On the other hand, if MPI is disabled then
    this macro simply reduces to a blank method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_RECV(&agentList[0], agentListSize,
                                 MPI_TYPE_UNSIGNED, status.Get_source(),
                                 AGENT_LIST, status);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_RECV(data, count, type, rank, tag, status) \
    MPI_Recv(data, count, type, rank, tag, MPI_COMM_WORLD, &status)
#else
// MPI is not available
#define MPI_RECV(data, count, type, rank, tag, status)
#endif


/** \def MPI_SEND(data, count, type, rank, tag)

    \brief Macro to map MPI_SEND to MPI::COMM_WORLD.Send (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::COMM_WORLD.Send method. If MPI is available, then
    MPI_BCAST defaults to MPI::COMM_WORLD.Send.  On the other hand, if
    MPI is disabled then this macro simply reduces to a blank
    method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_SEND(&agentList[0], agentListSize, MPI_TYPE_UNSIGNED, ROOT_KERNEL,
                 AGENT_LIST);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_SEND(data, count, type, rank, tag) \
    MPI_Send(data, count, type, rank, tag, MPI_COMM_WORLD)
#else
// MPI is not available
int MPI_SEND(const void* data, int count, int type, int rank, int tag);
#endif

/** \def MPI_WTIME

    \brief Macro to map MPI_WTIME to MPI::Wtime (if MPI is enabled) or
    to a suitable OS-independent implementation for Wtime.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI::Wtime method. If MPI is available, then MPI_WTIME
    defaults to MPI::Wtime.  On the other hand, if MPI is disabled
    then this macro simply reduces to a suitable OS-independent method
    call.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
	const double startTime = MPI::Wtime();
        // ... more code goes here ..
	const double elapsedTime = (MPI::Wtime() - startTime) * 1000.0;
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_WTIME MPI_Wtime
#else
// MPI is not available
extern double MPI_WTIME();
#endif

/** \def MPI_CODE

    \brief Macro to work around warnings about unused MPI variables or
    conditionally compile-out MPI code.

    <p>This macro provides a convenient, conditionally defined macro
    to conditionally compile-out MPI related code when MPI is
    disabled.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
	MPI_CODE({
	    MPI_STATUS msgInfo;
            MPI_PROBE(sourceRank, REPOPULATE_REQUEST, msgInfo);
        });
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_CODE(x) x
#else
// MPI is not available
#define MPI_CODE(x)
#endif

/** \def MPI_BARRIER

	\brief Macro to map MPI_BARRIER to MPI_Barrier(MPI_COMM_WORLD)
	(if MPI is enabled) or an empty method call if MPI is
	unavailable.

	<p>This macro provides a convenient, conditionally defined
	macro to refer to MPI::Barrier method. If MPI is available,
	then MPI_BARRIER defaults to MPI_Barrier.  On the other hand,
	if MPI is disabled then this macro simply reduces to a blank
	method.</p>

	This macro can be used as shown below:

	\code

	#include "MPIHelper.h"

	int main(int argc, char *argv[]) {
	// ... some code goes here ..
	MPI_BARRIER();
	// ... more code goes here ..
	}
	\endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_BARRIER() MPI_Barrier(MPI_COMM_WORLD)
#else
// MPI is not available
#define MPI_BARRIER()
#endif

/** \def MPI_GET_COUNT

	\brief Macro to map MPI_GET_COUNT to MPI_get_count function
	call (if MPI is enabled) or an empty method call if MPI is
	unavailable.

	<p>This macro provides a convenient, conditionally defined
	macro to refer to MPI_get_count function and return the size
	of message. If MPI is available, then MPI_GET_COUNT defaults
	to MPI_get_count.  On the other hand, if MPI is disabled then
	this macro simply returns zero.</p>

	This macro can be used as shown below:

	\code

	#include "MPIHelper.h"

	int main(int argc, char *argv[]) {
	// ... some code goes here ..
        MPI_STATUS status;
	MPI_GET_COUNT(status, MPI_TYPE_UNSIGNED);
	// ... more code goes here ..
	}
	\endcode
*/
#ifdef HAVE_LIBMPI
inline int MPI_GET_COUNT(const MPI_STATUS& status, MPI_Datatype datatype) {
    int count = 0;
    MPI_Get_count(&status, datatype, &count);
    return count;
}
#else
// MPI is not available
#define MPI_GET_COUNT(status, datatype) 0
#endif

/** \def MPI_ALL_GATHER

    \brief Macro to map MPI_ALL_GATHER to MPI_Allgather (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI_ALL_GATHER method. If MPI is available, then
    MPI_ALL_GATHER defaults to MPI_Gather.  On the other hand, if MPI
    is disabled then this macro simply reduces to a blank method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_ALL_GATHER(sendBuf, 1, MPI_INT, recvBuf, 1, MPI_INT);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_ALL_GATHER(sendbuf, sendcount, sendtype, recvbuf,\
                       recvcount, recvtype)                  \
    MPI_Allgather(sendbuf, sendcount, sendtype,              \
                  recvbuf, recvcount, recvtype, MPI_COMM_WORLD)
#else
// MPI is not available
#define MPI_ALL_GATHER(sendbuf, sendcount, sendtype, recvbuf, recvcount, \
                       recvtype)
#endif

/** \def MPI_ALL_GATHERV

    \brief Macro to map MPI_ALL_GATHERV to MPI_Allgatherv (if MPI is
    enabled) or an empty method call if MPI is unavailable.

    <p>This macro provides a convenient, conditionally defined macro
    to refer to MPI_Allgatherv method. If MPI is available, then
    MPI_ALL_GATHERV defaults to MPI_Gatherv.  On the other hand, if
    MPI is disabled then this macro simply reduces to a blank
    method.</p>

    This macro can be used as shown below:

    \code

    #include "MPIHelper.h"

    int main(int argc, char *argv[]) {
        // ... some code goes here ..
        MPI_ALL_GATHERV(sendBuf, 1, MPI_INT, recvBuf, recvCount,
                        displs, MPI_INT);
        // ... more code goes here ..
    }
    \endcode
*/
#ifdef HAVE_LIBMPI
#define MPI_ALL_GATHERV(sendbuf, sendcount, sendtype, recvbuf,   \
                        recvcounts, displs, recvtype)            \
    MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf,        \
                   recvcounts, displs, recvtype, MPI_COMM_WORLD)
#else
// MPI is not available
#define MPI_ALL_GATHERV(sendbuf, sendcount, sendtype, recvbuf,   \
                        recvcounts, displs, recvtype)
#endif

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
