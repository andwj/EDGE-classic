project(
  libwebsid
  LANGUAGES C CXX
  VERSION 0.1.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
if(WIN32 AND NOT MSYS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CC_FLAGS} /fp:fast /EHs")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CC_FLAGS} -ffast-math -fno-strict-aliasing")
endif()

add_library(
  libwebsid
  core.cpp
  digi.cpp
  envelope.cpp
  filter.cpp
  filter6581.cpp
  filter8580.cpp
  loaders.cpp
  sid.cpp
  system.cpp
  wavegenerator.cpp
  cia.c
  cpu.c
  hacks.c
  memory.c
  vic.c
)
