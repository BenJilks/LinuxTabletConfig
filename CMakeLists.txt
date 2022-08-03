cmake_minimum_required(VERSION 3.1.0)
project(LinuxTabletConfig LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

pkg_check_modules(XRANDR REQUIRED xrandr)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

include_directories(${XRANDR_INCLUDE_DIRS} ${X11_INCLUDE_DIR} ${JSONCPP_INCLUDE_DIR})
link_directories(${XRANDR_LIBRARY_DIRS} ${X11_LIBRARIES} ${JSONCPP_LIB_DIRS})
add_definitions(${XRANDR_CFLAGS})
add_executable(LinuxTabletConfig
    src/LinuxTabletConfig.cpp
    src/Window.cpp
    src/SettingsPanel.cpp
    src/Mapper.cpp
    src/MapperSettings.cpp
    src/DeviceManager.cpp
    src/Config.cpp
)

target_link_libraries(LinuxTabletConfig ${XRANDR_LIBRARIES})
target_link_libraries(LinuxTabletConfig ${JSONCPP_LIBRARIES})
target_link_libraries(LinuxTabletConfig ${X11_LIBRARIES} ${X11_Xi_LIB})
target_link_libraries(LinuxTabletConfig Qt5::Core Qt5::Gui Qt5::Widgets)
install(TARGETS LinuxTabletConfig RUNTIME DESTINATION bin)
install(FILES LinuxTabletConfig.desktop DESTINATION /usr/share/applications)

