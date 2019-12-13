
FIND_PACKAGE(Boost REQUIRED COMPONENTS unit_test_framework)

SET(TEST_SOURCES
    c_api.cpp
)

add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests ${Boost_LIBRARIES} libumo)
add_test(UnitTest unit_tests)

