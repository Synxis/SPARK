# ############################################# #
#                                               #
#             SPARK Particle Engine             #
#                  Core Module                  #
#                                               #
# ############################################# #



# Project declaration
# ###############################################
cmake_minimum_required(VERSION 2.8)
project(SPARK_Core)
set(SPARK_STATIC_BUILD OFF CACHE BOOL "Store whether SPARK is built as a static library (ON) or a dynamic one OFF)")



# Sources
# ###############################################
set(SPARK_DIR ../../..)
get_filename_component(SPARK_DIR ${SPARK_DIR}/void REALPATH)
get_filename_component(SPARK_DIR ${SPARK_DIR} PATH)

file(GLOB_RECURSE TMP_SRC_FILES
	${SPARK_DIR}/include/Core/*.h
	${SPARK_DIR}/include/Extensions/*.h
)
foreach(f ${TMP_SRC_FILES})
	file(RELATIVE_PATH SRCGR ${SPARK_DIR}/include ${f})
	string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
	string(REPLACE / \\ SRCGR ${SRCGR})
	source_group(Headers\\${SRCGR} FILES ${f})
endforeach()
set(SRC_FILES ${TMP_SRC_FILES})

file(GLOB_RECURSE TMP_SRC_FILES
	${SPARK_DIR}/src/Core/*.cpp
	${SPARK_DIR}/src/Extensions/*.cpp
)
foreach(f ${TMP_SRC_FILES})
	file(RELATIVE_PATH SRCGR ${SPARK_DIR}/src ${f})
	string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
	string(REPLACE / \\ SRCGR ${SRCGR})
	source_group(Sources\\${SRCGR} FILES ${f})
endforeach()
set(SRC_FILES
	${SPARK_DIR}/include/SPARK.h
	${SPARK_DIR}/include/SPARK_Core.h
	${SRC_FILES}
	${TMP_SRC_FILES}
)
source_group(Headers FILES
	${SPARK_DIR}/include/SPARK.h
	${SPARK_DIR}/include/SPARK_Core.h
)



# Build step
# ###############################################
set(SPARK_GENERATOR "(${CMAKE_SYSTEM_NAME}@${CMAKE_GENERATOR})")
include_directories(${SPARK_DIR}/include)
include_directories(${SPARK_DIR}/external/pugixml/src)
link_directories(${SPARK_DIR}/lib/${SPARK_GENERATOR}/external/pugixml)
if(${SPARK_STATIC_BUILD})
	set(SPARK_OUTPUT_TYPE static)
	add_library(SPARK_Core STATIC ${SRC_FILES})
else()
	add_definitions(-DSPK_CORE_EXPORT)
	set(SPARK_OUTPUT_TYPE dynamic)
	add_library(SPARK_Core SHARED ${SRC_FILES})
endif()
if(MSVC)
	set_target_properties(SPARK_Core PROPERTIES COMPILE_FLAGS "/fp:fast")
endif()
target_link_libraries(SPARK_Core
	debug pugixml_d
	optimized pugixml
)
set_target_properties(SPARK_Core PROPERTIES
	OUTPUT_NAME SPARK
	DEBUG_POSTFIX _debug
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

INSTALL(
    TARGETS SPARK_Core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
    
    
set(NIUBI_SETUP_HEADERS
${CMAKE_SOURCE_DIR}/include/SPARK.h
${CMAKE_SOURCE_DIR}/include/SPARK_Core.h
${CMAKE_SOURCE_DIR}/include/SPARK_DX9.h
${CMAKE_SOURCE_DIR}/include/SPARK_GL.h
${CMAKE_SOURCE_DIR}/include/SPARK_IRR.h
)

set(NIUBI_SETUP_Core_HEADERS
${CMAKE_SOURCE_DIR}/include/Core/SPK_Action.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_ArrayData.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Attributes.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_ClassDescription.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Color.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_ConnectionIterators.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Connections.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Controller.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Controls.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_DataHandler.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_DataSet.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_DEF.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_DescriptionDefines.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Emitter.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Enum.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Factory.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Getters.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Group.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Interpolator.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Iterator.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Logger.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_MemoryTracer.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Meta.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Modifier.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Object.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Octree.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Particle.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Reference.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_RenderBuffer.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Renderer.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Setters.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_StaticDescription.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_System.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Traits.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Transform.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Transformable.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_TypeOperations.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Types.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Vector3D.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_Zone.h
${CMAKE_SOURCE_DIR}/include/Core/SPK_ZonedModifier.h
)
set(NIUBI_SETUP_Core_IO_HEADERS
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Buffer.h
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Context.h
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Deserializer.h
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Loader.h
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Manager.h
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Saver.h
${CMAKE_SOURCE_DIR}/include/Core/IO/SPK_IO_Serializer.h
)
set(NIUBI_SETUP_Rendering_OpenGL_HEADERS
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_Buffer.h
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_DEF.h
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_LineRenderer.h
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_LineTrailRenderer.h
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_PointRenderer.h
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_QuadRenderer.h
${CMAKE_SOURCE_DIR}/include/Rendering/OpenGL/SPK_GL_Renderer.h
)
set(NIUBI_SETUP_Extensions_Actions_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/Actions/SPK_ActionSet.h
${CMAKE_SOURCE_DIR}/include/Extensions/Actions/SPK_SpawnParticlesAction.h
)
set(NIUBI_SETUP_Extensions_Emitters_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/Emitters/SPK_NormalEmitter.h
${CMAKE_SOURCE_DIR}/include/Extensions/Emitters/SPK_RandomEmitter.h
${CMAKE_SOURCE_DIR}/include/Extensions/Emitters/SPK_SphericEmitter.h
${CMAKE_SOURCE_DIR}/include/Extensions/Emitters/SPK_StaticEmitter.h
${CMAKE_SOURCE_DIR}/include/Extensions/Emitters/SPK_StraightEmitter.h
)
set(NIUBI_SETUP_Extensions_Interpolators_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/Interpolators/SPK_DefaultInitializer.h
${CMAKE_SOURCE_DIR}/include/Extensions/Interpolators/SPK_GraphInterpolator.h
${CMAKE_SOURCE_DIR}/include/Extensions/Interpolators/SPK_RandomInitializer.h
${CMAKE_SOURCE_DIR}/include/Extensions/Interpolators/SPK_RandomInterpolator.h
${CMAKE_SOURCE_DIR}/include/Extensions/Interpolators/SPK_SimpleInterpolator.h
)
set(NIUBI_SETUP_Extensions_IOConverters_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/IOConverters/SPK_IO_SPKCommon.h
${CMAKE_SOURCE_DIR}/include/Extensions/IOConverters/SPK_IO_SPKLoader.h
${CMAKE_SOURCE_DIR}/include/Extensions/IOConverters/SPK_IO_SPKSaver.h
${CMAKE_SOURCE_DIR}/include/Extensions/IOConverters/SPK_IO_XMLLoader.h
${CMAKE_SOURCE_DIR}/include/Extensions/IOConverters/SPK_IO_XMLSaver.h
)
set(NIUBI_SETUP_Extensions_Modifiers_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_BasicModifiers.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_Collider.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_Destroyer.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_EmitterAttacher.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_LinearForce.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_Obstacle.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_PointMass.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_RandomForce.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_Rotator.h
${CMAKE_SOURCE_DIR}/include/Extensions/Modifiers/SPK_Vortex.h
)

set(NIUBI_SETUP_Extensions_Renderers_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/Renderers/SPK_LineRenderBehavior.h
${CMAKE_SOURCE_DIR}/include/Extensions/Renderers/SPK_Oriented3DRenderBehavior.h
${CMAKE_SOURCE_DIR}/include/Extensions/Renderers/SPK_PointRenderBehavior.h
${CMAKE_SOURCE_DIR}/include/Extensions/Renderers/SPK_QuadRenderBehavior.h
)

set(NIUBI_SETUP_Extensions_Zones_HEADERS
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Line.h
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Box.h
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Cylinder.h
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Plane.h
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Point.h
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Ring.h
${CMAKE_SOURCE_DIR}/include/Extensions/Zones/SPK_Sphere.h
)


INSTALL(
    FILES ${NIUBI_SETUP_HEADERS}
    DESTINATION include
)

INSTALL(
    FILES ${NIUBI_SETUP_Core_HEADERS}
    DESTINATION include/Core
)
INSTALL(
    FILES ${NIUBI_SETUP_Core_IO_HEADERS}
    DESTINATION include/Core/IO
)
INSTALL(
    FILES ${NIUBI_SETUP_Rendering_OpenGL_HEADERS}
    DESTINATION include/Rendering/OpenGL
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_Actions_HEADERS}
    DESTINATION include/Extensions/Actions
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_Emitters_HEADERS}
    DESTINATION include/Extensions/Emitters
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_Interpolators_HEADERS}
    DESTINATION include/Extensions/Interpolators
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_IOConverters_HEADERS}
    DESTINATION include/Extensions/IOConverters
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_Modifiers_HEADERS}
    DESTINATION include/Extensions/Modifiers
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_Renderers_HEADERS}
    DESTINATION include/Extensions/Renderers
)
INSTALL(
    FILES ${NIUBI_SETUP_Extensions_Zones_HEADERS}
    DESTINATION include/Extensions/Zones
)




