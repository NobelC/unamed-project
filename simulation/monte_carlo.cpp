#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "../backend/include/BKTEngine.hpp"
#include "../backend/include/MABEngine.hpp"
#include "../backend/include/SessionManager.hpp"

using namespace hestia::bkt;
using namespace hestia::mab;

struct LearnerArchetype {
    std::string name;
    double p_transition;
    double p_slip;
    double p_guess;
    double p_forget;
    std::array<double, 5> method_success_probs; // VISUAL, AUDITORY, KINESTHETIC, PHONETIC, GLOBAL
};

const std::vector<LearnerArchetype> ARCHETYPES = {
    {"Fast Learner", 0.30, 0.05, 0.10, 0.02, {0.9, 0.8, 0.7, 0.6, 0.8}},
    {"Average Learner", 0.15, 0.10, 0.10, 0.05, {0.6, 0.7, 0.6, 0.5, 0.6}},
    {"Slow and Consistent", 0.05, 0.05, 0.10, 0.01, {0.4, 0.4, 0.8, 0.4, 0.4}}, // Visual learner
    {"Forgetful Learner", 0.20, 0.10, 0.10, 0.40, {0.5, 0.5, 0.5, 0.5, 0.5}},
    {"Struggling Learner", 0.02, 0.25, 0.10, 0.15, {0.3, 0.3, 0.3, 0.4, 0.3}}
};

void run_simulation(const LearnerArchetype& arch, int num_runs = 100, int items_per_session = 10, int max_sessions = 50) {
    std::cout << "--- Simulating: " << arch.name << " (" << num_runs << " runs) ---" << std::endl;
    
    std::mt19937 generator(42);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    const double MASTERY_THRESHOLD = 0.90;
    std::vector<int> convergence_times;
    int oscillating_runs = 0;

    for (int run = 0; run < num_runs; ++run) {
        BKTEngine engine;
        MABEngine mab(1.5);
        SkillState state;
        state.m_pTransition = arch.p_transition;
        state.m_pSlip = arch.p_slip;
        state.m_pGuess = arch.p_guess;
        state.m_pForget = arch.p_forget;
        state.is_initialized = true;
        
        int convergence_session = -1;

        for (int s = 0; s < max_sessions; ++s) {
            mab.resetSession();
            for (int i = 0; i < items_per_session; ++i) {
                METHOD method = mab.selectMethod();
                double method_prob = arch.method_success_probs[static_cast<int>(method)];
                
                bool known = (distribution(generator) < state.m_pLearn_operative);
                
                // observable response based on method preference
                double p_correct_if_known = method_prob * (1.0 - arch.p_slip);
                double p_correct_if_unknown = (1.0 - method_prob) * arch.p_guess;
                
                bool correct = known ? (distribution(generator) < p_correct_if_known) 
                                     : (distribution(generator) < p_correct_if_unknown);

                SessionManager session;
                session.applyTransitionDecay(state, 0.5);
                engine.updateKnowledge(state, correct, 1000.0);
                mab.updateMethod(method, correct);
            }

            if (state.m_pLearn_operative >= MASTERY_THRESHOLD) {
                convergence_session = s + 1;
                break;
            }
        }

        if (convergence_session != -1) {
            convergence_times.push_back(convergence_session);
        } else {
            oscillating_runs++;
        }
    }

    if (convergence_times.empty()) {
        std::cout << "Result: 0% convergence. All runs oscillated or failed to reach mastery." << std::endl;
        std::cout << std::endl;
        return;
    }

    std::sort(convergence_times.begin(), convergence_times.end());
    
    double sum = std::accumulate(convergence_times.begin(), convergence_times.end(), 0.0);
    double mean = sum / convergence_times.size();
    
    double sq_sum = std::inner_product(convergence_times.begin(), convergence_times.end(), convergence_times.begin(), 0.0);
    double stddev = std::sqrt(sq_sum / convergence_times.size() - mean * mean);
    
    int p10 = convergence_times[convergence_times.size() * 0.10];
    int p50 = convergence_times[convergence_times.size() * 0.50];
    int p90 = convergence_times[convergence_times.size() * 0.90];
    double convergence_rate = (double)convergence_times.size() / num_runs * 100.0;

    std::cout << "Convergence Rate : " << convergence_rate << "%" << std::endl;
    std::cout << "Oscillating Runs : " << oscillating_runs << std::endl;
    std::cout << "Mean Sessions    : " << mean << " ± " << stddev << std::endl;
    std::cout << "Percentiles      : P10=" << p10 << ", P50=" << p50 << ", P90=" << p90 << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "HESTIA Monte Carlo Validation System (Advanced Stats + MAB)\n" << std::endl;
    for (const auto& arch : ARCHETYPES) {
        run_simulation(arch);
    }
    return 0;
}
