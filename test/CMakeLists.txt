
FIND_PACKAGE(Boost REQUIRED COMPONENTS unit_test_framework)

FIND_PROGRAM(cbc cbc)
FIND_PROGRAM(cplex cplex)
FIND_PROGRAM(scip scip)
FIND_PROGRAM(glpk glpsol)
FIND_PROGRAM(gurobi gurobi)
FIND_PROGRAM(minisat minisat)

SET(TESTS
    c_api
    cpp_api
    operators
    compute
    small_models
)

FOREACH(TEST IN LISTS TESTS)
    add_executable(${TEST}_test ${TEST}.cpp)
    target_link_libraries(${TEST}_test ${Boost_LIBRARIES} umo)
    add_test(${TEST} ${TEST}_test)
ENDFOREACH(TEST)

SET(LP_SOLVERS
    scip
    cbc
    gurobi
)

SET(SAT_SOLVERS
    scip
    cbc
    gurobi
    minisat
)

FOREACH(SOLVER IN LISTS LP_SOLVERS)
    IF(${SOLVER})
        set(TEST linearize_${SOLVER})
        message("Solver ${SOLVER} found (${${SOLVER}})")
        add_executable(${TEST}_test linearize.cpp)
        target_compile_definitions(${TEST}_test PRIVATE "-DSOLVER_PARAM=${SOLVER}")
        target_link_libraries(${TEST}_test ${Boost_LIBRARIES} umo)
        add_test(${TEST} ${TEST}_test)
    ELSE()
        message("Solver ${SOLVER} not found")
    ENDIF()
ENDFOREACH(SOLVER)

FOREACH(SOLVER IN LISTS SAT_SOLVERS)
    IF(${SOLVER})
        set(TEST satify_${SOLVER})
        message("Solver ${SOLVER} found (${${SOLVER}})")
        add_executable(${TEST}_test satify.cpp)
        target_compile_definitions(${TEST}_test PRIVATE "-DSOLVER_PARAM=${SOLVER}")
        target_link_libraries(${TEST}_test ${Boost_LIBRARIES} umo)
        add_test(${TEST} ${TEST}_test)
    ELSE()
        message("Solver ${SOLVER} not found")
    ENDIF()
ENDFOREACH(SOLVER)

