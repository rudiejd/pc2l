#ifndef VECTOR_CPP
#define VECTOR_CPP
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
// Authors:   JD Rudie               rudiejd@miamioh.edu
//---------------------------------------------------------------------
/**
 * @file Vector.h
 * @brief Implementation of Vector
 * @author JD Rudie
 * @version 0.1
 * @date 2021-08-30
 * 
 */
#include "Vector.h"
#include "System.h"
#include "MPIHelper.h"

BEGIN_NAMESPACE(pc2l);

constexpr int MANAGER = 0;
constexpr int DATA_TAG = 1;

int Vector::at(unsigned int index) {
    const int worldSize = System::get().worldSize();
    if (worldSize < 2) {
        return localVec.at(index);
    }

    // Do distributed caching operations where the data is 
    // obtained from a remote sender process to rank 0.
    int value;
    const int senderRank = index % System::get().worldSize() == index % System::get().worldSize();
    if (MPI_GET_RANK() == MANAGER && senderRank == MANAGER) {
        return localVec[index / System::get().worldSize()]; 
    }
    MPI_Request request;
    MPI_STATUS status;
    if (MPI_GET_RANK() != MANAGER) {
        int sendingBuffer = localVec[index / System::get().worldSize()];
        MPI_Isend(&sendingBuffer, 1, MPI_TYPE_INT, MANAGER, DATA_TAG, MPI_COMM_WORLD, &request);
    } else { 
        MPI_Irecv(&value, 1, MPI_TYPE_INT, senderRank, DATA_TAG, MPI_COMM_WORLD, &request);
    }
    return value;
}

void Vector::insert(unsigned int index, int value) {
    int targetRank = index % System::get().worldSize();
    std::cout << " my rank is " << MPI_GET_RANK() << std::endl;
    if (MPI_GET_RANK() == targetRank) {
        localVec.push_back(value);
        for (auto v : localVec) std::cout << v << std::endl;
    } 
}

END_NAMESPACE(pc2l);
#endif

