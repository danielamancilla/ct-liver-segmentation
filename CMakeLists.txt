# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 2.8.9)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()


# This project is designed to be built outside the Insight source tree.
PROJECT(CTLiverSegmentation)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

SET(
  _programs
  LiverSegmentation
  LiverSegmentationMultipleSeeds
  WatershedSegmentation
  LiverSegmentationWatershedRegionGrow
  LiverSegmentationWatershedRegionGrowIsolated
  LiverSegmentationAllImages
  )

FOREACH(_p ${_programs})
  ADD_EXECUTABLE(${_p} ${_p}.cxx)
  TARGET_LINK_LIBRARIES(${_p} ${ITK_LIBRARIES})
ENDFOREACH(_p)
