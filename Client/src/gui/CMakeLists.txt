set(imgui_dir "${CMAKE_SOURCE_DIR}/include/imgui")
message(${imgui_dir})

include_directories(${imgui_dir})

set(IMGUI_SOURCES
        ${imgui_dir}/imgui.cpp
        ${imgui_dir}/imgui_widgets.cpp
        ${imgui_dir}/imgui_draw.cpp
        ${imgui_dir}/imgui_tables.cpp
        ${imgui_dir}/backends/imgui_impl_win32.cpp
        ${imgui_dir}/backends/imgui_impl_dx11.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC ${imgui_dir})
include_directories(${INCLUDE_DIR})

add_executable(Client_GUI
        main.cpp
        ${CMAKE_SOURCE_DIR}/Server/src/helper.cpp
)

target_link_libraries(Client_GUI PRIVATE imgui d3d11 dwmapi d3dcompiler ws2_32 ${BOOST_LIBS})