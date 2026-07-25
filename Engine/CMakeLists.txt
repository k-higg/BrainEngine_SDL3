################
#    Engine    #
################

find_package(fmt CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/ImGui/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ImGui/*.h"
)

add_library(Engine STATIC)

target_include_directories(Engine
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_sources(Engine PRIVATE
        ${SOURCES}
)

target_link_libraries(Engine PUBLIC
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