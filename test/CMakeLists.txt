
FIND_PACKAGE(Boost REQUIRED COMPONENTS unit_test_framework)

SET(TESTS
    c_api
    cpp_api
    operators
    compute
)

FOREACH(TEST IN LISTS TESTS)
    add_executable(${TEST}_test ${TEST}.cpp)
    target_link_libraries(${TEST}_test ${Boost_LIBRARIES} libumo)
    add_test(${TEST} ${TEST}_test)
ENDFOREACH(TEST)

