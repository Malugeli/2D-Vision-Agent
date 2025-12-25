cmake_minimum_required(VERSION 3.28)

# Vcpkg zwingen, die MinGW-Version zu nutzen
set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic")

project(MeinOpenCVProjekt CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

# --- DEBUG: Zeig mir, was du gefunden hast! ---
message(STATUS "DEBUG: OpenCV Include Ordner: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "DEBUG: OpenCV Bibliotheken: ${OpenCV_LIBS}")
# ----------------------------------------------

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(app main.cpp)

target_link_libraries(app PRIVATE ${OpenCV_LIBS} stdc++exp)