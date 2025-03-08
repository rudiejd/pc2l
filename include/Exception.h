#ifndef EXCEPTION_H
#define EXCEPTION_H

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
#include <iostream>
#include <stdexcept>

// namespace pc2l {
BEGIN_NAMESPACE (pc2l);

/** \file Exception.h

    \brief Class and macros for generating a pc2l::Exception

    This file provides the definition for a custom exception class
    that is used by the various methods and classes in a general
    purpose way to throw suitable exceptions when error conditions
    occur.  The pc2l::Exception has been designed to include suitable
    information about an exception along with suggestion.  However,
    some of the information may not be useful/suitable for reporting
    exceptions to end users or should not be placed in release
    versions.  Consequently, such information can be effectively
    compiled out.  To ease uniform generation of exceptions this file
    also defines an PC2L_EXP macro that must be used for throwing an
    exception.  The working of the macros are controlled by compiler
    flags which automatically include or exclude additional debugging
    information.  Refer to the documentation on the macros and the
    class for further details.
*/

/** A general purpose Exception class that may be readily reused.

    This class defines a generic exception that may be thrown by
    different classes and methods in PC2L.  The \c pc2l::Exception
    class has been defined to encapsulate additional information
    regarding an exception thrown.  The objective is to provide a
    reasonably-detailed error message to aid in troubleshooting and
    debugging the source code.  The class provides various member
    functions that can be used to access the various pieces of
    information stored in the exception class, such as:

    <UL> <li> The error code associated with the exception (must be
        non-zero). </li>

        <li> The error string for the exception (maybe empty). </li>

        <li> Any suggestions to recover from the error (maybe empty). </li>

        <li> Source file name (empty is not available). </li>

        <li> Line number in source file (zero if not available). </li>
    </UL>

    <p>The exceptions thrown by all methods constituting the
    implementation of a class may extend this class for further
    customization.  Note that the \c pc2l::Exception class is fully
    self sufficient and can be directly used for generating
    exceptions.  However, if need for further customization is
    desired, then a new exception sub-class can be created.</p>

    <p>Depending on the type of environment being developed (\em i.e.,
    for release or internal debugging) the pc2l::Exception can include
    additional information (such as source code file name and line
    number) to aid in debugging.  To ease generation of such
    exceptions, a EXP macro has been defined, that suitably creates a
    new \c pc2l::Exception class using suitable parameters.</p>

    \note When throwing a \c pcl2::Exception always use the \c PC2L_EXP
    macro defined in this header file.  Here is a short example of how
    to use the \c PC2L_EXP macro:

    \code
    throw PC2L_EXP("Invalid file: %s", "Check command-line arguments", path);
    \endcode
*/
class Exception : public std::exception
{
  friend std::ostream &operator<< (std::ostream &, const pc2l::Exception &);

public:
  /** The default and only constructor.

      <p>The \c pc2l::Exception class has been defined to
      encapsulate as much information as possible regarding an
      exception thrown.  The objective is to provide a detailed
      error message to aid in troubleshooting and debugging the
      source code.</p>

      <p>All the information regarding an exception must be provided
      as a part of the constructor.  The primary objective is to
      eliminate any unwanted side effects that may arise when
      creating the exception (and the members of the exception are
      all constant objects) and to avoid uncontrolled changes to an
      exception once it has been created.</p>

      \param errorMessage The error message string. This can be a
      printf style format string to ease printing variables/values
      in exception error messages. Ideally, this should use a
      std::format like approach, but we are using a more traditional
      printf style appraoch for backwards compatibility.

      \param suggestion A text message suggesting how to avoid this
      exception. This can also be a format string. Note that the
      same variadic arguments are passed to both errorMessage and
      suggestion.

      \param fileName An \em optional source code fileName.

      \param lineNumber An \em optional source code line number.
  */
  Exception (const std::string &errorMessage,
             const std::string &suggestion = "",
             const std::string &fileName = "", const int lineNumber = 0, ...);

  /** Obtain the error message for this exception.

      This method must be used to obtain the error message
      associated with this exception.  The error message is
      represented as C string ('\0' terminted array of characters)
      containing an ASCII text description of the error.

      \return The error message associated with the exception, if
      any
  */
  const std::string &
  getErrorMessage () const noexcept
  {
    return message;
  }

