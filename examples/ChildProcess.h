#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

/**
 * This source file contains the definition for the ChildProcess
 * class.  This class provides a convenient API to fork and execute
 * other programs.
 *
 * Copyright (C) 2020 raodm@miamiOH.edu
 */

#include <ext/stdio_filebuf.h>
#include <iostream>
#include <string>
#include <vector>

// A convenience shortcut to a vector-of-strings
using StrVec = std::vector<std::string>;

// ------------------------------------------------------------------- //
// ****  NOTE: NEVER NEVER put "using namespace" IN A HEADER FILE  *** //
// ------------------------------------------------------------------- //

/**
 * A simple class to help with forking & executing programs in a child
 * process.  This class maintains the PID of the child process in an
 * instance variable.
 */
class ChildProcess {
public:
    /** A simple default (no-argument) constructor.  This constructor
        merely initializes (so the body of the method should be
        empty!) the childPid instance variable to -1.
    */
    ChildProcess();
    
    /** The destructor. This method cleans-up any resources (like open
        files etc.). However, this class is very simple and the
        destructor is just an empty method.
        
        Note: Destructor for an object is called when the object goes
        out of scope.
    */
    ~ChildProcess();

    /**
     * Convenience method to split a given string into a vector of
     * words.  The split is done based on blank spaces.  This method
     * honors quotes in the string using the std::quoted method.
     *
     * \param[in] words A string containing the words to be split.
     *
     * \return A list of words in the string returned as a
     * std::vector<std::string>.
     */
    static StrVec split(const std::string& words);
    
    /**
     * This method creates an process with output of child process
     * redirected using a pipe.
     *
     *
     * \param[in] argList The list of command-line arguments.  The
     *   first entry is assumed to be the command to be executed. 
     */
    int forkNexecIO(const StrVec& argList);

    /** The primary method in this class that:

        1. First uses the fork system call to create a child process.
        2. In the child process it calls myExec to execute a program.
        3. In the parent process, it stores the value in childPid and
           returns the childPid value.

        \param[in] argList The list of command-line arguments.  The
        first entry is assumed to be the command to be executed. 
          
        \return This method returns the pid value of the child process
        forked by this method.
    */
    int forkNexec(const StrVec& argList);

    /** Helper method to wait for child process to finish.  This
        method calls the waitpid system call. It obtains the exit code
        of the child process from the 2nd argument of the waitpid
        system call.

        \return This method returns the exit code of the child
        process.
    */
    int wait() const;

    /**
     * Get the stream from where the child-process's outputs can be
     * read in the parent process.  The stream returned by this method
     * is meaningful only after the forkNexecIO() method has been
     * called in the parent process.
     *
     * \note First call forkNexecIO() before using the stream returned
     * by this method.
     *
     * \return The stream from where the child-process's outputs can
     * be read.
     */
    std::istream& getChildOutput() { return childOutput; }
    
protected:
    /** A helper method to setup pointers and call execvp system call.
        This method should be called from a child process.  Don't call
        this method directly.  Instead, call the forkNexec API method.

        NOTE: This method is a copy-paste from the lecture slides.
        
        \param[in] argList The list of command-line arguments.  The
        first entry is assumed to be the command to be executed.
    */
    void myExec(StrVec argList);

private:
    /** The only instance variable in this class.  It is initialized
        to -1 in the constructor.  The value is changed by the
        forkNexec method.
    */
    int childPid;

    /**
     * A wrapper class that is needed to convert a pipe handle (which
     * is an integer) to an std::istream so that we can conveniently
     * read data.  This buffer is used to initialize the childOutput
     * stream below.
     */
    __gnu_cxx::stdio_filebuf<char> pipeBuf;

    /**
     * This is the stream from where the child-process's outputs can
     * be read in the parent-process.  This stream wraps the above
     * pipeBuf so that we can perform standard operations using
     * operator>>, std::quoted, or std::getline.
     */
    std::istream childOutput;
};

#endif
