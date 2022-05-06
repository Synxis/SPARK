include(FetchContent)

# When building SPARK with Irrlicht support and using
# any of the visual studio generators remove the DEBUG
# configuration if the debug Irrlicht library was not
# set using the SPARK_IRRLICHT_LIB_DEBUG variable.
# This allows to at least build in release mode and is
# needed due to the msvc crt being incompatible between
# debug and release.
function(remove_incompatible_configuration_types)
	if (${SPARK_BUILD_IRRLICHT_MODULE} AND
		CMAKE_GENERATOR MATCHES "Visual Studio" AND
		NOT DEFINED SPARK_IRRLICHT_LIB_DEBUG)

		if("Debug" IN_LIST CMAKE_CONFIGURATION_TYPES)
			list(REMOVE_ITEM CMAKE_CONFIGURATION_TYPES "Debug")
			set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
			message(STATUS "No Irrlicht debug library set. Removing the Debug configuration.")
		endif()

	endif()
endfunction()

function(create_imported_target)
	set(options DEBUG_LIB_REQUIRED)
	set(oneValueArgs IMPORTED_TARGET_NAME LIBRARY_LINKAGE LIBRARY_FILEPATH LIBRARY_DEBUG_FILEPATH INCLUDE_DIR)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN} )

	if(NOT "${ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
		message(FATAL_ERROR "Unexpected function arguments.")
	endif()

	if(NOT EXISTS "${ARG_LIBRARY_FILEPATH}")
		message(FATAL_ERROR "The library file of the ${ARG_IMPORTED_TARGET_NAME} target was not found.")
	endif()

	add_library(${ARG_IMPORTED_TARGET_NAME}
				${ARG_LIBRARY_LINKAGE}
				IMPORTED
	)

	if("${ARG_LIBRARY_LINKAGE}" STREQUAL "SHARED" AND (MINGW OR MSVC))
		# https://cmake.org/cmake/help/latest/command/add_library.html
		#
		# For a SHARED library on Windows, the IMPORTED_IMPLIB target property
		# (or its per-configuration variant IMPORTED_IMPLIB_<CONFIG>) specifies
		# the location of the DLL import library file (.lib or .dll.a) on disk,
		# and the IMPORTED_LOCATION is the location of the .dll runtime library
		# (and is optional, but needed by the TARGET_RUNTIME_DLLS generator expression).

		set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
			IMPORTED_IMPLIB ${ARG_LIBRARY_FILEPATH}
		)
	else()
		set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
			IMPORTED_LOCATION ${ARG_LIBRARY_FILEPATH}
		)
	endif()

	get_property(isMulti GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
	if(${isMulti})

		if(DEFINED ARG_LIBRARY_DEBUG_FILEPATH)
			if(NOT EXISTS ${ARG_LIBRARY_DEBUG_FILEPATH})
				message(FATAL_ERROR "The debug library file of the ${ARG_IMPORTED_TARGET_NAME} target was not found.")
			endif()

			if (${ARG_LIBRARY_LINKAGE} STREQUAL "SHARED" AND MSVC)
				set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
					IMPORTED_IMPLIB_DEBUG ${ARG_LIBRARY_DEBUG_FILEPATH}
					IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
				)
			else()
				set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
					IMPORTED_LOCATION_DEBUG ${ARG_LIBRARY_DEBUG_FILEPATH}
					IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
				)
			endif()
		endif()

		if("RelWithDebInfo" IN_LIST CMAKE_CONFIGURATION_TYPES)
			set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
				MAP_IMPORTED_CONFIG_RELWITHDEBINFO "RELWITHDEBINFO;RELEASE;"
			)
			message(STATUS "Mapping imported target ${ARG_IMPORTED_TARGET_NAME} configuration RelWithDebInfo to Release")
		endif()

		if("MinSizeRel" IN_LIST CMAKE_CONFIGURATION_TYPES)
			set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
				MAP_IMPORTED_CONFIG_MINSIZEREL "MINSIZEREL;RELEASE;"
			)
			message(STATUS "Mapping imported target ${ARG_IMPORTED_TARGET_NAME} configuration MinSizeRel to Release")
		endif()
	endif()

	if(DEFINED ARG_INCLUDE_DIR)
		target_include_directories(${ARG_IMPORTED_TARGET_NAME} INTERFACE ${ARG_INCLUDE_DIR})
	endif()
