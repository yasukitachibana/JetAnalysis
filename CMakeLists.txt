###########################
### Initial Cmake Setup ###
###########################

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
#cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project (JetAnalysis CXX C)

# Fail if cmake is called in the source directory
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message("source directory:" ${CMAKE_SOURCE_DIR})
  message("binary directory:" ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "You don't want to configure in the source directory!")
endif()

# Default to the release build.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# for mac compliance
cmake_policy(SET CMP0042 NEW)

# Tell cmake where to find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmakemodules")


###############################
### Compiler & Linker Flags ###
###############################
message("Compiler and Linker flags ...")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pipe -Wall -std=c++17")
## can turn off some warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -Wno-unused-variable ")
## Then set the build type specific options. These will be automatically appended.
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
## can turn on debugging information
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

### system dependence.
### note that APPLE also defines UNIX, hence the elseif to differentiate
if (APPLE)
    message( STATUS "Apple : " ${CMAKE_HOST_SYSTEM})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang") 
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare -Wno-unused-but-set-variable -Wno-parentheses -fext-numeric-literals")
    endif()

    ## can turn off some warnings
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
elseif(UNIX) 
    message( STATUS "Linux : " ${CMAKE_HOST_SYSTEM})
    ## can turn off some warnings
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare -Wno-unused-but-set-variable -Wno-parentheses -fext-numeric-literals")
    endif()

    ## Additional options
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()   

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
#message(STATUS "LD_FLAGS = " ${CMAKE_EXE_LINKER_FLAGS})




#############################################
### Installing Header and Library Files   ###
#############################################
## Run with, e.g., cmake -DCMAKE_INSTALL_PREFIX=~/tmp .. 
## default directory is the build directory
## Note that trento also automatically populates a bin directory
## and we cannot disable this behavior
## Also, iSS and mpihydro install binaries in CMAKE_HOME_DIR. Sigh.
###
# Install header files
# default install prefix: build directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}"
    CACHE PATH "Install path prefix, prepended onto install directories."
    FORCE)
endif()
message(STATUS "Now: install prefix is ${CMAKE_INSTALL_PREFIX}")

install(
  # source directory
  DIRECTORY
  ##
  # our source
  "${CMAKE_SOURCE_DIR}/src/"
  ##
  # external packages
  "${CMAKE_SOURCE_DIR}/external_packages/"
  ###
  DESTINATION "include" # target directory
  FILES_MATCHING # install only matched files
  PATTERN "*.h*" # select header files
  )



###########################
### Required packages   ###
###########################

#ROOT
message("Looking for ROOT ...")
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDES})
Message(STATUS "ROOT Include dir : " ${ROOT_INCLUDES})
Message(STATUS "ROOT Libraries : " ${ROOT_LIBRARIES})


#Pythia8
message("Looking for Pythia8 ...")
find_package(Pythia8 REQUIRED)
include_directories(${PYTHIA8_INCLUDE_DIR})
Message(STATUS "Pythia8 Include dir : " ${PYTHIA8_INCLUDE_DIR})
Message(STATUS "Pythia8 Libraries : " ${PYTHIA8_LIBRARIES})


#FastJet
message("Looking for FastJet ...")
find_package(FASTJET REQUIRED)
include_directories(${FASTJET_INCLUDE_DIR})
link_directories(${FASTJET_LIBS_DIR})
add_definitions(${FASTJET_DEFINITIONS})
Message(STATUS "FastJet Include dir : " ${FASTJET_INCLUDE_DIR})
Message(STATUS "FastJet Libraries dir : " ${FASTJET_LIBS_DIR})
Message(STATUS "FastJet Definition dir : " ${FASTJET_DEFINITIONS})
Message(STATUS "FastJet Libraries : ${FASTJET_LIBS}")


###########################
### Framework Includes  ###
###########################
message ("Include Directories ...")
include_directories(./src/ )
include_directories(./src/framework/)
include_directories(./src/analysis/)
include_directories(./src/subtraction/)
include_directories(./src/reconstruction/)
include_directories(./src/load/)
include_directories(./external_packages/)

### include_directories will be expanded as packages are found



###################################################
### Some additional settings for subdirectories ###
###################################################
add_subdirectory(./external_packages)
add_subdirectory(./src)



###########################
### Executables         ###
###########################

add_executable(Main ./src/main.cc)
target_link_libraries(Main JetAnalysis)

