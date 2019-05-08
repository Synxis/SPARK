# ############################################# #
#                                               #
#         SPARK Particle Engine : Demos         #
#                 Solution file                 #
#                                               #
# ############################################# #

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/")

find_package(OpenGL)
find_package(SDL)


add_subdirectory(projects/engine)
add_subdirectory(projects/demos)
