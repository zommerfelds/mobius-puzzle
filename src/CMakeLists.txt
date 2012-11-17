set(sources
	main.cpp
	viewer.cpp
	appwindow.cpp
	algebra.cpp)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(game ${sources})
target_link_libraries(game ${GTKMM_LIBRARIES} ${GLEXT_LIBRARIES})
