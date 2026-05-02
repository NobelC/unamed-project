#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../include/PersistenceLayer.hpp"
#include <cstdio>

using namespace hestia::persistence;
using namespace hestia::bkt;
using namespace hestia::mab;

// Helper: Crea la base de datos con el schema necesario y PRAGMA user_version=1
static std::unique_ptr<PersistenceLayer> createTestDB(const std::string& path) {
    // Primero creamos las tablas manualmente
    sqlite3* raw_db;
    sqlite3_open(path.c_str(), &raw_db);

    const char* schema = R"(
        PRAGMA user_version = 5;

        CREATE TABLE IF NOT EXISTS skill_state (
            student_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            p_learn_operative REAL NOT NULL,
            p_learn_theorical REAL NOT NULL,
            p_transition REAL NOT NULL,
            p_slip REAL NOT NULL,
            p_guess REAL NOT NULL,
            p_forget REAL NOT NULL,
            avg_response_time REAL NOT NULL,
            last_practice_time INTEGER NOT NULL,
            PRIMARY KEY (student_id, skill_id)
        ) WITHOUT ROWID;

        CREATE TABLE IF NOT EXISTS method_state (
            student_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            method_id INTEGER NOT NULL,
            attempts INTEGER NOT NULL DEFAULT 0,
            successes INTEGER NOT NULL DEFAULT 0,
            PRIMARY KEY (student_id, skill_id, method_id)
        ) WITHOUT ROWID;

        CREATE TABLE IF NOT EXISTS response_log (
            log_id INTEGER PRIMARY KEY,
            student_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            method_id INTEGER NOT NULL,
            timestamp INTEGER NOT NULL,
            is_correct INTEGER NOT NULL,
            response_ms INTEGER NOT NULL,
            p_learn REAL DEFAULT 0.0
        );

        CREATE TABLE IF NOT EXISTS srs_state (
            student_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            correct_streak INTEGER NOT NULL DEFAULT 0,
            next_review INTEGER NOT NULL DEFAULT 0,
            PRIMARY KEY (student_id, skill_id)
        ) WITHOUT ROWID;

        CREATE TABLE IF NOT EXISTS students (
            student_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            created_at INTEGER NOT NULL
        );
    )";

    char* err_msg = nullptr;
    sqlite3_exec(raw_db, schema, nullptr, nullptr, &err_msg);
    if (err_msg) sqlite3_free(err_msg);
    sqlite3_close(raw_db);

    return PersistenceLayer::create(path);
}

// Helper RAII para limpiar el archivo de test DB
struct TestDBGuard {
    std::string path;
    TestDBGuard(const std::string& p) : path(p) {}
    ~TestDBGuard() { std::remove(path.c_str()); }
};

TEST_CASE("PersistenceLayer: INSERT + SELECT skill_state", "[persistence]") {
    const std::string db_path = "test_persistence_skill.db";
    TestDBGuard guard(db_path);

    auto db = createTestDB(db_path);
    REQUIRE(db != nullptr);

    // Crear un SkillState con valores conocidos
    SkillState state;
    state.m_pLearn_operative = 0.75;
    state.m_pLearn_theorical = 0.80;
    state.m_pTransition = 0.08;
    state.m_pSlip = 0.03;
    state.m_pGuess = 0.04;
    state.m_pForget = 0.40;
    state.avg_response_time_ms = 1500.0;
    state.last_practice_time = std::chrono::system_clock::now();

    // Guardar
    auto result = db->saveInteraction(1, 10, state, METHOD::VISUAL, true, 1500, 5, 3);
    REQUIRE(result.success());

    // Cargar y verificar
    auto loaded = db->loadSkillState(1, 10);
    REQUIRE(loaded.has_value());

    REQUIRE_THAT(loaded->m_pLearn_operative, Catch::Matchers::WithinAbs(0.75, 0.001));
    REQUIRE_THAT(loaded->m_pLearn_theorical, Catch::Matchers::WithinAbs(0.80, 0.001));
    REQUIRE_THAT(loaded->m_pTransition, Catch::Matchers::WithinAbs(0.08, 0.001));
    REQUIRE_THAT(loaded->m_pSlip, Catch::Matchers::WithinAbs(0.03, 0.001));
    REQUIRE_THAT(loaded->m_pGuess, Catch::Matchers::WithinAbs(0.04, 0.001));
    REQUIRE_THAT(loaded->m_pForget, Catch::Matchers::WithinAbs(0.40, 0.001));
    REQUIRE_THAT(loaded->avg_response_time_ms, Catch::Matchers::WithinAbs(1500.0, 1.0));
}

