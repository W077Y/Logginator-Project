cmake_minimum_required (VERSION 3.19)


set(target_name "TEST_FORMAT_LINE")
 message(STATUS "    -> ${target_name}")


add_executable(${target_name})

target_include_directories(${target_name}
 PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
)

target_sources(${target_name}
 PRIVATE "${TEST_MAIN_CPP}" 
 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/tst_format_line.cpp"
)

target_link_libraries(${target_name} PRIVATE UT_CATCH)

target_link_libraries(${target_name}
 PRIVATE Logginator-Lib
)


add_test(${target_name} ${target_name})



