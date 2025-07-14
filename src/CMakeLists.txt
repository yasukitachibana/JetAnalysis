cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project (libJetAnalysis)

#for MacOSX
cmake_policy(SET CMP0042 NEW)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "./lib")

# framework
FILE(GLOB frameworksources framework/*.cc)
set (SOURCES  ${SOURCES}  ${frameworksources} )

# analysis
FILE(GLOB analysissources analysis/*.cc)
set (SOURCES  ${SOURCES}  ${analysissources} )

# subtraction
FILE(GLOB subtractionsources subtraction/*.cc)
set (SOURCES  ${SOURCES}  ${subtractionsources} )

# reconstruction
FILE(GLOB reconstructionsources reconstruction/*.cc)
set (SOURCES  ${SOURCES}  ${reconstructionsources} )

# load
FILE(GLOB loadsources load/*.cc)
set (SOURCES  ${SOURCES}  ${loadsources} )

# load
FILE(GLOB smearsources smear/*.cc)
set (SOURCES  ${SOURCES}  ${smearsources} )


#######################################
add_library(JetAnalysis SHARED ${SOURCES})
target_link_libraries (JetAnalysis JetAnalysisExternal ${ROOT_LIBRARIES} ${PYTHIA8_LIBRARIES} ${FASTJET_LIBS})
# -------------------------------------------------------------------------
