# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                  Core Module                  #
#                                               #
# ############################################# #

include(../../cmake/UtilFunctions.cmake)

# Project declaration
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(SPARK_Core)

find_package(pugixml QUIET)
if(NOT pugixml_FOUND)
	fetch_Pugixml()
endif()

# Sources
# ###############################################

file(GLOB_RECURSE TMP_SRC_FILES
	${SPARK_DIR}/include/Core/*.h
	${SPARK_DIR}/include/Extensions/*.h
)
foreach(f ${TMP_SRC_FILES})
	file(RELATIVE_PATH SRCGR ${SPARK_DIR}/include ${f})
	string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
	string(REPLACE / \\ SRCGR ${SRCGR})
	source_group(Headers\\${SRCGR} FILES ${f})
endforeach()
set(SRC_FILES ${TMP_SRC_FILES})

file(GLOB_RECURSE TMP_SRC_FILES
	${SPARK_DIR}/src/Core/*.cpp
	${SPARK_DIR}/src/Extensions/*.cpp
)
foreach(f ${TMP_SRC_FILES})
	file(RELATIVE_PATH SRCGR ${SPARK_DIR}/src ${f})
	string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
	string(REPLACE / \\ SRCGR ${SRCGR})
	source_group(Sources\\${SRCGR} FILES ${f})
endforeach()
set(SRC_FILES
	${SPARK_DIR}/include/SPARK.h
	${SPARK_DIR}/include/SPARK_Core.h
	${SRC_FILES}
	${TMP_SRC_FILES}
)
source_group(Headers FILES
	${SPARK_DIR}/include/SPARK.h
	${SPARK_DIR}/include/SPARK_Core.h
)

# Build step
# ###############################################

if(${SPARK_STATIC_BUILD})
	set(SPARK_OUTPUT_TYPE static)
	add_library(SPARK_Core STATIC ${SRC_FILES})

	target_include_directories(SPARK_Core PUBLIC
		${SPARK_DIR}/include
	)
else()
	set(SPARK_OUTPUT_TYPE dynamic)
	add_library(SPARK_Core SHARED ${SRC_FILES})

	target_include_directories(SPARK_Core PUBLIC
		${SPARK_DIR}/include
	)

	target_compile_definitions(SPARK_Core PRIVATE SPK_CORE_EXPORT)
endif()

if(MINGW)
	# Don't prefix the DLL with 'lib'
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
	set_target_properties(SPARK_Core PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj")
endif()

if(MSVC)
	set_target_properties(SPARK_Core PROPERTIES COMPILE_FLAGS "/fp:fast /bigobj")
endif()

target_link_libraries(SPARK_Core
	PUGIXML::PUGIXML
)

set_target_properties(SPARK_Core PROPERTIES
	OUTPUT_NAME SPARK
)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set_target_properties(SPARK_Core PROPERTIES
		DEBUG_POSTFIX _debug
	)
endif()

if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
	set_target_properties(SPARK_Core PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		LIBRARY_OUTPUT_DIRECTORY ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		ARCHIVE_OUTPUT_DIRECTORY ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/lib/${SPARK_GENERATOR}/${SPARK_OUTPUT_TYPE}
	)

	if (NOT ${SPARK_STATIC_BUILD})
		add_custom_command(TARGET SPARK_Core POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SPARK_Core> "${SPARK_DIR}/demos/bin"
		)
	endif()
endif()
