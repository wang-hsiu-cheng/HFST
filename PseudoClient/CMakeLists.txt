cmake_minimum_required(VERSION 3.10)
project(PseudoClient)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include path
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create library
add_library(TaifexOrder STATIC
  src/TaifexOrderClient.cpp
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

add_library(ClientMessenger src/ClientMessenger.cpp)
target_link_libraries(ClientMessenger
  TaifexOrder
  TaifexOrderUtility
  TaifexORGenerator
  ${ARMADILLO_LIBRARIES})

# Create executable
add_executable(TaifexOrderClient src/TaifexOrderClient.cpp)
target_link_libraries(TaifexOrderClient 
  TaifexOrderUtility
  TaifexORGenerator
  ClientMessenger
  ${ARMADILLO_LIBRARIES})
