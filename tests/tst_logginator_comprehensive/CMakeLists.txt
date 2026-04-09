cmake_minimum_required(VERSION 3.19)

set(target_name "tst_comprehensive")

add_executable(${target_name})

target_sources(${target_name}
 PRIVATE "${TEST_MAIN_CPP}" 
 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/tst_comprehensive.cpp"
)

target_link_libraries(${target_name} PRIVATE Logginator-Lib)
target_link_libraries(${target_name} PRIVATE UT_CATCH)

add_test(
 NAME ${target_name}
 COMMAND ${target_name}
)
