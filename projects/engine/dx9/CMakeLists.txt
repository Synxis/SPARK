# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                DirectX9 Module                #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(SPARK_DirectX9)

include(../../cmake/UtilFunctions.cmake)

set(SPARK_DIRECTX_DIR "" CACHE PATH "Store the path to the DirectX SDK (folder containing 'Lib' and 'Include')")
mark_as_advanced(SPARK_DIRECTX_DIR)

if(NOT SPARK_DIRECTX_DIR)
	fetch_Minidx9()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(DIRECTX_LIB_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(DIRECTX_LIB_ARCH "x86")
	endif()

	create_imported_target(IMPORTED_TARGET_NAME "D3D9"
						   LIBRARY_LINKAGE "SHARED"
						   LIBRARY_FILEPATH "${SPARK_DIRECTX_DIR}/Lib/${DIRECTX_LIB_ARCH}/d3d9.lib"
						   INCLUDE_DIR "${SPARK_DIRECTX_DIR}/Include"
	)

	create_imported_target(IMPORTED_TARGET_NAME "D3DX9"
						   LIBRARY_LINKAGE "SHARED"
						   LIBRARY_FILEPATH "${SPARK_DIRECTX_DIR}/Lib/${DIRECTX_LIB_ARCH}/d3dx9.lib"
						   INCLUDE_DIR "${SPARK_DIRECTX_DIR}/Include"
	)
endif()

# Sources
# ###############################################
file(GLOB_RECURSE SRC_FILES
	${SPARK_DIR}/include/SPARK_DX9.h
	${SPARK_DIR}/include/Rendering/DX9/*.h
	${SPARK_DIR}/src/Rendering/DX9/*.cpp
)


# Build step
# ###############################################
if(${SPARK_STATIC_BUILD})
	set(SPARK_OUTPUT_TYPE static)
	add_library(SPARK_DirectX9 STATIC ${SRC_FILES})
else()
	set(SPARK_OUTPUT_TYPE dynamic)
	add_library(SPARK_DirectX9 SHARED ${SRC_FILES})
	target_compile_definitions(SPARK_DirectX9 PRIVATE SPK_CORE_IMPORT SPK_DX9_EXPORT)
endif()

if(MINGW)
	# Don't prefix the DLL with 'lib'
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(MSVC)
	set_target_properties(SPARK_DirectX9 PROPERTIES COMPILE_FLAGS "/fp:fast")
endif()

target_link_libraries(SPARK_DirectX9
	SPARK_Core
	D3D9
	D3DX9
)

set_target_properties(SPARK_DirectX9 PROPERTIES
	OUTPUT_NAME SPARK_DX9
)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set_target_properties(SPARK_DirectX9 PROPERTIES
		DEBUG_POSTFIX _debug
	)
endif()

if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
	set_target_properties(SPARK_DirectX9 PROPERTIES
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
		add_custom_command(TARGET SPARK_DirectX9 POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SPARK_DirectX9> "${SPARK_DIR}/demos/bin"
		)
	endif()
endif()
