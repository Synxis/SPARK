# ############################################# #
#                                               #
#         SPARK Particle Engine : Demos         #
#                 Test Irrlicht                 #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 3.19)
project(Test_Irrlicht)



# Sources
# ###############################################
set(SRC_FILES
	${SPARK_DIR}/demos/src/SPKTestIrrlicht.cpp
)



# Build step
# ###############################################
add_executable(Test_Irrlicht ${SRC_FILES})

target_link_libraries(Test_Irrlicht
	SPARK_Irrlicht
)

if(${SPARK_STATIC_BUILD})
	if(MINGW)
		# The '-static' flag is not required, it is simply
		# set for ease of use when running the examples.
		target_link_options(Test_Irrlicht PRIVATE "-static")
	endif()
else()
	target_compile_definitions(Test_Irrlicht PRIVATE SPK_IMPORT)
endif()

if(${SPARK_IRRLICHT_USE_STATIC_LIBS})
	target_compile_definitions(Test_Irrlicht PRIVATE _IRR_STATIC_LIB_)
endif()

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set_target_properties(Test_Irrlicht PROPERTIES DEBUG_POSTFIX _debug)
else()
	set_target_properties(Test_Irrlicht PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()

if(${SPARK_USE_HARDCODED_RUNTIME_OUTPUT_DIRS})
	set_target_properties(Test_Irrlicht PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${SPARK_DIR}/demos/bin
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/demos/bin
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/demos/bin
	)
endif()
