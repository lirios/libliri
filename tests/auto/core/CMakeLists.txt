qt6_add_executable(tst_liri_core tst_qobjectlistmodel.cpp)

target_link_libraries(tst_liri_core PRIVATE Qt6::Test Liri::Models)

add_test(
    NAME tst_liri_core
    COMMAND tst_liri_core
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
