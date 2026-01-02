##################
#   Brain APP    #
##################

file(GLOB SOURCES "Src/*.cpp" "Src/*.h")

add_executable(Brain)

target_sources(Brain PRIVATE ${SOURCES})
target_link_libraries(Brain PRIVATE Engine)
target_include_directories(Brain PRIVATE Src)

set_target_properties(Brain PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/App"
)
