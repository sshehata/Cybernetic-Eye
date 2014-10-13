cmake_minimum_required (VERSION 2.8)  
project (CyberVis)

find_package (OpenCV REQUIRED)
include_directories	("../include")

file (MAKE_DIRECTORY ../bin)
file (GLOB_RECURSE SOURCES "../src/*.cpp")

add_executable (cybervis ${SOURCES})
set_target_properties (cybervis PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/)
target_link_libraries (cybervis ${OpenCV_LIBS})


