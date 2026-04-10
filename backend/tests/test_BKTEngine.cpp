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
}
