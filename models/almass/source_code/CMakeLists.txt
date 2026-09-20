cmake_minimum_required (VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON) 

message("CMAKE VERSION = ${CMAKE_VERSION}")


if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    message("CMAKE_BUILD_TYPE not set, defaulting to ${CMAKE_BUILD_TYPE}")
  endif()
endif()

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

project (almass)

find_package(OpenMP QUIET)
include_directories(./lib) 

set(COMMON_SOURCES
  BatchALMaSS/AOR_Probe.cpp 
  BatchALMaSS/CurveClasses.cpp 
  BatchALMaSS/PopulationManager.cpp 
  BatchALMaSS/ALMaSS_Random.cpp
  lib/toojpeg/toojpeg.cpp
  Landscape/Calendar.cpp 
  Landscape/colmap.cpp
  Landscape/Configurator.cpp 
  Landscape/Croprotation.cpp 
  Landscape/Elements.cpp 
  Landscape/Farm.cpp 
  Landscape/FarmFuncs.cpp 
  Landscape/habitatmap.cpp
  Landscape/Hedgebanks.cpp 
  Landscape/Landscape.cpp 
  Landscape/Map_cfg.cpp 
  Landscape/MapErrorMsg.cpp 
  Landscape/Misc.cpp 
  Landscape/Plants.cpp 
  Landscape/Rastermap.cpp 
  Landscape/sunset.cpp
  Landscape/Weather.cpp
  Landscape/cropprogs/DK_SpringBarley.cpp
  Landscape/cropprogs/DK_WinterWheat.cpp
  Landscape/cropprogs/HB_ScheduledPlan.cpp
  Skylark/SkylarkAll.cpp
  Skylark/SkylarkConfigs.cpp
  Skylark/Skylarks_toletov.cpp
  SubPopulation/SubPopulation_Population_Manager.cpp
  SubPopulation/SubPopulation.cpp
  Theoretical/Theoretical1.cpp
  Theoretical/Theoretical1_Population_Manager.cpp
  Theoretical/Theoretical2.cpp
  Theoretical/Theoretical2_Population_Manager.cpp
  HB_PFG/HB_PFG.cpp
  HB_PFG/HB_PFG_Population_Manager.cpp
)

set(COMMON_HEADERS
  ALMaSSDefines.h
  BatchALMaSS/AOR_Probe.h 
  BatchALMaSS/CurveClasses.h  
  BatchALMaSS/PopulationManager.h 
  BatchALMaSS/ALMaSS_Random.h 
  BatchALMaSS/ALMaSS_Setup.h 
  Landscape/Calendar.h
  Landscape/colmap.h 
  Landscape/Configurator.h 
  Landscape/Croprotation.h 
  Landscape/Elements.h 
  Landscape/Farm.h
  Landscape/habitatmap.h
  Landscape/Landscape.h 
  Landscape/LandscapeFarmingEnums.h
  Landscape/Map_cfg.h 
  Landscape/MapErrorMsg.h 
  Landscape/Misc.h 
  Landscape/Plants.h 
  Landscape/Rastermap.h 
  Landscape/sunset.h
  Landscape/cropprogs/DK_SpringBarley.h
  Landscape/cropprogs/DK_WinterWheat.h
  Landscape/cropprogs/HB_ScheduledPlan.h
  Skylark/SkylarkAll.h
  Skylark/Skylarks_toletov.h
  SubPopulation/SubPopulation_Population_Manager.h
  SubPopulation/SubPopulation.h
  Theoretical/Theoretical1.h
  Theoretical/Theoretical1_Population_Manager.h
  Theoretical/Theoretical2.h
  Theoretical/Theoretical2_Population_Manager.h 
  HB_PFG/HB_PFG.h
  HB_PFG/HB_PFG_Population_Manager.h
)
 
set(CL_SOURCES 
  BatchALMaSS/ALMaSS_CmdLine.cpp 
) 

add_executable (almass_cmd ${COMMON_SOURCES} ${CL_SOURCES} ${COMMON_HEADERS})

if(OpenMP_CXX_FOUND)
  message("Found OpenMP, will parallelize")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp" )
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fopenmp" )
  target_link_libraries(almass_cmd OpenMP::OpenMP_CXX)
endif()
