set(imgui_dir "${CMAKE_SOURCE_DIR}/include/imgui")

include_directories(${imgui_dir})

file(GLOB_RECURSE IMGUI_SOURCES ${imgui_dir}/*.cpp)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC ${imgui_dir})

add_executable(Client_GUI main.cpp)

target_link_libraries(Client_GUI PRIVATE imgui)