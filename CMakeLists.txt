cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)

project(Poke VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(BUILD_SHARED_LIBS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/src/*.h)

set(CONFIGURE_FILE ${CMAKE_BINARY_DIR}/generated/cmake.h)
configure_file(${CMAKE_SOURCE_DIR}/src/cmake.h.in ${CONFIGURE_FILE} @ONLY)
list(APPEND HEADERS ${CONFIGURE_FILE})

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_WARNING_AS_ERROR ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(SDL_EMSCRIPTEN_PERSISTENT_PATH /storage CACHE INTERNAL "")
set(SDL_SHARED OFF CACHE INTERNAL "")
set(SDL_STATIC ON CACHE INTERNAL "")
set(SDL_TEST_LIBRARY OFF CACHE INTERNAL "")
set(SDL_RENDER ON CACHE INTERNAL "")
set(SDL_AUDIO ON CACHE INTERNAL "")
set(SDL_GPU OFF CACHE INTERNAL "")
set(SDL_CAMERA OFF CACHE INTERNAL "")
set(SDL_INSTALL OFF CACHE INTERNAL "")

FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SDL3)

set(S_TRUCTURES_BUILD_TESTS OFF CACHE INTERNAL "")

FetchContent_Declare(S_tructures
    GIT_REPOSITORY https://github.com/Schwungus/S_tructures.git
    GIT_TAG 60d6adcf4e6a0fbf5cb66fee9a0f3776dc693d72
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(S_tructures)

set(S_FIXED_BUILD_TEST OFF CACHE INTERNAL "")

FetchContent_Declare(S_fixed
    GIT_REPOSITORY https://github.com/Schwungus/S_fixed.git
    GIT_TAG 8667493230501d7d1b1ba5e3454b263c228035df
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(S_fixed)

FetchContent_Declare(NutBlast
	GIT_REPOSITORY https://github.com/Schwungus/NutBlast.git
	GIT_TAG 44d2dfe08c5eccb9f25936659d4bb801e7b17457
	EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(NutBlast)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static S_tructures S_fixed NutBlast)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/generated)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

if(EMSCRIPTEN)
    target_link_options(${PROJECT_NAME} PRIVATE
        "SHELL:-sEXPORTED_RUNTIME_METHODS=HEAPU8,HEAPU32,HEAPF32"
        "SHELL:-sALLOW_MEMORY_GROWTH=1"

        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@"

        "SHELL:-g"
        "SHELL:-sASSERTIONS=2"
        "SHELL:-sNO_DISABLE_EXCEPTION_CATCHING")
endif()
