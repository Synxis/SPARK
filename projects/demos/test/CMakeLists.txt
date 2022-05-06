# ############################################# #
#                                               #
#         SPARK Particle Engine : Demos         #
#                  Simple Test                  #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(Test_Simple)



# Sources
# ###############################################
set(SRC_FILES
	${SPARK_DIR}/demos/src/SPKTest.cpp
)



# Build step
# ###############################################
add_executable(Test_Simple
	${SRC_FILES}
)

if(MINGW)
	target_link_libraries(Test_Simple mingw32)
endif()

if(MINGW OR MSVC)
	target_link_libraries(Test_Simple SDL2::SDL2main)
endif()

target_link_libraries(Test_Simple
	SPARK_OpenGL
	SDL2::SDL2
)

if(${SPARK_STATIC_BUILD})
	if(MINGW)
		# The '-static' flag is not required, it is simply
		# set for ease of use when running the examples.
		# The '-fstack-protector' is needed when using a
		# recent mingw64 version due to the use of _FORTIFY_SOURCE.
		target_link_options(Test_Simple PRIVATE "-static" "-fstack-protector")
	endif()
else()
	target_compile_definitions(Test_Simple PRIVATE SPK_IMPORT)

	if(UNIX AND NOT APPLE)
		target_link_libraries(Test_Simple OpenGL::GLU)
	endif()
endif()

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set_target_properties(Test_Simple PROPERTIES DEBUG_POSTFIX _debug)
else()
	set_target_properties(Test_Simple PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()

if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
	set_target_properties(Test_Simple PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${SPARK_DIR}/demos/bin
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/demos/bin
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/demos/bin
	)
endif()
