cmake_minimum_required(VERSION 3.20)

project(Todo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(Todo
    Todo.cpp
    resource.rc
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp  
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_win32.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
)


target_include_directories(Todo PRIVATE 
    external/imgui
    external/imgui/backends
)


target_link_libraries(Todo PRIVATE 
    d3d11
    d3dcompiler
)

if(MSVC)
    target_link_options(Todo PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
endif()

