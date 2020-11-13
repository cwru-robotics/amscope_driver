cmake_minimum_required(VERSION 2.8.3)
project(amscope_driver)

find_package(catkin REQUIRED COMPONENTS
	roscpp
)

include_directories(
	${catkin_INCLUDE_DIRS}
	${catkin_INCLUDES}
)

catkin_package()


add_executable(cam_driver_node src/cam_driver.cpp)

add_custom_command(
	TARGET cam_driver_node PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/lib/libnncam.so
		${CATKIN_DEVEL_PREFIX}/lib/libnncam.so
)

target_link_libraries(cam_driver_node
	${catkin_LIBRARIES}
	${OpenCV_LIBRARIES}
	nncam
)
