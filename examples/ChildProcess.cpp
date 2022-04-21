#ifndef CHILD_PROCESS_CPP
#define CHILD_PROCESS_CPP

/**
 * This source file contains the implementation for the various
 * methods defined in the ChildProcess class.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */

// All the necessary #includes are already here
#include "ChildProcess.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

// Named-constants to keep pipe code readable below
const int READ = 0, WRITE = 1;


/** NOTE: Unlike Java, C++ does not require class names and file names
 * should match.  Hence when defining methods pertaining to a specific
 * class, the class name should be prefixed with each method -- as in
 * "ChildProcess::"
 */


// This method is just a copy-paste from lecture notes. This is done
// to illustrate an example.
void
ChildProcess::myExec(StrVec argList) {
    std::vector<char*> args;    // list of pointers to args
    for (auto& s : argList) {
        args.push_back(&s[0]);  // address of 1st character
    }
    // nullptr is very important
    args.push_back(nullptr);
    // Make execvp system call to run desired process
    execvp(args[0], &args[0]);
    // In case execvp ever fails, we throw a runtime execption
    throw std::runtime_error("Call to execvp failed for: " + argList[0]);
}

// Implement the constructor
ChildProcess::ChildProcess() : childPid(-1), childOutput(&pipeBuf) {
    // childPid is initialized and not assigned!  Hence body is empty.
}

// Implement the destructor.  The destructor is an empty method
// because this class does not have any resources to release.
ChildProcess::~ChildProcess() {
    // Maybe it is a good idea to call wait() method here to ensure
    // that the child process does not get killed? On the other hand,
    // if we call wait() here and the child process misbehaves, then
    // it will appear as if this program is buggy -- Awwww.. -- no
    // good choice.  In this situation, a good API is to leave it to
    // the user to decide what is the best course of action.
}

StrVec
ChildProcess::split(const std::string& words) {
    StrVec retVal;  // The list of words to be returned.
    std::istringstream is(words);   // Stream to split words

    // Extract words while honoring quotes
    for (std::string word; is >> std::quoted(word);) {
        retVal.push_back(word);  // add words to the vector.
    }
    return retVal;  // return the list of words
}

// Use the comments in the header to implement the forkNexec method.
// This is a relatively simple method with an if-statement to call
// myExec in the child process and just return the childPid in parent.
int
ChildProcess::forkNexec(const StrVec& argList) {
    // Fork and save the pid of the child process
    childPid = fork();
    // Call the myExec helper method in the child
    if (childPid == 0) {
        // We are in the child process
        myExec(argList);
    }
    // Control drops here only in the parent process!
    return childPid;
}

// Use the comments in the header to implement the wait method.  This
// is a relatively simple method which uses waitpid call to get
// exitCode as shown in Slide #6 of ForkAndExec.pdf
int
ChildProcess::wait() const {
    int exitCode = 0;  // Child process's exit code
    waitpid(childPid, &exitCode, 0);  // wait for child to finish
    return exitCode;
}

// Method to first redirect output of child process via a pipe. Then
// this method runs the specified program in the child process. The
// output of the cild process can be read in the parent process via
// the childProcess stream.
int
ChildProcess::forkNexecIO(const StrVec& argList) {
    int pipefd[2];  // The pipe file descriptors
    pipe(pipefd);   // Make system call to get pipe file descriptors

    // Fork and save the pid of the child process.
    childPid = fork();

    // Appropriately tie the I/O streams of the parent and child processes.
    if (childPid == 0) {
        // In the child process
        close(pipefd[READ]);     // Close unused end (in child)
        dup2(pipefd[WRITE], 1);  // Tie/redirect std::cout of command
        myExec(argList);         // Run a different program
    }

    // When control drops here we are in the parent process.  In the
    // parent process. Wrap the pipe's end into a buffer so we can
    // read the output of our child process.
    pipeBuf = {pipefd[READ], std::ios::in, sizeof(char)};
    // Note the above pipeBuf is already set to be used by
    // childOutput stream in the constructor of ChildProcess
    
    // Close the unused end of the pipe in the parent process.
    close(pipefd[WRITE]);

    // Return the child's pid in the parent.
    return childPid;
}

#endif
