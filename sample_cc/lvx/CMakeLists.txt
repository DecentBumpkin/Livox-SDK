cmake_minimum_required(VERSION 3.0)

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/sample_cc/lvx/external/rpavlik-cmake-modules-fe2273")

include(CreateLaunchers)
include(MSVCMultipleProcessCompile) 

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
    external/AntTweakBar-1.16/include/
    external/glfw-3.1.2/include/
    external/glm-0.9.7.1/
    external/glew-1.13.0/include/
    external/assimp-3.0.1270/include/
    external/bullet-2.81-rev2613/src/
    .
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set(DEMO_NAME lvx_sample_cc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(${DEMO_NAME} 
    main.cpp 
    lds_lvx.cpp 
    lds.cpp 
    lvx_file.cpp
	common/shader.cpp
	common/shader.hpp
    shader/SimpleFragmentShader.fragmentshader
	shader/SimpleVertexShader.vertexshader
)

if (WIN32)
add_custom_command(TARGET ${DEMO_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third_party/apr/bin/libapr-1.dll"
        $<TARGET_FILE_DIR:${DEMO_NAME}>)
endif()

target_link_libraries(${DEMO_NAME}
    PRIVATE
    ${PROJECT_NAME}_static
    ${ALL_LIBS}
    )

# Xcode and Visual working directories
set_target_properties(${DEMO_NAME} PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
create_target_launcher(${DEMO_NAME} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
create_target_launcher(${DEMO_NAME} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
#create_default_target_launcher(${DEMO_NAME} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}") 