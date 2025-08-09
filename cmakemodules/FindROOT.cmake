################################################################################
#    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
#                                                                              #
#              This software is distributed under the terms of the             #
#         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################
# - Find ROOT installation
# This module locates the root-config script and extracts version, include,
# libdir, bindir, and library flags from it.
# It defines:
#   ROOT_FOUND
#   ROOTSYS, ROOT_INSTALL_DIR
#   ROOT_VERSION_STRING, ROOT_VERSION_MAJOR, _MINOR, _PATCH, ROOT_VERSION_NUMBER
#   ROOT_LIBRARY_DIR, ROOT_BINARY_DIR, ROOT_INCLUDE_DIR, ROOT_LIBRARIES
#   ROOT_CINT_EXECUTABLE, RLIBMAP_EXECUTABLE

Message(STATUS "Looking for Root...Wow!")

set(ROOT_CONFIG_SEARCHPATH
  ${SIMPATH}/bin
  ${SIMPATH}/tools/root/bin
  $ENV{ROOTSYS}/bin
)

set(ROOT_FOUND FALSE)
set(ROOT_INSTALLED_VERSION_TOO_OLD FALSE)

find_program(ROOT_CONFIG_EXECUTABLE
  NAMES root-config
  PATHS ${ROOT_CONFIG_SEARCHPATH}
  NO_DEFAULT_PATH
)

if(ROOT_CONFIG_EXECUTABLE)

  # Derive ROOTSYS from the root-config path
  string(REGEX REPLACE "(^.*)/bin/root-config" "\\1" _tmp "${ROOT_CONFIG_EXECUTABLE}")
  set(ENV{ROOTSYS} "${_tmp}")
  set(ROOTSYS   "${_tmp}")

  # --- Version string ---
  execute_process(
    COMMAND "${ROOT_CONFIG_EXECUTABLE}" --version
    OUTPUT_VARIABLE ROOT_VERSION_STRING
  )
  string(STRIP   "${ROOT_VERSION_STRING}" ROOT_VERSION_STRING)
  # normalize "6.14/06" → "6.14.06"
  string(REPLACE "/" "." ROOT_VERSION_STRING "${ROOT_VERSION_STRING}")

  message(STATUS "Looking for Root... - Found ${ROOTSYS}/bin/root")
  message(STATUS "Looking for Root... - Found version is ${ROOT_VERSION_STRING}")

  # --- Numeric parse ---
  # extract all digit sequences: major, minor, patch
  string(REGEX MATCHALL "[0-9]+" _root_ver_list "${ROOT_VERSION_STRING}")
  list(GET _root_ver_list 0 ROOT_VERSION_MAJOR)
  list(GET _root_ver_list 1 ROOT_VERSION_MINOR)
  list(GET _root_ver_list 2 ROOT_VERSION_PATCH)

  # compute single integer for comparisons
  math(EXPR found_vers         "${ROOT_VERSION_MAJOR}*10000 + ${ROOT_VERSION_MINOR}*100 + ${ROOT_VERSION_PATCH}")
  set(ROOT_VERSION_NUMBER      ${found_vers})
  set(ROOT_Version             ${found_vers})

  # --- Compare to requested version, if any ---
  # 'req_vers' is set when find_package(ROOT X.Y REQUIRED) is used
  if(DEFINED req_vers)
    if(found_vers LESS req_vers)
      set(ROOT_FOUND FALSE)
      set(ROOT_INSTALLED_VERSION_TOO_OLD TRUE)
    else()
      set(ROOT_FOUND TRUE)
    endif()
  else()
    # no specific version requested → accept
    set(ROOT_FOUND TRUE)
  endif()

else()
  message(STATUS "Looking for Root... - Not found")
endif()

if(ROOT_FOUND)

  # library directory
  execute_process(
    COMMAND "${ROOT_CONFIG_EXECUTABLE}" --libdir
    OUTPUT_VARIABLE ROOT_LIBRARY_DIR
  )
  string(STRIP "${ROOT_LIBRARY_DIR}" ROOT_LIBRARY_DIR)

  # binary directory
  execute_process(
    COMMAND "${ROOT_CONFIG_EXECUTABLE}" --bindir
    OUTPUT_VARIABLE ROOT_BINARY_DIR
  )
  string(STRIP "${ROOT_BINARY_DIR}" ROOT_BINARY_DIR)

  # include directory
  execute_process(
    COMMAND "${ROOT_CONFIG_EXECUTABLE}" --incdir
    OUTPUT_VARIABLE ROOT_INCLUDE_DIR
  )
  string(STRIP "${ROOT_INCLUDE_DIR}" ROOT_INCLUDE_DIR)

  # link flags
  execute_process(
    COMMAND "${ROOT_CONFIG_EXECUTABLE}" --glibs
    OUTPUT_VARIABLE ROOT_LIBRARIES
  )
  string(STRIP "${ROOT_LIBRARIES}" ROOT_LIBRARIES)

  mark_as_advanced(ROOT_LIBRARY_DIR ROOT_INCLUDE_DIR ROOT_DEFINITIONS)

  set(ROOT_INCLUDES  "${ROOT_INCLUDE_DIR}")
  set(LD_LIBRARY_PATH "${LD_LIBRARY_PATH};${ROOT_LIBRARY_DIR}")

  find_program(ROOT_CINT_EXECUTABLE
    NAMES rootcint
    PATHS ${ROOT_BINARY_DIR}
    NO_DEFAULT_PATH
  )
  find_program(RLIBMAP_EXECUTABLE
    NAMES rlibmap
    PATHS ${ROOT_BINARY_DIR}
    NO_DEFAULT_PATH
  )

else()
  if(ROOT_FIND_REQUIRED)
    message(STATUS "Looking for ROOT... - Found version too old.")
    message(STATUS "Looking for ROOT... - Minimum required version is ${ROOT_FIND_VERSION}")
    message(FATAL_ERROR "Stop here because of a wrong Root version.")
  endif()
endif()