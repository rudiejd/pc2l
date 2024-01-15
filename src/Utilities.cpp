#ifndef UTILITIES_CPP
#define UTILITIES_CPP

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
// Authors:   Dhananjai M. Rao, JD Rudie          {raodm, rudiejd}@miamioh.edu
//---------------------------------------------------------------------

#include "Utilities.h"
#include <sys/types.h>
#include <sys/stat.h>

char*
getTimeStamp(const char *fileName, char *buffer) {
    if (fileName == NULL) {
        // Nothing further to be done here.
        return buffer;
    }
    // The follwing structure will contain the file information.
    struct stat fileInfo;
    int returnValue = 0;
#ifdef _WINDOWS
    returnValue = _stat(fileName, &fileInfo);
#else
    // The only difference between windows and Linux is the extra "_"
    // at the beginning of stat() method.
    returnValue = stat(fileName, &fileInfo);
#endif
    // Check to ensure there were no errors.  If there were errors
    // exit immediately.
    if (returnValue == -1) {
        // O!o! there was an error.
        return buffer;
    }
    // Convert the last modification time to string and return it back
    // to the caller.
    return getSystemTime(buffer, &fileInfo.st_mtime);
}

// Returns a date as in -- "Wed Jun 30 21:49:08 1993"
char* getSystemTime(char *buffer, const time_t *encodedTime) {
    if (buffer == NULL) {
        // Nothing more to do.
        return NULL;
    }
    // Get instant system time.
    time_t timeToConv = time(NULL);
    if (encodedTime != NULL) {
        // If we have a valid time supplied, then override system time.
        timeToConv = *encodedTime;
    }
    // Convert the time.
    ctime_s(buffer, 128, &timeToConv);
    // Return the buffer back to the caller
    return buffer;
}

#endif
