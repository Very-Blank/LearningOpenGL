cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DGLFW_BUILD_DOCS OFF)

project(opengl)

file(GLOB ENGINECPP ${CMAKE_SOURCE_DIR}/engine/src/*.cpp)
file(GLOB ENGINEHHP ${CMAKE_SOURCE_DIR}/engine/include/*.hpp)

add_executable(${PROJECT_NAME} main.cpp ${ENGINECPP} ${ENGINEHPP})

message("ENGINECPP: ${ENGINECPP}")
message("ENGINEHHP: ${ENGINEHHP}")

add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/glad)

target_include_directories(${PROJECT_NAME}
	PUBLIC external/glfw/include
	PUBLIC external/glm
	PUBLIC external/glad/include
	PUBLIC external/stb_image
	PUBLIC engine/include
)

target_link_directories(${PROJECT_NAME}
	PRIVATE external/glfw/src
	PRIVATE external/glad/src
)

target_link_libraries(${PROJECT_NAME} glfw glad)
