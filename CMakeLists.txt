cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BrainEngine)

find_package(fmt CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB_RECURSE IMGUI_SRC_DIR 
     "${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/buildtrees/imgui/src/*-docking-*/backends/imgui_impl_glfw.cpp"
)

if(IMGUI_SRC_DIR)
    get_filename_component(BACKEND_PATH "${IMGUI_SRC_DIR}" DIRECTORY)
    message(STATUS "Found ImGui Backends at: ${BACKEND_PATH}")
    
    set(IMGUI_BACKEND_SOURCES 
        "${BACKEND_PATH}/imgui_impl_glfw.cpp"
        "${BACKEND_PATH}/imgui_impl_opengl3.cpp"
    )
else()
    message(FATAL_ERROR "Could not locate ImGui backend sources in buildtrees.")
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_BACKEND_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    fmt::fmt
    SDL3::SDL3
    $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
    SDL3_ttf::SDL3_ttf
    imgui::imgui
    glad::glad
    glfw
    OpenGL::GL
    glm::glm
)
