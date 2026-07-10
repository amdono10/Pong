# set minimum CMake version
cmake_minimum_required(VERSION 4.3 FATAL_ERROR)
# project name and language
project(Pong CXX)

# automatically add the current source and build directories to the include path
# this is useful in out-of-source buildes
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# add source for additional modules here
set(LIB_SOURCE
	game.hpp game.cpp)

# add source for Main program loop here
set(MAIN_SOURCE main.cpp)

# hard code path to sdl (FOR NOW) 
# list(APPEND CMAKE_PREFIX_PATH "C:/SDL/cmake")

# Developer override options with -DSDL3_USE_SYSTEM=OFF build flag
# Developer Option 1: Use installed SDL3 if available
# Developer Option 2: Auto-download if not found
option(SDL3_USE_SYSTEM "Use system SDL3 if available" ON)
if(SDL3_USE_SYSTEM)
	# Try to find SDL3 first
	find_package(SDL3 QUIET)
endif()

if(NOT SDL3_FOUND)
	message(STATUS "SDL3 not found. Using FetchContent to download from Github and build...")

	include(FetchContent)
	FetchContent_Declare(SDL3
		GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
		GIT_TAG main
	)

	FetchContent_MakeAvailable(SDL3)
	message(STATUS "SDL3 downloaded and configured")
else()
	message(STATUS "SDL3 found: ${SDL3_DIR}")
endif()

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

# Copy SDL3.dll to the executable directory after build
add_custom_command(TARGET game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:SDL3::SDL3>
		$<TARGET_FILE_DIR:game>
	COMMENT "Copying SDL3.dll to executable directory"
)