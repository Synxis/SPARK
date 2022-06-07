# ############################################# #
#                                               #
#            SPARK Particle Engine              #
#                Solution file                  #
#                                               #
# ############################################# #

# Solution
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(SPARK)

include(cmake/UtilFunctions.cmake)

remove_incompatible_configuration_types()

file(REAL_PATH .. SPARK_DIR)
set(SPARK_GENERATOR "${CMAKE_SYSTEM_NAME}@${CMAKE_GENERATOR}")

set(SPARK_STATIC_BUILD							OFF CACHE BOOL "Store whether SPARK is built as a static library (ON) or a dynamic one OFF)")
set(SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS		ON  CACHE BOOL "Use the default runtime output directory for the built libraries")
set(SPARK_BUILD_DEMOS							OFF CACHE BOOL "Store whether to build the demos module")
set(SPARK_BUILD_IRRLICHT_MODULE					OFF CACHE BOOL "Store whether to build the Irrlicht module")

find_package(OpenGL REQUIRED)

# Projects
# ###############################################
add_subdirectory(engine/core)
add_subdirectory(engine/ogl)

if(${SPARK_BUILD_IRRLICHT_MODULE})
	add_subdirectory(engine/irr)
endif()

if(MINGW OR MSVC)
	add_subdirectory(engine/dx9)
endif()

if(${SPARK_BUILD_DEMOS})
	if(MINGW)
		if(${SPARK_STATIC_BUILD})
			find_library(ICONV_STATIC_LIB libiconv.a REQUIRED)
		else()
			find_library(ICONV_SHARED_LIB iconv REQUIRED)
		endif()
	endif()

	find_package(SDL2 QUIET)
	if(NOT SDL2_FOUND)
		if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
			fetch_SDL(RUNTIME_OUTPUT_DIR "${SPARK_DIR}/demos/bin")
		else()
			fetch_SDL()
		endif()
	endif()

	add_subdirectory(demos/flakes)
	add_subdirectory(demos/collision)
	add_subdirectory(demos/test)
	add_subdirectory(demos/explosion)
	if(${SPARK_BUILD_IRRLICHT_MODULE})
		add_subdirectory(demos/test_irr)
		add_subdirectory(demos/test_irr_controllers)
	endif()
endif()

if(MSVC)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	if(TARGET glew)
		set_target_properties(glew PROPERTIES FOLDER Deps)
	endif()

	if(TARGET glew_s)
		set_target_properties(glew_s PROPERTIES FOLDER Deps)
	endif()

	set_target_properties(pugixml-static PROPERTIES FOLDER Deps)

	if(${SPARK_BUILD_DEMOS})
		if(TARGET SDL2)
			set_target_properties(SDL2 SDL2main uninstall PROPERTIES FOLDER Deps)
		endif()
		set_target_properties(Collision Explosion Flakes Test_Simple PROPERTIES FOLDER Demos)
		if(${SPARK_BUILD_IRRLICHT_MODULE})
			set_target_properties(Test_Irrlicht Test_Irrlicht_Ctrl PROPERTIES FOLDER Demos)
		endif()
	endif()
endif()
