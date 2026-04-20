#include <iostream>
#include <vector>
#include <random>
#include "../backend/include/BKTEngine.hpp"

using namespace hestia::bkt;

/**
 * Monte Carlo Simulation for HESTIA
 * Validates learning curves and convergence criteria for different archetypes.
 */

struct LearnerArchetype {
    std::string name;
    double p_transition;
    double p_slip;
    double p_guess;
    double p_forget;
};

const std::vector<LearnerArchetype> ARCHETYPES = {
    {"Fast Learner", 0.30, 0.05, 0.10, 0.02},
    {"Average Learner", 0.15, 0.10, 0.10, 0.05},
    {"Slow and Consistent", 0.05, 0.05, 0.10, 0.01}, // Archetype for success criteria
    {"Forgetful Learner", 0.20, 0.10, 0.10, 0.40},
    {"Struggling Learner", 0.02, 0.25, 0.10, 0.15}
};

void run_simulation(const LearnerArchetype& arch, int items_per_session = 10, int max_sessions = 12) {
    std::cout << "--- Simulating: " << arch.name << " ---" << std::endl;
    
    BKTEngine engine;
    SkillState state;
    state.m_pTransition = arch.p_transition;
    state.m_pSlip = arch.p_slip;
    state.m_pGuess = arch.p_guess;
    state.m_pForget = arch.p_forget;
    state.is_initialized = true;

    std::default_random_engine generator(42);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    int convergence_session = -1;
    const double MASTERY_THRESHOLD = 0.90;

    for (int s = 0; s < max_sessions; ++s) {
        for (int i = 0; i < items_per_session; ++i) {
            // hidden knowledge state
            bool known = (distribution(generator) < state.m_pLearn_operative);
            
            // observable response
            bool correct = known ? (distribution(generator) > arch.p_slip) 
                                 : (distribution(generator) < arch.p_guess);

            engine.updateKnowledge(state, correct, 1000.0, 0.5);
        }

        std::cout << "Session " << s + 1 << ": P(L) = " << state.m_pLearn_operative << std::endl;

        if (convergence_session == -1 && state.m_pLearn_operative >= MASTERY_THRESHOLD) {
            convergence_session = s + 1;
        }
    }

    std::cout << "Final P(L): " << state.m_pLearn_operative << std::endl;
    if (convergence_session != -1) {
        std::cout << "Result: CONVERGENCE at session " << convergence_session << std::endl;
        if (arch.name == "Slow and Consistent") {
            if (convergence_session <= 8) {
                std::cout << "STATUS: SUCCESS (Met doc criteria: <= 8 sessions)" << std::endl;
            } else {
                std::cout << "STATUS: FAILURE (Exceeded doc criteria: > 8 sessions)" << std::endl;
            }
        }
    } else {
        std::cout << "Result: NO CONVERGENCE reached in " << max_sessions << " sessions." << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "HESTIA Monte Carlo Validation System\n" << std::endl;
    for (const auto& arch : ARCHETYPES) {
        run_simulation(arch);
    }
    return 0;
}
