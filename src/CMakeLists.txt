cmake_minimum_required (VERSION 2.6)
project (gcodetimer)

set(Project_VERSION_MAJOR 1)
set(Project_VERSION_MINOR 0)
set(Project_VERSION_REVISION 0)

# Properties
set (EXECUTABLE_NAME "${PROJECT_NAME}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# External binary libraries
set(Boost_FIND_REQUIRED true)
find_package (Boost REQUIRED filesystem)

# Includes
include_directories ("${PROJECT_SOURCE_DIR}/../include")
include_directories ("${PROJECT_SOURCE_DIR}/../3rd-party/cfgpath/include")

include_directories (${Boost_INCLUDE_DIRS})

configure_file (
  "${PROJECT_SOURCE_DIR}/../include/versioninfo.h.in"
  "${PROJECT_BINARY_DIR}/include/versioninfo.h"
)

include_directories ("${PROJECT_BINARY_DIR}/include")


# Main program compilation
set (MAIN_CPP_FILES
        gcodetimer.cc

        GCodeProcessorBase.cc
        CmdLineParams.cc
        Config.cc
        )

add_executable (${EXECUTABLE_NAME} ${MAIN_CPP_FILES})

# Linker
target_link_libraries (${EXECUTABLE_NAME} ${Boost_LIBRARIES})
