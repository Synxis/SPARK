# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                Irrlicht module                #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(SPARK_Irrlicht)

include(../../cmake/UtilFunctions.cmake)

set(SPARK_IRRLICHT_LIB              ""  CACHE FILEPATH "Store the full path of the Irrlicht library file")
set(SPARK_IRRLICHT_INCLUDE_DIR      ""  CACHE PATH	 "Store the path to the 'include' folder of Irrlicht")
set(SPARK_IRRLICHT_USE_STATIC_LIBS  OFF CACHE BOOL	 "Store whether to link statically to the Irrlicht library")

get_property(isMulti GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${isMulti})
	set(SPARK_IRRLICHT_LIB_DEBUG "" CACHE FILEPATH "Store the full path of the debug Irrlicht library file")
endif()

if(${SPARK_IRRLICHT_USE_STATIC_LIBS})
	set(IRRLICHT_LIBRARY_LINKAGE "STATIC")
else()
	set(IRRLICHT_LIBRARY_LINKAGE "SHARED")
endif()

if(${isMulti} AND DEFINED SPARK_IRRLICHT_LIB_DEBUG)
	set(ARGS_IRR "LIBRARY_DEBUG_FILEPATH" "${SPARK_IRRLICHT_LIB_DEBUG}")
endif()

create_imported_target(IMPORTED_TARGET_NAME "Irrlicht"
					   LIBRARY_LINKAGE ${IRRLICHT_LIBRARY_LINKAGE}
					   LIBRARY_FILEPATH ${SPARK_IRRLICHT_LIB}
					   ${ARGS_IRR}
					   INCLUDE_DIR ${SPARK_IRRLICHT_INCLUDE_DIR}
)

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
endif()

if(APPLE)
	find_library(COCOA_LIB Cocoa REQUIRED)
	find_library(IOKIT_LIB IOKit REQUIRED)
endif()

if(MINGW)
	find_library(WINMM_LIB winmm REQUIRED)
endif()

if(UNIX AND NOT APPLE)
	target_link_libraries(Irrlicht INTERFACE
		X11::Xxf86vm
		X11::X11
	)
endif()

target_link_libraries(Irrlicht INTERFACE
	OpenGL::GL
)

if(MINGW)
	target_link_libraries(Irrlicht INTERFACE
		${WINMM_LIB}
	)
endif()

if(APPLE)
	target_link_libraries(Irrlicht INTERFACE
		${COCOA_LIB}
		${IOKIT_LIB}
	)
endif()

# Sources
# ###############################################
#file(GLOB_RECURSE SRC_FILES
#	${SPARK_DIR}/src/Rendering/Irrlicht/*.cpp
#)
file(GLOB_RECURSE SRC_FILES
	${SPARK_DIR}/include/SPARK_IRR.h
	${SPARK_DIR}/include/Rendering/Irrlicht/*.h
	${SPARK_DIR}/src/Rendering/Irrlicht/*.cpp
)


# Build step
# ###############################################
if(${SPARK_STATIC_BUILD})
	set(SPARK_OUTPUT_TYPE static)
	add_library(SPARK_Irrlicht STATIC ${SRC_FILES})
else()
	set(SPARK_OUTPUT_TYPE dynamic)
	add_library(SPARK_Irrlicht SHARED ${SRC_FILES})
	target_compile_definitions(SPARK_Irrlicht PRIVATE SPK_CORE_IMPORT SPK_IRR_EXPORT)
endif()

if(${SPARK_IRRLICHT_USE_STATIC_LIBS})
	target_compile_definitions (SPARK_Irrlicht PRIVATE "_IRR_STATIC_LIB_")
endif()

target_link_libraries(SPARK_Irrlicht
	SPARK_Core
	Irrlicht
)

if(MINGW)
	# Don't prefix the DLL with 'lib'
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(MSVC)
	set_target_properties(SPARK_Irrlicht PROPERTIES COMPILE_FLAGS "/fp:fast")
endif()

set_target_properties(SPARK_Irrlicht PROPERTIES
	OUTPUT_NAME SPARK_IRR
)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set_target_properties(SPARK_Irrlicht PROPERTIES
		DEBUG_POSTFIX _debug
	)
endif()

if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
	set_target_properties(SPARK_Irrlicht PROPERTIES
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
		add_custom_command(TARGET SPARK_Irrlicht POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SPARK_Irrlicht> "${SPARK_DIR}/demos/bin"
		)
	endif()
endif()

