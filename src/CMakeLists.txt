set(HEADER_LIST
	"${pc2l_SOURCE_DIR}/include/MPIHelper.h"
	"${pc2l_SOURCE_DIR}/include/pc2l.h"
	"${pc2l_SOURCE_DIR}/include/Message.h"
	"${pc2l_SOURCE_DIR}/include/ArgParser.h"
	"${pc2l_SOURCE_DIR}/include/Worker.h"
	"${pc2l_SOURCE_DIR}/include/CacheWorker.h"
	"${pc2l_SOURCE_DIR}/include/CacheManager.h"
	"${pc2l_SOURCE_DIR}/include/Exception.h"
	"${pc2l_SOURCE_DIR}/include/Vector.h"
	"${pc2l_SOURCE_DIR}/include/Map.h"
	"${pc2l_SOURCE_DIR}/include/LeastRecentlyUsedCacheWorker.h"
	"${pc2l_SOURCE_DIR}/include/MostRecentlyUsedCacheWorker.h"
	"${pc2l_SOURCE_DIR}/include/LeastFrequentlyUsedCacheWorker.h"
		"${pc2l_SOURCE_DIR}/include/PseudoLRUCacheWorker.h"
		"${pc2l_SOURCE_DIR}/include/StorageCacheWorker.h"
		)
set(SRCFILES "${pc2l_SOURCE_DIR}/src/ArgParser.cpp"
				   "${pc2l_SOURCE_DIR}/src/Message.cpp"
             	   "${pc2l_SOURCE_DIR}/src/CacheManager.cpp"
				   "${pc2l_SOURCE_DIR}/src/CacheWorker.cpp"
				   "${pc2l_SOURCE_DIR}/src/Exception.cpp"
				   "${pc2l_SOURCE_DIR}/src/MPIHelper.cpp"
				   "${pc2l_SOURCE_DIR}/src/System.cpp"
				   "${pc2l_SOURCE_DIR}/src/Utilities.cpp"
				   "${pc2l_SOURCE_DIR}/src/Worker.cpp"
                   "${pc2l_SOURCE_DIR}/src/Vector.cpp"
		           "${pc2l_SOURCE_DIR}/src/LeastRecentlyUsedCacheWorker.cpp"
		           "${pc2l_SOURCE_DIR}/src/MostRecentlyUsedCacheWorker.cpp"
		           "${pc2l_SOURCE_DIR}/src/LeastFrequentlyUsedCacheWorker.cpp"
		           "${pc2l_SOURCE_DIR}/src/PseudoLRUCacheWorker.cpp"
		           "${pc2l_SOURCE_DIR}/src/StorageCacheWorker.cpp"
		../test/Environment.h ../bench/Benchmark.h)

add_library(pc2l_lib ${SRCFILES} ${HEADER_LIST})
target_include_directories(pc2l_lib PUBLIC ${pc2l_SOURCE_DIR}/include)
target_precompile_headers(pc2l_lib PUBLIC ${HEADER_LIST})
target_link_libraries(pc2l_lib ${HEADER_LIST})
find_package(MPI)
if(MPI_CXX_FOUND)
	add_compile_definitions(MPI_FOUND)
    target_include_directories(pc2l_lib PRIVATE ${MPI_CXX_INCLUDE_PATH})
    target_compile_options(pc2l_lib PRIVATE ${MPI_CXX_COMPILE_FLAGS})
    target_link_libraries(pc2l_lib ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
endif(MPI_CXX_FOUND)

if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(pc2l_lib PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pc2l_lib "${CMAKE_THREAD_LIBS_INIT}")
endif()

