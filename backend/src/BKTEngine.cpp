#include "../include/BKTEngine.hpp"
#include <chrono>

namespace hestia::bkt {
    void UpdateKnowledge(SkillState& state, bool is_correct, double response_time_ms) noexcept{
      state.total_attempts++;
      if(!state.isColdStart() ){
        state.is_initialized = true;
        auto& last_state = state;
        SkillState new_know_data = SkillState();

        if(is_correct){
          new_know_data.m_pLearnOperative = (last_state.m_pLearnOperative * (1 - last_state.m_pSlip)) / 
            ((last_state.m_pLearnOperative * (1 - last_state.m_pSlip)) + ((1 - state.m_pLearnOperative) * (1 - state.m_pGuess)));  
        }
        else{
          new_know_data.m_pLearnOperative = (last_state.m_pLearnOperative * last_state.m_pGuess) / 
            ((last_state.m_pLearnOperative * last_state.m_pSlip) + ((1-last_state.m_pLearnOperative) * (1-last_state.m_pGuess)));
        }

        //Proceso de transicion
        state.m_pTransition = new_know_data.m_pLearnOperative + (1.0 - new_know_data.m_pLearnOperative) * last_state.m_pTransition;
        state.last_practice_time = std::chrono::system_clock::now();

        state.validationRangesData();
      }
    }

    void applyForgetFactor(SkillState& state) noexcept{
      if(state.exceedsForgetThreshold()){
      }
    } 
}

