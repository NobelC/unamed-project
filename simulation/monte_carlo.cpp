#include <iostream>
#include <vector>
#include <random>
#include "../backend/include/BKTEngine.hpp"
#include "../backend/include/MABEngine.hpp"

using namespace hestia::bkt;
using namespace hestia::mab;

/**
 * Monte Carlo Simulation for HESTIA
 * Simulates different learner archetypes to validate engine stability.
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
    {"Slow Learner", 0.05, 0.15, 0.10, 0.10},
    {"Forgetful Learner", 0.20, 0.10, 0.10, 0.40},
    {"Struggling Learner", 0.02, 0.25, 0.10, 0.15}
};

void run_simulation(const LearnerArchetype& arch, int iterations = 100) {
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

    for (int i = 0; i < iterations; ++i) {
        // True knowledge state (hidden)
        bool known = (distribution(generator) < state.m_pLearn_operative);
        
        // Response generation based on knowledge and slip/guess
        bool correct;
        if (known) {
            correct = (distribution(generator) > arch.p_slip);
        } else {
            correct = (distribution(generator) < arch.p_guess);
        }

        engine.updateKnowledge(state, correct, 1000.0, 0.5);

        if (i % 10 == 0) {
            std::cout << "Iteration " << i << ": P(L) = " << state.m_pLearn_operative << std::endl;
        }
    }
    std::cout << "Final P(L): " << state.m_pLearn_operative << "\n" << std::endl;
}

int main() {
    for (const auto& arch : ARCHETYPES) {
        run_simulation(arch);
    }
    return 0;
}
