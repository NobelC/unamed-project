#include <catch2/catch_test_macros.hpp>
#include "../include/SessionManager.hpp"

using namespace hestia::bkt;

TEST_CASE("SessionManager: Control de ciclo de vida", "[session]") {
    SessionManager manager;
    SkillState state;

    SECTION("startSession inicializa time_point y resetea P(T)") {
        state.m_pTransition = 0.99;  // Valor alterado

        manager.startSession(state);

        REQUIRE(state.session_start_time.time_since_epoch().count() > 0);
        REQUIRE(state.m_pTransition == DEFAULT_P_TRANSITION);
    }

    SECTION("endSession incrementa contador y limpia time_points") {
        state.session_count = 5;
        manager.startSession(state);
        manager.endSession(state);

        REQUIRE(state.session_count == 6);
        REQUIRE(state.session_start_time.time_since_epoch().count() == 0);
        // Bug fix #3: endSession también debe limpiar el steady_clock point
        REQUIRE(state.session_start_time_steady.time_since_epoch().count() == 0);
    }
}

TEST_CASE("SessionManager: Validacion de tiempo", "[session]") {
    SessionManager manager;

    SECTION("isResponseTimeAnomalous (5 minutos)") {
        REQUIRE_FALSE(manager.isResponseTimeAnomalous(1000.0));    // 1 segundo
        REQUIRE_FALSE(manager.isResponseTimeAnomalous(299000.0));  // 4.98 minutos
        REQUIRE(manager.isResponseTimeAnomalous(301000.0));        // > 5 minutos
    }

    SECTION("getSessionElapsedMinutes devuelve 0 si no hay sesion activa") {
        SkillState state;  // Sin startSession
        REQUIRE(manager.getSessionElapsedMinutes(state) == 0.0);
    }

    SECTION("getSessionElapsedMinutes calcula correctamente") {
        SkillState state;
        manager.startSession(state);

        // Bug fix #3: getSessionElapsedMinutes ahora usa session_start_time_steady.
        // Manipulamos el campo steady (no el system_clock) para simular 5 minutos pasados
        // sin tener que esperar con sleep.
        state.session_start_time_steady =
            std::chrono::steady_clock::now() - std::chrono::minutes(5);

        double elapsed = manager.getSessionElapsedMinutes(state);
        // Permite un pequeño margen por la diferencia de microsegundos en la ejecución
        REQUIRE(elapsed >= 4.9);
        REQUIRE(elapsed <= 5.1);
    }
}
