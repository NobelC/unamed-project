# backend/tests/CMakeLists.txt

# Añadimos test_MABEngine.cpp a la lista de fuentes
add_executable(hestia_tests 
    test_BKTEngine.cpp
    test_MABEngine.cpp 
)

target_link_libraries(hestia_tests 
    PRIVATE 
        hestia_backend 
        Catch2::Catch2WithMain
)

# Nota: Si usas Catch2 v3 instalado vía sistema (pacman), 
# 'include(Catch)' suele funcionar directamente sin manipular CMAKE_MODULE_PATH.
include(Catch)
catch_discover_tests(hestia_tests)
