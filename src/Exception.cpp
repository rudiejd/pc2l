#ifndef EXCEPTION_CPP
#define EXCEPTION_CPP

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
// Authors:    JD Rudie          rudiejd@miamioh.edu
//---------------------------------------------------------------------

#include "Exception.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <stdexcept>

// namespace pc2l {
BEGIN_NAMESPACE (pc2l);

// The constructor that creates the full error message to be stored in
// this object.
Exception::Exception (const std::string &errorMessage,
                      const std::string &suggestion,
                      const std::string &fileName, const int lineNumber, ...)
    : fileName (fileName), lineNumber (lineNumber)
{
  // We create a temporary buffer to format any format strings in
  // the error message. We use at least a 512 byte buffer. However,
  // if the errorMessage is longer then we use a longer buffer.
  const int msgSize
      = std::max ({ 512UL, errorMessage.size () * 2, suggestion.size () * 2 });
  // A temporary buffer to format the errorMessage.
  std::string buffer (msgSize, 0);
  // buffer.reserve(msgSize);

  // Format the error message and suggestion in case they have any
  // format strings.  Need to handle variadic parameters here.
  std::va_list args;
  va_start (args, lineNumber);

  // Format the error message first
  vsnprintf (&buffer[0], buffer.size (), errorMessage.c_str (), args);
  this->message = buffer.c_str (); // store in instance variable.

  // Next, format the suggestion message
  vsnprintf (&buffer[0], buffer.size (), suggestion.c_str (), args);
  this->suggestion = buffer.c_str (); // store in instance variable.

  // Wrap-up variadic arguments.
  va_end (args);

  // Update our full exception message to be used when the what()
  // method is called.
  fullInfo = "pc2l::Exception: " + message + ".\n\tSuggestion: " + suggestion;
  if (!fileName.empty ())
    {
      // Add file information as it is available.
      fullInfo += "\n\tAt: " + fileName
                  + "(line: " + std::to_string (lineNumber) + ")";
    }
}

std::ostream &
operator<< (std::ostream &os, const pc2l::Exception &exp)
{
  return (os << exp.what ());
}

END_NAMESPACE (pc2l);
// }   // end namespace pc2l

#endif