  /** Obtain the help or suggestion text for this exception.

      This method must be used to access any help or suggestion
      messages associated with the exception.  The error message is
      represented as C string ('\0' terminted array of characters)
      containing an ASCII text that provides tips or suggestions on
      how to avoid the exception being thrown.  Note that the
      suggestion is optional and not all exceptions may include a
      suggestion.

      \return The suggestion associated with the exception (never NULL).
  */
  const std::string &
  getSuggestion () const noexcept
  {
    return suggestion;
  }

  /** Source file name for the exception (only Debug version).

      This method maybe used to determine the source code file name
      from where this exception was thrown.  The source file name is
      filled in only in the debug versions.  In the non-debug
      versions, the value returned by this method is an empty string
      (@a i.e., "\0") but not NULL.

      \return An optional source file name for the exception.
  */
  const std::string &
  getFileName () const noexcept
  {
    return fileName;
  }

  /** Obtain the source code line number (only Debug version).

      This method maybe used to determine the source code line
      number from where this exception was thrown.  The source code
      line number is filled in only for the dbeug version.  In the
      non-debug versions, this method returns 0 (zero).

      \return The optional source file line number.
  */
  int
  getLineNumber () const noexcept
  {
    return lineNumber;
  }

  /** The destructor.

      The pc2l::Exception class does not allocate any memory or
      perform signficiant operations to avoid auxiliary exceptions
      from being generated.  Consequently, the destructor has no
      tasks to perform.  It is here just to adhere to the standard
      convention of having a constructor-destructor pair for every
      class.
  */
  ~Exception () {}

  /**
     Overrides the default base class method to return the error
     message associated with this exception.

     \return The error message associated with this exception.  This
     method never returns nullptr.
  */
  virtual const char *
  what () const noexcept override
  {
    return fullInfo.c_str ();
  }

protected:
  // Currently this class has no protected members.

private:
  /** This member contains a detailed string description for the
      given exception.  Some exceptions may not have an error
      message associated with them.  However, it is \em strongly
      recommended that every exception have a error message
      associated with it
  */
  std::string message;

  /** The objective of this member is to provide a brief text (ASCII
      string) message providing some feedback or suggestion on how
      to avoid (future exceptions) or recover from the exception.
      This is an optional piece of information that maybe omitted.
  */
  std::string suggestion;

  /** This string contains the full information to be printed for
      this exception.  This string is stored here as a convenience
      to use in the \c what() method in this class.
   */
  std::string fullInfo;

  /** This member contains the source code file name from which this
      exception was generated.  Note that this information is
      optional and may be omitted in certain versions (such as
      release versions).
  */
  std::string fileName;

  /** This member contains the line number in the sourceCode
      (specified by the fileName member) from which the exception
      was generated.  If no line number information was provided (as
      in the case of release binaries) this member is set to 0
      (zero).
  */
  int lineNumber = 0;
};

/**
* @brief  Insertion operator to stream exception information to a given
    output stream.  This method provides a convenient mechanism to
    dump the complete exception information for debugging purposes.
*
* @param os  The output stream to which the formatted exception
    information is to be written.
* @param exp @c Exception to be inserted into stream
* @return Returns os stream with exception contents
*/
extern std::ostream &operator<< (std::ostream &os, const pc2l::Exception &exp);

END_NAMESPACE (pc2l);
// }   // end namespace pc2l

/** \def PC2L_EXP(errorMessage, suggestion, ...)

    \brief Macro to ease throwing a pc2l::Exception.

    This macro provides a convenient mechanism to create a pc2l::Exception,
    depending on whether the DEVELOPER_ASSERTIONS flag has been turned
    on or off.  In case the DEVELOPER_ASSERTIONS flag has been
    specified, then this macro automatically tags the source code file
    name and source code line number from where the exception was
    raised.  This makes debugging much faster.
*/

#ifdef DEVELOPER_ASSERTIONS

#define PC2L_EXP(errorMessage, suggestion, ...)                               \
  pc2l::Exception ((errorMessage), (suggestion), __FILE__, __LINE__,          \
                   ##__VA_ARGS__)

#else
// Use dummy values for file and line number (in non-debug mode)
#define PC2L_EXP(errorMessage, suggestion, ...)                               \
  pc2l::Exception ((errorMessage), (suggestion), "", 0, ##__VA_ARGS__)

#endif // DEVELOPER_ASSERTIONS

#endif
