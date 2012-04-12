# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                External - GLEW                #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 2.8)
project(GLEW)



# Sources
# ###############################################
set(SPARK_DIR ../../../..)
get_filename_component(SPARK_DIR ${SPARK_DIR}/void REALPATH)
get_filename_component(SPARK_DIR ${SPARK_DIR} PATH)
file(GLOB_RECURSE SRC_FILES
	${SPARK_DIR}/external/glew/src/*.c
	${SPARK_DIR}/external/glew/src/*.cpp
	${SPARK_DIR}/external/glew/src/*.cxx
	${SPARK_DIR}/external/glew/src/*.h
	${SPARK_DIR}/external/glew/src/*.hpp
)



# Build step
# ###############################################
add_definitions(-DGLEW_STATIC)
add_library(GLEW STATIC ${SRC_FILES})
include_directories(${SPARK_DIR}/external/glew/src)
set_target_properties(GLEW PROPERTIES
	OUTPUT_NAME glew
	DEBUG_POSTFIX _d
	RUNTIME_OUTPUT_DIRECTORY ${SPARK_DIR}/external/glew/bin
	LIBRARY_OUTPUT_DIRECTORY ${SPARK_DIR}/external/glew/bin
	ARCHIVE_OUTPUT_DIRECTORY ${SPARK_DIR}/external/glew/bin
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/external/glew/bin
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/external/glew/bin
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/external/glew/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/external/glew/bin
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/external/glew/bin
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/external/glew/bin
)
