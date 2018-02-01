cmake_minimum_required(VERSION 3.8)
project(PT)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
include(cmake/get_libraries.cmake)

################################################################
### Get git version number                                   ###
### Generates a header gitversion/gitversion.h               ###
### Include it using #include <gitversion.h>                 ###
### Gives a namespace GIT:: with several git version numbers.###
################################################################
include(cmake/gitversion.cmake)
include(cmake/directory.cmake)

message("Compiling with: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")



###########################################
###  Find or install all dependencies   ###
###########################################
find_package(MPI REQUIRED)
message("MPI_CXX_COMPILER           : ${MPI_CXX_COMPILER}")
message("MPI_CXX_VERSION            : ${MPI_CXX_VERSION}")
message("MPI_CXX_COMPILE_OPTIONS    : ${MPI_CXX_COMPILE_OPTIONS}")
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()


# add this line only when you are using openmpi which has a different c++ bindings
#add_definitions(-DOMPI_SKIP_MPICXX)
# MAKE SURE "g++" and "c++" environment variables points to the same compiler, i.e. clang or gnu.


#include_directories(${MPI_INCLUDE_PATH})

include(cmake/Find_or_install_EIGEN3.cmake)                                     ### For Eigen linear algebra header-library http://eigen.tuxfamily.org/
include(cmake/Find_or_install_HDF5.cmake)                                       ### For HDF5 data storage format            https://support.hdfgroup.org

message("======================================")
message("=== SUMMARY OF CMAKE CONFIGURATION ===")
message("======================================")
message("FOUND EIGEN3   :   ${EIGEN3_INCLUDE_DIR}")
message("FOUND HDF5     :   ${HDF5_LIBRARIES}")
message("======================================")



###################
### Setup flags ###
###################
set(COMMON_OPTIONS -g -Wall -march=native)
set(DEBUG_OPTIONS       -Wextra -O0)
set(RELEASE_OPTIONS     -DNDEBUG -O3)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
message("Compiling with: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
site_name(this_host)
if ("${this_host}" MATCHES "triolith")
    message("Host: ${this_host}")
    set(CMAKE_CXX_COMPILER mpiicpc)
    set(COMMON_OPTIONS ${COMMON_OPTIONS} -ip -xavx -cxxlib=/software/apps/gcc/5.3.0/build01/ -gxx-name=g++)
else()
    message("Host: ${this_host}")
    set(CMAKE_CXX_COMPILER mpicxx)
endif()



if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
    message("Special flags for g++:  -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-int-in-bool-context")
    set(COMMON_OPTIONS ${COMMON_OPTIONS}   -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-int-in-bool-context)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message("Special flags for Clang:  -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-invalid-partial-specialization -Wno-missing-braces -Wno-overloaded-virtual")
    set(COMMON_OPTIONS ${COMMON_OPTIONS}  -stdlib=libstdc++ -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-invalid-partial-specialization -Wno-missing-braces -Wno-overloaded-virtual)
endif()









###########################################
###  Add all source files recursively   ###
###########################################
file(GLOB_RECURSE SOURCES "source/*.cpp")
file(GLOB_RECURSE HEADERS "source/*.h")
add_executable(PT main.cpp ${SOURCES} ${HEADERS})


###########################################
###  Apply RELEASE/DEBUG compile flags  ###
###########################################
target_compile_features(PT PUBLIC cxx_std_17)                                         ### Demand c++17 compiler support
target_compile_options(PT PUBLIC ${COMMON_OPTIONS})                                   ### Common options
target_compile_options(PT PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")               ### Debug build options
target_compile_options(PT PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")           ### Release build options
target_link_libraries(PT -flto)                                                       ### Link time optimization flag


#######################################
###  Link and include dependencies  ###
#######################################

target_include_directories(PT PRIVATE source)
target_include_directories(PT PRIVATE ${EIGEN3_INCLUDE_DIR})
target_include_directories(PT PRIVATE ${MPI_INCLUDE_PATH})

target_link_libraries(PT ${HDF5_LIBRARIES} ${HDF5_LDFLAGS} -ldl)
target_link_libraries(PT ${MPI_LIBRARIES})
target_link_libraries(PT -lstdc++fs)                              ### For experimental::filesystem




# To print all variables, use the code below:
#
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


