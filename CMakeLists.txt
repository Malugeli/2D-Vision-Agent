cmake_minimum_required(VERSION 3.28)
project(YgoDeckBuilder LANGUAGES CXX)

include(CTest)
add_library(deck_data src/deck_catalog.cpp src/deck_selection.cpp)
target_compile_features(deck_data PUBLIC cxx_std_23)
target_include_directories(deck_data PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(BUILD_TESTING)
    add_executable(deck_data_tests tests/deck_data_tests.cpp)
    target_link_libraries(deck_data_tests PRIVATE deck_data)
    add_test(NAME deck_data_tests COMMAND deck_data_tests "${CMAKE_CURRENT_SOURCE_DIR}/Pics")
endif()

if(WIN32)
    find_package(OpenCV REQUIRED)
    find_package(wil CONFIG REQUIRED)
    add_executable(${PROJECT_NAME}
        src/main.cpp src/application.cpp src/client_window.cpp src/screen_capture.cpp
        src/input_controller.cpp src/deck_builder.cpp src/hotkey.cpp)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE deck_data ${OpenCV_LIBS} WIL::WIL d3d11 dxgi user32)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
        target_compile_options(deck_data PRIVATE /utf-8)
    endif()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Pics" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Pics")
else()
    message(STATUS "Die Desktop-Anwendung benötigt Windows; nur Deckdaten und Tests werden gebaut.")
endif()
