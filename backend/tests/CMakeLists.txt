# backend/tests/CMakeLists.txt

# Crear el ejecutable de tests
add_executable(hestia_tests test_BKTEngine.cpp)

target_link_libraries(hestia_tests 
    PRIVATE 
        hestia_backend 
        Catch2::Catch2WithMain
)

include(CatchShutdownTests)
catch_discover_tests(hestia_tests)
