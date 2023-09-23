cmake_minimum_required(VERSION 3.11.4)
project (3dVis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

# Libraries and includes

find_package(OpenMP REQUIRED)
set(BASE_LIBS ${BASE_LIBS} glut Xi X11 Xext GL GLU CACHE INTERNAL "")
set(BASE_INCLUDES src CACHE INTERNAL "")
set(BASE_COMPILEFLAGS "-fopenmp" CACHE INTERNAL "")

# Base source files

set(BASE_SOURCES ${BASE_SOURCES} src/Graph.cpp src/Node.cpp src/SceneParameters.cpp src/SpaceGrid.cpp src/Vector.cpp src/arcball/arcball.cpp src/cnf3d2.cpp CACHE INTERNAL "")

# Executables

add_executable(cnf3d2 ${BASE_SOURCES})
target_include_directories(cnf3d2 PRIVATE ${BASE_INCLUDES})
target_compile_options(cnf3d2 PRIVATE ${BASE_COMPILEFLAGS})
target_link_libraries(cnf3d2 PRIVATE ${BASE_LIBS} OpenMP::OpenMP_CXX)
