cmake_minimum_required(VERSION 3.5)

project(QPortAudio LANGUAGES CXX)




set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets  REQUIRED)
add_executable(QPortAudio
    port_audio_dev_info.cpp
    port_audio_dev_info.h
    port_audio_stream_settings.cpp
    port_audio_stream_settings.h
    port_audio_dev.cpp
    port_audio_dev.h
    qport_audio.h
    qport_audio_format.cpp
    qport_audio_format.h
    qport_audio_device_info.cpp
    qport_audio_device_info.h
    qport_audio_output.cpp
    qport_audio_output.h
    qport_audio_input.cpp
    qport_audio_input.h
    echo_test.cpp
    echo_test.h

    echo_window.cpp
    echo_window.ui
    echo_window.h
    main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets portaudio)


install(TARGETS QPortAudio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
