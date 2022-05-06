# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                 OpenGL Module                 #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(SPARK_OpenGL)

include(../../cmake/UtilFunctions.cmake)

find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
	fetch_GLEW()
endif()

# Sources
# ###############################################
file(GLOB_RECURSE SRC_FILES
	${SPARK_DIR}/include/SPARK_GL.h
	${SPARK_DIR}/include/Rendering/OpenGL/*.h
	${SPARK_DIR}/src/Rendering/OpenGL/*.cpp
)

# Build step
# ###############################################

if(${SPARK_STATIC_BUILD})
	set(SPARK_OUTPUT_TYPE static)
	add_library(SPARK_OpenGL STATIC ${SRC_FILES})
else()
	set(SPARK_OUTPUT_TYPE dynamic)
	add_library(SPARK_OpenGL SHARED ${SRC_FILES})
	target_compile_definitions(SPARK_OpenGL PRIVATE SPK_CORE_IMPORT SPK_GL_EXPORT)
endif()

if(MINGW)
	# Don't prefix the DLL with 'lib'
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(MSVC)
	set_target_properties(SPARK_OpenGL PROPERTIES COMPILE_FLAGS "/fp:fast")
endif()

target_link_libraries(SPARK_OpenGL
	GLEW::GLEW
	SPARK_Core
	OpenGL::GL
)

set_target_properties(SPARK_OpenGL PROPERTIES
	OUTPUT_NAME SPARK_GL
)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set_target_properties(SPARK_OpenGL PROPERTIES
		DEBUG_POSTFIX _debug
	)
endif()

if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
	set_target_properties(SPARK_OpenGL PROPERTIES
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
		add_custom_command(TARGET SPARK_OpenGL POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SPARK_OpenGL> "${SPARK_DIR}/demos/bin"
		)
	endif()
endif()
