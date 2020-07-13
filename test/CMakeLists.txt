
FIND_PACKAGE(Boost REQUIRED COMPONENTS unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})

FIND_PROGRAM(cbc cbc)
FIND_PROGRAM(cplex cplex)
FIND_PROGRAM(scip scip)
FIND_PROGRAM(glpk glpsol)
FIND_PROGRAM(gurobi gurobi)
FIND_PROGRAM(minisat minisat)
FIND_PROGRAM(couenne couenne)

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

SET(MINLP_SOLVERS
    couenne
)

SET(MILP_SOLVERS
    scip
    cbc
    gurobi
    glpk
)

SET(SAT_SOLVERS
    scip
    cbc
    gurobi
    glpk
    minisat
)

FOREACH(SOLVER IN LISTS MINLP_SOLVERS)
    IF(${SOLVER})
        set(TEST solve_minlp_${SOLVER})
        message("Solver ${SOLVER} found (${${SOLVER}})")
        add_executable(${TEST}_test solve_minlp.cpp)
        target_compile_definitions(${TEST}_test PRIVATE "-DSOLVER_PARAM=${SOLVER}")
        target_link_libraries(${TEST}_test ${Boost_LIBRARIES} umo)
        add_test(${TEST} ${TEST}_test)
    ELSE()
        message("Solver ${SOLVER} not found")
    ENDIF()
ENDFOREACH(SOLVER)

FOREACH(SOLVER IN LISTS MILP_SOLVERS)
    IF(${SOLVER})
        set(TEST solve_milp_${SOLVER})
        message("Solver ${SOLVER} found (${${SOLVER}})")
        add_executable(${TEST}_test solve_milp.cpp)
        target_compile_definitions(${TEST}_test PRIVATE "-DSOLVER_PARAM=${SOLVER}")
        target_link_libraries(${TEST}_test ${Boost_LIBRARIES} umo)
        add_test(${TEST} ${TEST}_test)
    ELSE()
        message("Solver ${SOLVER} not found")
    ENDIF()
ENDFOREACH(SOLVER)

FOREACH(SOLVER IN LISTS SAT_SOLVERS)
    IF(${SOLVER})
        set(TEST solve_sat_${SOLVER})
        message("Solver ${SOLVER} found (${${SOLVER}})")
        add_executable(${TEST}_test solve_sat.cpp)
        target_compile_definitions(${TEST}_test PRIVATE "-DSOLVER_PARAM=${SOLVER}")
        target_link_libraries(${TEST}_test ${Boost_LIBRARIES} umo)
        add_test(${TEST} ${TEST}_test)
    ELSE()
        message("Solver ${SOLVER} not found")
    ENDIF()
ENDFOREACH(SOLVER)