endfunction()

function(fetch_GLEW)
	set(TMP_CMAKE_POSITION_INDEPENDENT_CODE ${CMAKE_POSITION_INDEPENDENT_CODE})
	set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
	set(BUILD_UTILS OFF CACHE INTERNAL "")

	set(SRC_DIR "${FETCHCONTENT_BASE_DIR}/glew-src")
	set(PATCH_FILE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/glew_patch.json")

	FetchContent_Declare(
		glew
		URL	"https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip"
		SOURCE_SUBDIR	"build/cmake"
		PATCH_COMMAND	${CMAKE_COMMAND} -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/replace.cmake ${SRC_DIR} ${PATCH_FILE}
	)

	FetchContent_MakeAvailable(glew)

	add_library(GLEW::GLEW ALIAS glew_s)

	unset(BUILD_UTILS)
	set(CMAKE_POSITION_INDEPENDENT_CODE ${TMP_CMAKE_POSITION_INDEPENDENT_CODE})
endfunction()

function(fetch_Pugixml)
	set(SRC_DIR "${FETCHCONTENT_BASE_DIR}/pugixml-src")
	set(PATCH_FILE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/pugixml_patch.json")

	FetchContent_Declare(
		pugixml
		URL	"https://github.com/zeux/pugixml/releases/download/v1.12.1/pugixml-1.12.1.zip"
		PATCH_COMMAND	${CMAKE_COMMAND} -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/replace.cmake ${SRC_DIR} ${PATCH_FILE}
	)

	FetchContent_MakeAvailable(pugixml)

	add_library(PUGIXML::PUGIXML ALIAS pugixml)
endfunction()

function(fetch_Minidx9)
	FetchContent_Declare(
		minidx9
		URL	"https://github.com/hrydgard/minidx9/archive/7751cf7.zip"
	)

	FetchContent_GetProperties(minidx9)
	if(NOT minidx9_POPULATED)
		FetchContent_Populate(minidx9)
	endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(DIRECTX_LIB_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(DIRECTX_LIB_ARCH "x86")
	endif()

	create_imported_target(IMPORTED_TARGET_NAME "D3D9"
						   LIBRARY_LINKAGE "SHARED"
						   LIBRARY_FILEPATH "${minidx9_SOURCE_DIR}/Lib/${DIRECTX_LIB_ARCH}/d3d9.lib"
						   INCLUDE_DIR "${minidx9_SOURCE_DIR}/Include"
	)

	create_imported_target(IMPORTED_TARGET_NAME "D3DX9"
						   LIBRARY_LINKAGE "SHARED"
						   LIBRARY_FILEPATH "${minidx9_SOURCE_DIR}/Lib/${DIRECTX_LIB_ARCH}/d3dx9.lib"
						   INCLUDE_DIR "${minidx9_SOURCE_DIR}/Include"
	)
endfunction()

function(fetch_SDL)
	set(oneValueArgs RUNTIME_OUTPUT_DIR)
	cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN} )

	set(SDL_STATIC OFF CACHE INTERNAL "")
	set(SDL_SHARED ON  CACHE INTERNAL "")

	FetchContent_Declare(
		sdl2
		URL	"https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.0.20.zip"
	)

	FetchContent_MakeAvailable(sdl2)

	if (DEFINED ARG_RUNTIME_OUTPUT_DIR)
		set_target_properties(SDL2 PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY ${ARG_RUNTIME_OUTPUT_DIR}/$<0:>
				RUNTIME_OUTPUT_DIRECTORY ${ARG_RUNTIME_OUTPUT_DIR}/$<0:>
		)

		foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
			set_target_properties(SDL2 PROPERTIES
					LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${ARG_RUNTIME_OUTPUT_DIR}/$<0:>
					RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${ARG_RUNTIME_OUTPUT_DIR}/$<0:>
		)
		endforeach()
	endif()

	unset(SDL_SHARED)
	unset(SDL_STATIC)
endfunction()
