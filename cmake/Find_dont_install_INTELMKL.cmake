

# If you want to use Eigen and MKL you need to add #define EIGEN_USE_MKL_ALL before including <Eigen..>
# Adding a -DEIGEN_USE_MKL_ALL here may conflict with arpack++

set(MKL_USE_STATIC_LIBS ON)
set(MKL_MULTI_THREADED ON)
find_package(MKL)
if (MKL_FOUND)
    if(MKL_MULTI_THREADED)
        list(APPEND MKL_LIBRARIES -lpthread -lm)
    endif()
    add_definitions(-DMKL_AVAILABLE)
    message("MKL_LIBRARIES: ${MKL_LIBRARIES}" )
    target_include_directories(${PROJECT_NAME} PRIVATE ${MKL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${MKL_LIBRARIES})
    target_compile_options(${PROJECT_NAME} PUBLIC -DEIGEN_USE_MKL_ALL -Wno-unknown-pragmas -Wno-parentheses -Wno-unused-variable)                                   ### Common options
endif()



#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    #            if("${_variableName}" MATCHES "HDF5")
#    message(STATUS "${_variableName}=${${_variableName}}")
#    #            endif()
#endforeach()