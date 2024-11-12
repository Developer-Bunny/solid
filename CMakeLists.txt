cmake_minimum_required(VERSION 3.29)
project(solid)

set(CMAKE_CXX_STANDARD 14)

# Configuración de directorios de salida
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Opción para construir bibliotecas compartidas o estáticas
option(BUILD_SHARED_LIBS "Build libraries as shared instead of static" ON)

# Agregar SDL como subdirectorio
add_subdirectory(libs/SDL)

# Encontrar Vulkan
find_package(Vulkan REQUIRED)

# Recursos de Dear ImGui
set(IMGUI_FILES
        libs/imgui/imgui.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_sdl3.cpp
)

# Configurar el ejecutable con los archivos de Dear ImGui
add_executable(solid
        src/saver.cpp
        src/saver.h
        main.cpp
        ${IMGUI_FILES}
        src/VulkanRenderer.cpp
        src/VulkanRenderer.h
        src/DebugConfig.h
        src/DebugConfig.cpp
)

# Incluir directorios específicos para solid
target_include_directories(solid PRIVATE libs/SDL/include libs/imgui src)

# Enlazar SDL y otras librerías necesarias
target_link_libraries(solid PRIVATE SDL3::SDL3 Vulkan::Vulkan)

# Copiar automáticamente SDL3.dll en el directorio del ejecutable en Windows
if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(
            TARGET solid POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:solid>
            VERBATIM
    )
endif()
