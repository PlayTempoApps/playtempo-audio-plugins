cmake_minimum_required(VERSION 3.25)
project(ModernMetalBass VERSION 0.4.1)

if(NOT DEFINED IPLUG2_DIR)
  set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../iPlug2" CACHE PATH "iPlug2 root directory")
endif()

include(${IPLUG2_DIR}/iPlug2.cmake)

find_package(iPlug2 REQUIRED)

set(MODERN_METAL_BASS_SOURCES
  ModernMetalBass.cpp
  ModernMetalBass.h
  resources/resource.h
)

if(WIN32)
  list(APPEND MODERN_METAL_BASS_SOURCES resources/main.rc)
  set_source_files_properties(resources/main.rc PROPERTIES
    COMPILE_FLAGS "/I\"${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts\" /I\"${CMAKE_CURRENT_SOURCE_DIR}/resources/img\" /I\"${CMAKE_CURRENT_SOURCE_DIR}/resources\""
  )
  set(MODERN_METAL_BASS_FORMATS VST3)
else()
  set(MODERN_METAL_BASS_FORMATS MINIMAL_PLUGINS APP WASM)
endif()

iplug_add_plugin(${PROJECT_NAME}
  SOURCES
    ${MODERN_METAL_BASS_SOURCES}
  RESOURCES
    resources/fonts/Roboto-Regular.ttf
    resources/img/midnight-precision-panel.png
  FORMATS
    ${MODERN_METAL_BASS_FORMATS}
)
