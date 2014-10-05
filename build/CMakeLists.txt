cmake_minimum_required 	(VERSION 2.8)
project 		(DisplayImage)
find_package 		(OpenCV REQUIRED)
include_directories 	("../include")
file			(MAKE_DIRECTORY ../bin)
add_executable 		(DisplayImage ../src/DisplayImage.cpp)
set_target_properties	(DisplayImage PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/)
target_link_libraries 	(DisplayImage ${OpenCV_LIBS})


