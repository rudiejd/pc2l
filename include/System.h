#ifndef SYSTEM_H
#define SYSTEM_H

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
 * @file System.h
 * @author Dhananjai M. Rao (raodm@miamioh.edu)
 * @brief  Definition of our top-level System class that encapsulates
 * system wide data.
 * @version 0.1
 * @date 2021-04-23
 */

// namespace pc2l {
BEGIN_NAMESPACE(pc2l);

/**
 * A top-level class to encapsulate system-wide data and
 * properties. This class includes information that is common to many
 * of the data structures and alogrithms in PC2L.  It also
 * encapsulates any caches that are shared between multiple data
 * structures.  Note that each data structure has its own caches (to
 * fine tune to their specific access patterns).
 *
 * This class is currently meant to be used as a singleton class,
 * (without having the need to create an instance of this class). You
 * can get the process-wide instance as suggested below:
 *
 * \code
 * auto& pc2l = pc2l::System::get();
 * \endcode
 */
class System {
public:
    /**
     * Enumeration to define the global operation mode for a specific
     * run of PC2L.  Currently, the library only supports a single
     * node that reads/write data from a distributed cache.
     */
    enum OpMode : int {
         OneWriter_DistributedCache = 1, /**< single node that
                                            reads/writes with rest of
                                            the nodes serving as
                                            caches. */
         InvalidMode /**< Just a placeholder */
    };

    /**
     * Obtain a reference to the process-wide unique (singleton)
     * instance of the PC2L system object for further use.
     */
    static System& get() noexcept {
        return system;
    }
    
    /**
     * This method must be invoked prior to performing any operations
     * with the PC2L library.  This method is typically called at the
     * beginning of the program's main() function as suggested below:
     *
     * \code
     * #include "pc2l.h"
     * int main(int argc, char *argv[]) {
     *     auto& pc2l = pc2l::System::get();
     *     pc2l.initialize(argc, argv[]);
     *     pc2l.start();
     *     // Create and use PC2L data structures & algorithms.
     *     pc2l.stop();
     *     pc2l::System::finalize();     
     * }
     * \endcode
     *
     * This method uses the supplied command-line arguments to
     * initialize the distributed infrastructure of PC2L. If, MPI is
     * being used, this method initializes MPI.
     *
     * \param[in,out] argc The number of command-line arguments.  This
     * method uses the command-line arguments to suitably initialize
     * itself. Any arguments used by this method are removed from the
     * list of command-line arguments.
     *
     * \param[in,out] argv The actual command-line arguments.
     *
     * \param[in] initMPI If this flag is true then MPI is
     * initialized.  This flag is perset to provide users with finer
     * control on initialization if they need (i.e., maybe users want
     * to separately initialize MPI) or repeatedly start/stop a PC2L
     * system.
     *
     * \exception std::exception If any errors occur during
     * initialization then this method throws an exception.
     */
    void initialize(int& argc, char *argv[],
                    bool initMPI = true);

    /**
     * This is a method to actually start the operations of PC2L
     * system in a specific mode of operation.  This method should be
     * called before the PC2L data structures are used for further
     * operations.
     *
     * \param[in] mode The gloabl operation mode to be used by pc2l
     * for this run.  The default value is OneWriter_DistributedCache;
     */
    void start(const OpMode mode = pc2l::System::OneWriter_DistributedCache);

    /**
     * This method can be be used to shutdown the PC2L cache and
     * algorithm operations.
     */
    void stop();
    
    /**
     * This method must be invoked to wind-down the distributed
     * infrastructure of pc2l.  This method is typically called at the
     * end of the program's main() function as suggested below:
     *
     * \code
     * #include "pc2l.h"
     * int main(int argc, char *argv[]) {
     *     auto& pc2l = pc2l::System::get();
     *     pc2l.initialize(argc, argv[]);
     *     pc2l.start();
     *     // Create and use PC2L data structures & algorithms.
     *     pc2l.stop();
     *     pc2l::System::finalize();
     * }
     * \endcode
     *
     * \param[in] finMPI If this flag is true then MPI is also
     * finalized.  This flag is perset to provide users with finer
     * control on finalization if they need (i.e., maybe users want
     * to separately use MPI for some other operations)
     */
    void finalize(bool finMPI = true) noexcept;

protected:
    /**
     * Helper method to facilitate the PC2L system to run in
     * OpMode::OneWriter_DistributedCache mode.  On a worker-process
     * (i.e., MPI-rank != 0), this method creates a CacheWorker and
     * runs it.  On the manager-process (i.e., MPI-rank == 0), this
     * method just initializes the CacheManager object in this class.
     */
    void oneWriterDistribCache();
    
protected:
    /**
     * The current mode of operation in which the system is currently
     * operating.  This value is set in the start() method.
     */
    OpMode mode = InvalidMode;

    /**
     * The process-wide unique singleton instance of this class.
     */
    static System system;

private:
    /**
     * The default constructor has been made private to ensure that
     * this class is never instantiated.  Instead, use the global
     * singleton instance of this class for performing various
     * operations.  use the various static methods in this class
     * directly, for example:
     *
     * \code
     * pc2l::System::initialize();
     * \endcode
     */
    System() {}

    /**
     * Remove a copy constructor to ensure that the process-wide
     * unique system object is never copied.
     */
    System(const System&) = delete;

    /**
     * Remove the default assignment operator as well, just to play it
     * safe to ensure that the process-wide unique object is never
     * copied.
     */
    System& operator=(const System& other) = delete;
};

END_NAMESPACE(pc2l);
// }   // end namespace pc2l

#endif
