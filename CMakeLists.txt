project(clunk)
cmake_minimum_required(VERSION 2.6)

# Additional cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Clunk version
set(clunk_VERSION_MAJOR 1)
set(clunk_VERSION_MINOR 0)

# SDL
find_package(SDL REQUIRED)
option(WITH_SSE "Use highly optimized SSE FFT/MDCT routines" false)

# libsamplerate
find_package(libsamplerate)
option(WITH_LIBSAMPLERATE "Enable libsamplerate support" ${LIBSAMPLERATE_FOUND})
configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h @ONLY)

# Add package libraries to list
set(INCLUDE_DIRS ${SDL_INCLUDE_DIR})
set(LIBRARIES ${SDL_LIBRARY})
if(WITH_LIBSAMPLERATE)
	list(APPEND INCLUDE_DIRS ${LIBSAMPLERATE_INCLUDE_DIR})
	list(APPEND LIBRARIES ${LIBSAMPLERATE_LIBRARIES})
endif(WITH_LIBSAMPLERATE)

# Compiler defintions
if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WINDOWS)
endif(WIN32)

add_definitions(-DCLUNKAPI=DLLEXPORT)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Source files
set(SOURCES
	buffer.cpp
	clunk_ex.cpp
	context.cpp
	distance_model.cpp
	kemar.c
	logger.cpp
	object.cpp
	sample.cpp
	sdl_ex.cpp
	source.cpp
	stream.cpp
	clunk_c.cpp
)
set(PUBLIC_HEADERS
	buffer.h
	clunk.h
	clunk_assert.h
	context.h
	distance_model.h
	export_clunk.h
	fft_context.h
	locker.h
	logger.h
	mdct_context.h
	object.h
	sample.h
	source.h
	sse_fft_context.h
	stream.h
	v3.h
	clunk_c.h
	${CMAKE_BINARY_DIR}/config.h
)

if(WITH_SSE)
	set(SOURCES ${SOURCES} sse_fft_context.cpp)
	add_definitions(-DCLUNK_USES_SSE)
endif(WITH_SSE)

include_directories(${INCLUDE_DIRS})
add_library(clunk SHARED ${SOURCES})
target_link_libraries(clunk ${LIBRARIES})

install(TARGETS clunk DESTINATION lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION include/clunk)

add_executable(clunk_test test.cpp)
target_link_libraries(clunk_test clunk)
add_executable(clunk_c_test test_c.c)
target_link_libraries(clunk_c_test clunk)
