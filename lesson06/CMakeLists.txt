cmake_minimum_required(VERSION 3.0.0)
project(mat_mul_test VERSION 0.2)

# This file was written by Chiro2001

include(CTest)
enable_testing()

# MPI Settings
SET(CMAKE_CXX_COMPILER mpicxx)
SET(CMAKE_C_COMPILER  mpicc)

message(STATUS "${PROJECT_NAME} started building on ${CMAKE_SYSTEM_NAME}!")

# Packages
find_package(OpenBLAS REQUIRED)
include_directories(${OpenBLAS_INCLUDE_DIRS})
message(STATUS "OpenBLAS include path: ${OpenBLAS_INCLUDE_DIRS}")
find_package(MPI REQUIRED)
message(STATUS "MPI Include path: ${MPI_C_INCLUDE_DIRS}; ${MPI_CXX_INCLUDE_DIRS}. Libs: ${MPI_LIBRARIES}")

# Include
set(include_dirs
      "${PROJECT_SOURCE_DIR}/src"
      "${MPI_CXX_INCLUDE_DIRS}" "${MPI_C_INCLUDE_DIRS}")
message(STATUS "Include dirs: ${include_dirs}")
include_directories(${include_dirs})

# Src

file(GLOB_RECURSE source_files
        "${PROJECT_SOURCE_DIR}/src/*.h"
        "${PROJECT_SOURCE_DIR}/src/mat.c" "${PROJECT_SOURCE_DIR}/src/utils.c")
message(STATUS "Source files: ${source_files}")

# Resources
# file(GLOB_RECURSE resources "${PROJECT_SOURCE_DIR}/resource/*")
# message(STATUS "Copying resources file(s): ${resources}")
# file(COPY ${resources} DESTINATION ${CMAKE_BINARY_DIR}/)

# Linking
link_libraries(-lpthread ${OpenBLAS_LIBRARIES} -lm ${MPI_LIBRARIES})
message(STATUS "OpenBLAS libs are: ${OpenBLAS_LIBRARIES}")

set(CMAKE_C_FLAGS "-Wall -g -ggdb -mavx -fopenmp -ldl")
#set(CMAKE_C_FLAGS "-Wall -O3 -mavx -fopenmp -ldl -march=native")

add_executable(mat_mul_test ${PROJECT_SOURCE_DIR}/src/mat_mul_test.c ${source_files})

add_executable(mpi_demo ${PROJECT_SOURCE_DIR}/mpi_demo.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
