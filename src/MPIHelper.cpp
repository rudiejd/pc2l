#ifndef MPI_HELPER_CPP
#define MPI_HELPER_CPP

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

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

#ifndef MPI_FOUND

#ifndef _WINDOWS
// A simple implementation for MPI_WTIME on linux
#include <sys/time.h>
double MPI_WTIME() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1e6);
}

#else
// A simple implementation for MPI_WTIME on Windows
#include <windows.h>

double MPI_WTIME() {
    FILETIME st;
    GetSystemTimeAsFileTime(&st);
    long long time = st.dwHighDateTime;
    time <<= 32;
    time |= st.dwLowDateTime;
    return (double) time;
}


#endif  // _Windows

// Dummy MPI_INIT when we don't have MPI to keep code base streamlined
void MPI_INIT(int argc, char* argv[]) {
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
}

bool MPI_IPROBE(int src, int tag, MPI_STATUS status) {
    UNUSED_PARAM(src);
    UNUSED_PARAM(tag);
    UNUSED_PARAM(status);
    return false;
}

int MPI_SEND(const void* data, int count, int type, int rank, int tag) {
    UNUSED_PARAM(data);
    UNUSED_PARAM(count);
    UNUSED_PARAM(type);
    UNUSED_PARAM(rank);
    UNUSED_PARAM(tag);
    return -1;
}

#endif  // Don't have MPI

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif // MPI_HELPER_CPP
