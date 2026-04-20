#include "../include/SessionManager.hpp"
#include <chrono>

namespace hestia::bkt {

void SessionManager::startSession(SkillState& state) const noexcept {
    state.session_start_time = std::chrono::system_clock::now();
    state.m_pTransition = DEFAULT_P_TRANSITION;
    state.validationProbabilityRanges();
}

void SessionManager::endSession(SkillState& state) const noexcept {
    state.session_count++;
    // Cleanup: we can reset the session start time to epoch 0 to indicate no active session
    state.session_start_time = std::chrono::system_clock::time_point{};
}

bool SessionManager::isResponseTimeAnomalous(double response_time_ms) const noexcept {
    // 5 minutes in milliseconds
    return response_time_ms > 300000.0;
}

double SessionManager::getSessionElapsedMinutes(const SkillState& state) const noexcept {
    if (state.session_start_time.time_since_epoch().count() == 0) {
        return 0.0;
    }
    
    std::chrono::duration<double> duration = std::chrono::system_clock::now() - state.session_start_time;
    using minute_double_cast = std::chrono::duration<double, std::ratio<60>>;
    return std::chrono::duration_cast<minute_double_cast>(duration).count();
}

} // namespace hestia::bkt
