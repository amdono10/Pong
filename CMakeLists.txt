# set minimum CMake version
cmake_minimum_required(VERSION 4.3 FATAL_ERROR)
# project name and language
project(Pong CXX)

# automatically add the current source and build directories to the include path
# this is useful in out-of-source buildes
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FetchContent)

FetchContent_Declare(SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG main
)

# add source for additional modules here
set(LIB_SOURCE
	game.hpp game.cpp)

# add source for Main program loop here
set(MAIN_SOURCE main.cpp)

# hard code path to sdl (FOR NOW) 
# list(APPEND CMAKE_PREFIX_PATH "C:/SDL/cmake")

# locate SDL package
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

# set game name/title here
# set(EXECUTABLE_NAME Pong)

# try to prevent accidental in-source builds, as these cause a lot of problems
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds not allowed. Remove any files created thus far and use a different directory for the build.")
endif()

# Compiler standardization
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# executable target and corresponding source file(s)
add_executable(game ${MAIN_SOURCE} ${LIB_SOURCE})

# link the SDL target (handles include directories and binaries)
target_link_libraries(game PRIVATE SDL3::SDL3)