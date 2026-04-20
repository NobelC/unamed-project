#include <chrono>
#include <cassert>

#include <catch2/catch_test_macros.hpp>
#include "../include/BKTEngine.hpp"



// Placeholder: Verifica que el framework y CTest están configurados correctamente
TEST_CASE("Unit test for BKTEngine", "[Behavioral testing of variables]") {
  

  hestia::bkt::SkillState state;
  double lambda = 0.5;
  hestia::bkt::BKTEngine Engine;
  using namespace std::chrono_literals;
  auto time = 10min;
  //P(t) respeta el piso despues del decay
  SECTION("P(T) respera el piso despues del decay"){
    state.session_start_time = std::chrono::system_clock::now() - time;
    Engine.updateTransitionDecay(state,lambda);
    REQUIRE(state.m_pTransition >= hestia::bkt::P_TRANSITION_FLOOR);
  }
  //P(F) no aplica si no ha pasado 48 horas
  SECTION("P(F) no aplica si no hasta pasado 48 horas"){
    double pL_antes = state.m_pLearn_operative;
    state.last_practice_time = std::chrono::system_clock::now() - 10min;
    Engine.applyForgetFactor(state);
    REQUIRE(state.m_pLearn_operative == pL_antes);
  
    state.last_practice_time = std::chrono::system_clock::now() - 49h;
    Engine.applyForgetFactor(state);
    REQUIRE(state.m_pLearn_operative < pL_antes);
  }

  //Valores fuera de los limites
  SECTION("Valor fuera de los limites establecidos"){
    state.m_pLearn_operative = 2.0;
    state.validationProbabilityRanges();
    REQUIRE(state.m_pLearn_operative == hestia::bkt::MAX_P_LEARN_OPERATIVE);
  }

  // P(L) sube con respuesta correcta
  SECTION("P(L) sube con respuesta correcta") {
    hestia::bkt::SkillState test_state;
    test_state.is_initialized = true;
    test_state.avg_response_time_ms = 1000.0;
    test_state.m_pLearn_operative = 0.2;
    double pL_antes = test_state.m_pLearn_operative;
    
    Engine.updateKnowledge(test_state, true, 1000.0, lambda);
    
    REQUIRE(test_state.m_pLearn_operative > pL_antes);
  }

  // P(L) baja con respuesta incorrecta
  SECTION("P(L) baja con respuesta incorrecta") {
    hestia::bkt::SkillState test_state;
    test_state.is_initialized = true;
    test_state.avg_response_time_ms = 1000.0;
    test_state.m_pLearn_operative = 0.5; // un valor medio para asegurar que pueda bajar
    double pL_antes = test_state.m_pLearn_operative;
    
    Engine.updateKnowledge(test_state, false, 1000.0, lambda);
    
    REQUIRE(test_state.m_pLearn_operative < pL_antes);
  }

  // Penalización lenta > penalización rápida
  // Nota: Lo implementé con `is_correct=true` porque BKTEngine actual solo aplica 
  // penalización de tiempo (omega) cuando la respuesta es correcta.
  SECTION("Penalizacion lenta > penalizacion rapida") {
    hestia::bkt::SkillState state_fast;
    state_fast.is_initialized = true;
    state_fast.total_attempts = 10;
    state_fast.avg_response_time_ms = 1000.0;
    state_fast.m_pLearn_operative = 0.2;

    hestia::bkt::SkillState state_slow;
    state_slow.is_initialized = true;
    state_slow.total_attempts = 10;
    state_slow.avg_response_time_ms = 1000.0;
    state_slow.m_pLearn_operative = 0.2;

    // Respuesta rápida (1000ms <= avg)
    Engine.updateKnowledge(state_fast, true, 1000.0, lambda);
    // Respuesta lenta (1800ms > avg)
    Engine.updateKnowledge(state_slow, true, 1800.0, lambda);

    // El caso lento debe tener un incremento menor de P(L) debido a la penalización (omega)
    REQUIRE(state_slow.m_pLearn_operative < state_fast.m_pLearn_operative);
  }

  // Anti-stall: racha corta NO desbloquea
  SECTION("Anti-stall: racha corta NO desbloquea") {
    hestia::bkt::SkillState test_state;
    test_state.is_initialized = true;
    test_state.m_pLearn_theorical = 0.21; 
    test_state.m_pLearn_operative = 0.20;
    // Dif 0.01 < margen 0.05: NO incrementa m_sustained_theorical_dominance
    
    // Simulamos un par de aciertos (racha corta)
    for (int i = 0; i < 2; ++i) {
        Engine.updateKnowledge(test_state, true, 1000.0, lambda);
    }
    
    REQUIRE(test_state.m_sustained_theorical_dominance == 0);
  }

  // Anti-stall: dominio sostenido SÍ desbloquea
  SECTION("Anti-stall: dominio sostenido SI desbloquea") {
    hestia::bkt::SkillState test_state;
    test_state.is_initialized = true;
    test_state.avg_response_time_ms = 1000.0;
    test_state.m_sustained_theorical_dominance = 0;
    
    // Forzamos que el teórico supere al operativo consistentemente durante varios intentos
    for (int i = 0; i < 5; ++i) {
        test_state.m_pLearn_theorical = 0.9; 
        test_state.m_pLearn_operative = 0.2; 
        Engine.updateKnowledge(test_state, true, 1000.0, lambda);
    }
    
    // Esperamos que el contador de dominio haya superado el umbral (o al menos que no sea 0)
    REQUIRE(test_state.m_sustained_theorical_dominance > 0);
  }
}
