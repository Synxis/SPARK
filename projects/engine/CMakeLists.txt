# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                 Solution file                 #
#                                               #
# ############################################# #



# Solution
# ###############################################
cmake_minimum_required(VERSION 2.8)
project(SPARK)


set(SPARK_ENABLE_DX9 OFF CACHE BOOL "dx9")
set(SPARK_ENABLE_IRR OFF CACHE BOOL "irr")
set(SPARK_ENABLE_OGL ON CACHE BOOL "ogl")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )


IF(NOT SPARK_ENABLE_DX9)
    add_definitions(-DSPK_NO_DX9_INC)
ENDIF(NOT SPARK_ENABLE_DX9)

# Projects
# ###############################################
add_subdirectory(core core)
IF(SPARK_ENABLE_IRR)
    add_subdirectory(irr irr)
ENDIF(SPARK_ENABLE_IRR)
IF(SPARK_ENABLE_OGL)
    add_subdirectory(ogl ogl)
ENDIF(SPARK_ENABLE_OGL)
IF(SPARK_ENABLE_DX9)
    add_subdirectory(dx9 dx9)
ENDIF(SPARK_ENABLE_DX9)
add_subdirectory(../external external)



# Dependencies
# ###############################################
add_dependencies(SPARK_Core PugiXML)
IF(SPARK_ENABLE_OGL)
    add_dependencies(SPARK_OpenGL GLEW)
    add_dependencies(SPARK_OpenGL SPARK_Core)
ENDIF(SPARK_ENABLE_OGL)
IF(SPARK_ENABLE_DX9)
    add_dependencies(SPARK_DirectX9 SPARK_Core)
ENDIF(SPARK_ENABLE_DX9)
IF(SPARK_ENABLE_IRR)
    add_dependencies(SPARK_Irrlicht SPARK_Core)
ENDIF(SPARK_ENABLE_IRR)
