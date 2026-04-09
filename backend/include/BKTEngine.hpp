#pragma once
#include <chrono>
#include <cstdint>
#include <algorithm>

namespace hestia::bkt {

  constexpr double MAX_P_LEARN_OPERATIVE = 0.90;
  constexpr double MIN_P_LEARN_OPERATIVE = 0.01;
  constexpr std::chrono::hours FORGET_THRESHOLD_HOURS{48};
  constexpr double P_TRANSITION_FLOOR = 0.05;

  constexpr double DEFAULT_P_LEARN = 0.20;
  constexpr double DEFAULT_P_TRANSITION = 0.10;
  constexpr double DEFAULT_P_GUESS = 0.05;
  constexpr double DEFAULT_P_SLIP = 0.05;
  constexpr double DEFAULT_P_FORGET = 0.50;

  struct SkillState{
    //Pesos del modulo BKT+ 
    double m_pLearnOperative; //Factor P(L) Operativo capado a <= 0.90
    double m_pLearnTheorical; //Factor P(L) para evitar el anti-stall
    double m_pGuess; //Factor P(G) - suerte
    double m_pSlip; //Factor P(S) - descuido
    double m_pForget; //Factor P(F) - olvido
    double m_pTransition; //Factor P(T) - Transicion
    double avg_response_time_ms{0.0};
    
    //Trackeo temporal
    std::chrono::system_clock::time_point last_practice_time{};
    std::chrono::steady_clock session_start_time{};

    uint32_t skill_id;
    
    uint32_t total_attempts{0};
    uint32_t session_count{0};
    uint32_t consecutive_correct{0};
    uint32_t consecutive_error{0};
    uint32_t sustained_theorical_dominance{0};

    //Estado
    bool is_initialized : 1;
    
    //Consultas inmutables 
    [[nodiscard]] constexpr bool isColdStart() const noexcept{return !is_initialized;}
    [[nodiscard]] bool exceedsForgetThreshold() const noexcept{
      if(last_practice_time.time_since_epoch().count() == 0) {return false;}
      return std::chrono::system_clock::now() - last_practice_time >= FORGET_THRESHOLD_HOURS;
    }

    void validationRangesData() noexcept{
      m_pLearnOperative = std::clamp(m_pLearnOperative, MIN_P_LEARN_OPERATIVE, MAX_P_LEARN_OPERATIVE);
      m_pLearnTheorical = std::clamp(m_pLearnTheorical, MIN_P_LEARN_OPERATIVE, 1.0 );
      m_pForget = std::clamp(m_pForget, 0.01, 0.99);
      m_pGuess = std::clamp(m_pGuess, 0.01 , 0.99);
      m_pSlip = std::clamp(m_pSlip, 0.01, 0.99);
      m_pTransition = std::clamp(m_pTransition, P_TRANSITION_FLOOR, 0.99);
    }

    SkillState() :
      m_pLearnOperative(DEFAULT_P_LEARN),
      m_pLearnTheorical(DEFAULT_P_LEARN),
      m_pTransition(DEFAULT_P_TRANSITION),
      m_pSlip(DEFAULT_P_SLIP),
      m_pGuess(DEFAULT_P_GUESS),
      m_pForget(DEFAULT_P_FORGET),
      is_initialized(false) {}
  };

  class BKTEngine{
    public:
      explicit BKTEngine() noexcept = default;
      //Actular estado tras cada respuesta del estudiante
      void UpdateKnowledge(SkillState& state, bool is_correct, double response_time_ms) noexcept;
      [[nodiscard]] double currenPTtransition(const SkillState& state) const noexcept;
      void applyForgetFactor(SkillState& state) noexcept;
    private:
      [[nodiscard]] double computeRespondePenalty(double& response_time_ms, bool& is_correct) const noexcept;
  };
}