TEST_CASE("PersistenceLayer: INSERT + SELECT method_state", "[persistence]") {
    const std::string db_path = "test_persistence_method.db";
    TestDBGuard guard(db_path);

    auto db = createTestDB(db_path);
    REQUIRE(db != nullptr);

    SkillState state;  // Valores por defecto
    state.last_practice_time = std::chrono::system_clock::now();

    // Guardar interacciones con distintos métodos
    auto r1 = db->saveInteraction(1, 5, state, METHOD::VISUAL, true, 1000, 10, 8);
    REQUIRE(r1.success());

    auto r2 = db->saveInteraction(1, 5, state, METHOD::AUDITORY, false, 2000, 5, 2);
    REQUIRE(r2.success());

    // Cargar method states
    auto methods = db->loadMethodStates(1, 5);

    // VISUAL (index 0) debe tener los últimos valores guardados
    REQUIRE(methods[0].count_attempts == 10);
    REQUIRE(methods[0].successes == 8);

    // AUDITORY (index 1)
    REQUIRE(methods[1].count_attempts == 5);
    REQUIRE(methods[1].successes == 2);

    // Los demás métodos deben estar en 0 (no se han usado)
    REQUIRE(methods[2].count_attempts == 0);
    REQUIRE(methods[3].count_attempts == 0);
    REQUIRE(methods[4].count_attempts == 0);
}

TEST_CASE("PersistenceLayer: Estado persiste tras cierre y reapertura", "[persistence]") {
    const std::string db_path = "test_persistence_reopen.db";
    TestDBGuard guard(db_path);

    // Primera sesión: crear y guardar
    {
        auto db = createTestDB(db_path);
        REQUIRE(db != nullptr);

        SkillState state;
        state.m_pLearn_operative = 0.65;
        state.m_pLearn_theorical = 0.70;
        state.m_pTransition = 0.09;
        state.m_pSlip = 0.02;
        state.m_pGuess = 0.06;
        state.m_pForget = 0.35;
        state.avg_response_time_ms = 2000.0;
        state.last_practice_time = std::chrono::system_clock::now();

        auto result = db->saveInteraction(42, 7, state, METHOD::KINESTHETIC, true, 2000, 3, 2);
        REQUIRE(result.success());
        // db se destruye aquí (cierra la conexión)
    }

    // Segunda sesión: reabrir y verificar que los datos persisten
    {
        auto db = PersistenceLayer::create(db_path);
        REQUIRE(db != nullptr);

        auto loaded = db->loadSkillState(42, 7);
        REQUIRE(loaded.has_value());

        REQUIRE_THAT(loaded->m_pLearn_operative, Catch::Matchers::WithinAbs(0.65, 0.001));
        REQUIRE_THAT(loaded->m_pLearn_theorical, Catch::Matchers::WithinAbs(0.70, 0.001));
        REQUIRE_THAT(loaded->m_pTransition, Catch::Matchers::WithinAbs(0.09, 0.001));

        auto methods = db->loadMethodStates(42, 7);
        REQUIRE(methods[2].count_attempts == 3);  // KINESTHETIC = index 2
        REQUIRE(methods[2].successes == 2);
    }
}
