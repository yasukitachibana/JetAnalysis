###########################
### Initial Cmake Setup ###
###########################

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(JetAnalysis CXX C)

# require C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_HOME_DIRECTORY}")

set(CMAKE_INSTALL_RPATH ${CMAKE_HOME_DIRECTORY})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)



# Tell cmake where to find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmakemodules")


message("Looking for ROOT ...")
find_package(ROOT)
if (${ROOT_FOUND})
  include_directories(${ROOT_INCLUDES})
  Message(STATUS "ROOT Include dir : " ${ROOT_INCLUDES})
  Message(STATUS "ROOT Libraries : " ${ROOT_LIBRARIES})
endif()

message("Looking for Pythia8 ...")
find_package(Pythia8)
if (${PYTHIA8_FOUND})
  include_directories(${PYTHIA8_INCLUDE_DIR})
  Message(STATUS "Pythia8 Include dir : " ${PYTHIA8_INCLUDE_DIR})
  Message(STATUS "Pythia8 Libraries : " ${PYTHIA8_LIBRARIES})
endif()

message("Looking for FastJet ...")
find_package(FASTJET)
if (${FASTJET_FOUND})
  include_directories(${FASTJET_INCLUDE_DIR})
  link_directories(${FASTJET_LIBS_DIR})
  add_definitions(${FASTJET_DEFINITIONS})
  Message(STATUS "FastJet Include dir : " ${FASTJET_INCLUDE_DIR})
  Message(STATUS "FastJet Libraries dir : " ${FASTJET_LIBS_DIR})
  Message(STATUS "FastJet Definition dir : " ${FASTJET_DEFINITIONS})
  Message(STATUS "FASTJET Libraries : ${FASTJET_LIBS}")
endif()


#add_subdirectory(fjcore)
add_subdirectory(tinyxml2)
add_subdirectory(src)

add_executable (Main ./src/main.cc)
target_link_libraries (Main main_lib tinyxml2_lib ${ROOT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${FASTJET_LIBS})
