cmake_minimum_required(VERSION 3.16)
project(search_engine)
include(FetchContent)
include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 17)
message("-- ENABLE_TESTS: " ${ENABLE_TESTS})

find_package(Threads REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

FetchContent_Declare(
        json
        URL
        https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

if(ENABLE_TESTS)
    FetchContent_Declare(
            googletest
            URL
            https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    FetchContent_MakeAvailable(googletest)

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    set(gtest_disable_pthreads on)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    enable_testing()

    add_executable(search_engine_test src/ConverterJSON.cpp tests/tests.cpp src/InvertedIndex.cpp src/SearchServer.cpp)
    target_link_libraries(search_engine_test PRIVATE nlohmann_json::nlohmann_json gtest_main Threads::Threads ${Boost_LIBRARIES})

    include(GoogleTest)
    gtest_discover_tests(search_engine_test)

endif()

add_executable(search_engine src/main.cpp src/ConverterJSON.cpp src/InvertedIndex.cpp src/SearchServer.cpp )
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json Threads::Threads ${Boost_LIBRARIES})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/search_engine DESTINATION .)


