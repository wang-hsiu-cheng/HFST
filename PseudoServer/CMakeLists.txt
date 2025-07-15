cmake_minimum_required(VERSION 3.10)
project(PseudoServer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include path
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create library
add_library(TaifexOrder STATIC
  src/TaifexOrderServer.cpp
)
add_library(TaifexOrderUtility src/TaifexOrderUtility.cpp)
target_link_libraries(TaifexOrderUtility
  TaifexOrder
  ${ARMADILLO_LIBRARIES})

add_library(TaifexORGenerator src/TaifexORGenerator.cpp)
target_link_libraries(TaifexORGenerator
  TaifexOrder
  TaifexOrderUtility
  ${ARMADILLO_LIBRARIES})

add_library(ServerMessenger src/ServerMessenger.cpp)
target_link_libraries(ServerMessenger
  TaifexOrder
  TaifexOrderUtility
  ${ARMADILLO_LIBRARIES})

# Create executable
add_executable(TaifexOrderServer src/TaifexOrderServer.cpp)
target_link_libraries(TaifexOrderServer
  TaifexOrderUtility
  TaifexORGenerator
  ServerMessenger
  ${ARMADILLO_LIBRARIES})

add_executable(TestClient src/TestClient.cpp)