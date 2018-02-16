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
# add this line only when you are using openmpi which has a different c++ bindings
# MAKE SURE "g++" and "c++" environment variables points to the same compiler, i.e. clang or gnu.
add_definitions(-DOMPI_SKIP_MPICXX)
message("MPI_CXX_COMPILER           : ${MPI_CXX_COMPILER}")
message("MPI_CXX_VERSION            : ${MPI_CXX_VERSION}")
message("MPI_CXX_COMPILE_OPTIONS    : ${MPI_CXX_COMPILE_OPTIONS}")

###################
### Setup flags ###
###################
set(COMMON_OPTIONS -g -Wall -march=native)
set(DEBUG_OPTIONS       -Wextra -O0)
set(RELEASE_OPTIONS     -DNDEBUG -O3)
message("Compiling with: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
site_name(this_host)
if ("${this_host}" MATCHES "triolith")
    message("Host: ${this_host}")
    set(CMAKE_CXX_COMPILER mpiicpc)
    set(COMMON_OPTIONS ${COMMON_OPTIONS} -ip -xavx -cxxlib=/software/apps/gcc/5.3.0/build01/ -gxx-name=g++)
else()
    message("Host: ${this_host}")
#    set(CMAKE_CXX_COMPILER mpicxx)
#    set(CMAKE_)
endif()



if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
    message("Special flags for g++:  -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-int-in-bool-context")
    set(COMMON_OPTIONS ${COMMON_OPTIONS}  -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-int-in-bool-context)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message("Special flags for Clang:  -stdlib=libstdc++ -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-invalid-partial-specialization -Wno-missing-braces -Wno-overloaded-virtual")
    set(COMMON_OPTIONS ${COMMON_OPTIONS}   -stdlib=libstdc++ -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-invalid-partial-specialization -Wno-missing-braces -Wno-overloaded-virtual)
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
target_compile_options(${PROJECT_NAME} PUBLIC ${COMMON_OPTIONS})                                   ### Common options
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")               ### Debug build options
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")           ### Release build options
target_link_libraries (${PROJECT_NAME} -flto)                                                       ### Link time optimization flag


######################################################
###  Require c++17 support and test that it works  ###
######################################################
set_target_properties  (${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED 17)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)                                         ### Demand c++17 compiler support
INCLUDE(CheckIncludeFileCXX)
check_include_file_cxx(experimental/filesystem  has_experimental_filesystem -std=c++17)
check_include_file_cxx(experimental/type_traits has_experimental_type_traits -std=c++17)
if(has_experimental_filesystem AND has_experimental_type_traits)
    message(STATUS "C++17 experimental features are supported.")
else()
    message(FATAL_ERROR "Missing experimental headers for C++17. Check your compiler and libstdc++ versions")
endif()




#######################################
###  Link and include dependencies  ###
#######################################

target_include_directories(${PROJECT_NAME} PRIVATE source)
target_include_directories(${PROJECT_NAME} PRIVATE ${MPI_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} -lstdc++fs)                              ### For experimental::filesystem

include(cmake/Find_dont_install_INTELMKL.cmake)
include(cmake/Find_or_install_EIGEN3.cmake)                                     ### For Eigen linear algebra header-library http://eigen.tuxfamily.org/
include(cmake/Find_or_install_HDF5.cmake)                                       ### For HDF5 data env_storage format        https://support.hdfgroup.org

message("======================================")
message("=== SUMMARY OF CMAKE CONFIGURATION ===")
message("======================================")
message("FOUND INTEL MKL:   ${MKL_LIBRARIES}")
message("FOUND EIGEN3   :   ${EIGEN3_INCLUDE_DIR}")
message("FOUND HDF5     :   ${HDF5_LIBRARIES}")
message("======================================")

###################
### INFORMATION ###
###################
message("==================================================================")
message("")
message("      To clean cmake files, run './build.sh clean'")
message("      To clean cmake files downloaded libraries, run './clean.sh '")
message("      To launch this program,  './run.sh'")
message("")
message("==================================================================")



# To print all variables, use the code below:
#
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


