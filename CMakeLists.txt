cmake_minimum_required(VERSION 3.19)

#set(CMAKE_VERBOSE_MAKEFILE on )

project("Logginator-Project"
 VERSION 0.0.0.0
 DESCRIPTION ""
 HOMEPAGE_URL ""
 LANGUAGES C CXX ASM
)

message(STATUS "########################")
message(STATUS "#          PROJECT NAME: ${CMAKE_PROJECT_NAME}")
message(STATUS "#   PROJECT DESCRIPTION: ${CMAKE_PROJECT_DESCRIPTION}")
message(STATUS "#           PROJECT URL: ${CMAKE_PROJECT_HOMEPAGE_URL}")
message(STATUS "#       PROJECT VERSION: ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}.${CMAKE_PROJECT_VERSION_TWEAK}")
message(STATUS "########################")

if(MSVC)
     add_compile_options(
       /W4
       /std:c++latest
       /Zi
     )
else()
     add_compile_options(
         "$<$<COMPILE_LANGUAGE:CXX>:-std=c++23>"
         "$<$<COMPILE_LANGUAGE:CXX>:-fstack-protector>"
         "$<$<COMPILE_LANGUAGE:CXX>:-fstack-usage>"
         "$<$<COMPILE_LANGUAGE:CXX>:-Wall>"
         "$<$<COMPILE_LANGUAGE:CXX>:-Wextra>"
         "$<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>"
         "$<$<COMPILE_LANGUAGE:CXX>:-Werror>"
     )
endif()

enable_testing()

add_subdirectory( "${CMAKE_CURRENT_LIST_DIR}/logginator-lib" )
add_subdirectory( "${CMAKE_CURRENT_LIST_DIR}/tests" )


