# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#              External -- PugiXML              #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 2.8)
project(PugiXML)



# Sources
# ###############################################
set(SPARK_DIR ../../../..)
get_filename_component(SPARK_DIR ${SPARK_DIR}/void REALPATH)
get_filename_component(SPARK_DIR ${SPARK_DIR} PATH)
file(GLOB_RECURSE SRC_FILES
	${SPARK_DIR}/external/pugixml/src/*.c
	${SPARK_DIR}/external/pugixml/src/*.cpp
	${SPARK_DIR}/external/pugixml/src/*.cxx
	${SPARK_DIR}/external/pugixml/src/*.h
	${SPARK_DIR}/external/pugixml/src/*.hpp
)



# Build step
# ###############################################
add_library(PugiXML STATIC ${SRC_FILES})
set_target_properties(PugiXML PROPERTIES
	OUTPUT_NAME pugixml
	DEBUG_POSTFIX _d
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/external/pugixml/bin
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/external/pugixml/bin
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/external/pugixml/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/external/pugixml/bin
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/external/pugixml/bin
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/external/pugixml/bin
)
