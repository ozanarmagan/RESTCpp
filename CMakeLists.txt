cmake_minimum_required(VERSION 3.12)

project(restcpp VERSION 1.0.0 LANGUAGES C CXX)



file(GLOB source_files CONFIGURE_DEPENDS "./src/*.cpp" "./test/*.cpp")

add_executable(restcpp ${source_files})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
endif()

if(WIN32)
    target_link_libraries(restcpp wsock32)
    target_link_libraries(restcpp stdc++fs)
else()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(restcpp pthread OpenSSL::SSL)
endif()

configure_file("test/index.html" "root/index.html" COPYONLY)