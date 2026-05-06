cmake_minimum_required(VERSION 3.28)
project(YgoDeckBuilder CXX)

# C++23 aktivieren
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pakete finden
find_package(OpenCV REQUIRED)
find_package(wil CONFIG REQUIRED)

# Executable erstellen (Darf nur EINMAL im File stehen!)
add_executable(${PROJECT_NAME} main.cpp)

# Alles verlinken (OpenCV + WIL + DirectX)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${OpenCV_LIBS} 
    WIL::WIL
    d3d11
    dxgi
    user32
)