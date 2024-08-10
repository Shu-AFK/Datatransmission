set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
set(imgui_dir "${CMAKE_SOURCE_DIR}/include/imgui")

add_definitions(-DDATATRANSMISSION_CLIENT_GUI)

include_directories(${imgui_dir} ${INCLUDE_DIR})

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

add_executable(Client_GUI
        main.cpp
        ${CMAKE_SOURCE_DIR}/Client/src/client.cpp
        style.cpp
)

target_link_libraries(Client_GUI PRIVATE
        imgui
        d3d11
        dwmapi
        d3dcompiler
        ws2_32
        ${LZ4_LIBRARY}
)