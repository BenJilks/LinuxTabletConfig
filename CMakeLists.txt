cmake_minimum_required(VERSION 2.6)
project(LinuxTabletConfig)
find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)

set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR}/include)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
pkg_check_modules(XRANDR REQUIRED xrandr)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

include_directories(${GTK3_INCLUDE_DIRS} ${GTKMM3_INCLUDE_DIRS} ${XRANDR_INCLUDE_DIRS} ${X11_INCLUDE_DIR} ${JSONCPP_INCLUDE_DIR})
link_directories(${GTK3_LIBRARY_DIRS} ${GTKMM3_LIBRARY_DIRS} ${XRANDR_LIBRARY_DIRS} ${X11_LIBRARIES} ${JSONCPP_LIB_DIRS})
add_definitions(${GTK3_CFLAGS} ${GTKMM3_CFLAGS} ${XRANDR_CFLAGS})

file(GLOB all_SRCS
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(LinuxTabletConfig ${all_SRCS})
target_link_libraries(LinuxTabletConfig ${GTK3_LIBRARIES} ${GTKMM3_LIBRARIES} ${XRANDR_LIBRARIES} ${JSONCPP_LIBRARIES} 
	${X11_LIBRARIES} ${X11_Xi_LIB})

install(TARGETS LinuxTabletConfig RUNTIME DESTINATION bin)
install(FILES LinuxTabletConfig.desktop DESTINATION /usr/share/applications)

