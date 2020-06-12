cmake_minimum_required(VERSION 3.0)
project(cpyke)

add_subdirectory(pybind11)

include_directories(include)

# Library
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_library(cpyke SHARED src/cpyke.cpp)
target_link_libraries(cpyke PRIVATE pybind11::embed)

# Examples
add_executable(example-simple examples/example.cpp)
target_link_libraries(example-simple cpyke)
add_executable(example-plot examples/plotting.cpp)
target_link_libraries(example-plot cpyke)

install(TARGETS cpyke DESTINATION lib)
install(FILES include/cpyke.h include/cpyke_types.h DESTINATION include)
# Install cpyke package along with the library
install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install ${CMAKE_SOURCE_DIR})")
