add_library(utf
    STATIC
        ${PROJECT_SOURCE_DIR}/utf/src/unify.cpp
    )

target_include_directories(utf
    PRIVATE
        ${PROJECT_SOURCE_DIR}/utf/include
    )

################################################
# UNIT TESTS
################################################

add_executable(u8b_u16b
        ${PROJECT_SOURCE_DIR}/utf/test/u8b_u16b.cpp
    )

target_include_directories(u8b_u16b
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u8b_u16b
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u8b_u16b
    PRIVATE
        util gtest
    )

################################################

add_executable(unicode_unit_test
        ${PROJECT_SOURCE_DIR}/utf/test/unicode_unit_test.cpp
    )

target_include_directories(unicode_unit_test
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(unicode_unit_test
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(unicode_unit_test
    PRIVATE
        util gtest util
    )

################################################

add_executable(u8_to_u8
        ${PROJECT_SOURCE_DIR}/utf/test/u8_to_u8.cpp
    )

target_include_directories(u8_to_u8
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u8_to_u8
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u8_to_u8
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u8_to_u16
        ${PROJECT_SOURCE_DIR}/utf/test/u8_to_u16.cpp
    )

target_include_directories(u8_to_u16
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u8_to_u16
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u8_to_u16
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u8_to_u32
        ${PROJECT_SOURCE_DIR}/utf/test/u8_to_u32.cpp
    )

target_include_directories(u8_to_u32
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u8_to_u32
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u8_to_u32
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u16_to_u8
        ${PROJECT_SOURCE_DIR}/utf/test/u16_to_u8.cpp
    )

target_include_directories(u16_to_u8
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u16_to_u8
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u16_to_u8
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u16_to_u16
        ${PROJECT_SOURCE_DIR}/utf/test/u16_to_u16.cpp
    )

target_include_directories(u16_to_u16
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u16_to_u16
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u16_to_u16
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u16_to_u32
        ${PROJECT_SOURCE_DIR}/utf/test/u16_to_u32.cpp
    )

target_include_directories(u16_to_u32
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u16_to_u32
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u16_to_u32
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u32_to_u8
        ${PROJECT_SOURCE_DIR}/utf/test/u32_to_u8.cpp
    )

target_include_directories(u32_to_u8
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u32_to_u8
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u32_to_u8
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u32_to_u16
        ${PROJECT_SOURCE_DIR}/utf/test/u32_to_u16.cpp
    )

target_include_directories(u32_to_u16
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u32_to_u16
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u32_to_u16
    PRIVATE
        util gtest utf
    )

################################################

add_executable(u32_to_u32
        ${PROJECT_SOURCE_DIR}/utf/test/u32_to_u32.cpp
    )

target_include_directories(u32_to_u32
    PRIVATE
        ${GTEST_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(u32_to_u32
    PRIVATE
        ${GTEST_LIBRARY_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/../util
    )

target_link_libraries(u32_to_u32
    PRIVATE
        util gtest utf
    )

################################################
# BENCHMARK TESTS
################################################

add_executable(unicode_benchmark
        ${PROJECT_SOURCE_DIR}/utf/benchmark/unicode_benchmark.cpp
    )

target_include_directories(unicode_benchmark
    PRIVATE
        ${BENCHMARK_INCLUDE_PATH}
        ${PROJECT_SOURCE_DIR}/util/include
        ${PROJECT_SOURCE_DIR}/utf/include
    )

target_link_directories(unicode_benchmark
    PRIVATE
        ${BENCHMARK_LIBRARY_PATH}
    )

target_link_libraries(unicode_benchmark
    PRIVATE
        benchmark utf parsers
    )

################################################

