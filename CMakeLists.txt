cmake_minimum_required(VERSION 3.28)

# WICHTIG: Hier ändern wir auf den Windows-Standard (MSVC)
# "x64-windows" ist der Standard für Visual Studio
set(VCPKG_TARGET_TRIPLET "x64-windows")

project(MeinOpenCVProjekt CXX)

# C++23 Standard aktivieren
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pakete finden
find_package(OpenCV REQUIRED)
find_package(wil CONFIG REQUIRED)

# Debug-Info (optional, kannst du drin lassen)
message(STATUS "DEBUG: OpenCV Include Ordner: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "DEBUG: OpenCV Bibliotheken: ${OpenCV_LIBS}")

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(app main.cpp)

# WICHTIG: "stdc++exp" entfernen!
# Das war eine Spezial-Bibliothek für GCC (MinGW). MSVC braucht die nicht.
target_link_libraries(app PRIVATE ${OpenCV_LIBS} d3d11 dxgi user32 WIL::WIL)