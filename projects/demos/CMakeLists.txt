# ############################################# #
#                                               #
#         SPARK Particle Engine : Demos         #
#                 Solution file                 #
#                                               #
# ############################################# #



# Solution
# ###############################################
cmake_minimum_required(VERSION 2.8)
project(SPARK_Demos)
set(DEMOS_USE_STATIC_LIBS OFF CACHE BOOL "Store whether to link against static (ON) or dynamic (OFF) SPARK libraries")



# Projects
# ###############################################
add_subdirectory(flakes flakes)
add_subdirectory(collision collision)
#add_subdirectory(test_irr test_irr)
add_subdirectory(test test)
add_subdirectory(explosion explosion)



# Dependencies
# ###############################################