# ############################################# #
#                                               #
#         SPARK Particle Engine : Demos         #
#                 Test Irrlicht                 #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 2.8)
project(Test_Irrlicht)
set(DEMOS_IRRLICHT_LIB "" CACHE STRING "Store the name of the Irrlicht library file (release)")
set(DEMOS_IRRLICHT_LIB_DEBUG "" CACHE STRING "Store the name of the Irrlicht library file (debug)")
set(DEMOS_IRRLICHT_INCLUDE_DIR "" CACHE PATH "Store the path to the 'include' folder of Irrlicht")
set(DEMOS_IRRLICHT_LIB_DIR "" CACHE PATH "Store the path to the library folder of Irrlicht, ie 'lib/<config>'")



# Sources
# ###############################################
set(SPARK_DIR ../../..)
get_filename_component(SPARK_DIR ${SPARK_DIR}/void REALPATH)
get_filename_component(SPARK_DIR ${SPARK_DIR} PATH)
set(SRC_FILES
	${SPARK_DIR}/demos/src/SPKTestIrrlicht.cpp
)



# Build step
# ###############################################
set(SPARK_GENERATOR "(${CMAKE_SYSTEM_NAME}@${CMAKE_GENERATOR})")
include_directories(${SPARK_DIR}/include)
include_directories(${DEMOS_IRRLICHT_INCLUDE_DIR})
link_directories(${DEMOS_IRRLICHT_LIB_DIR})
if(${DEMOS_USE_STATIC_LIBS})
	link_directories(${SPARK_DIR}/lib/${SPARK_GENERATOR}/static)
else()
	add_definitions(-DSPK_IMPORT)
	link_directories(${SPARK_DIR}/lib/${SPARK_GENERATOR}/dynamic)
endif()
add_executable(Test_Irrlicht
	${SRC_FILES}
)
target_link_libraries(Test_Irrlicht
	debug ${DEMOS_IRRLICHT_LIB_DEBUG}
	debug SPARK_debug
	debug SPARK_IRR_debug
	optimized ${DEMOS_IRRLICHT_LIB}
	optimized SPARK
	optimized SPARK_IRR	
)
set_target_properties(Test_Irrlicht PROPERTIES
	DEBUG_POSTFIX _debug
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SPARK_DIR}/demos/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SPARK_DIR}/demos/bin
)













