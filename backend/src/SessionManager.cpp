#include "../include/SessionManager.hpp"
#include <chrono>
#include <cmath>

namespace hestia::bkt {

void SessionManager::startSession(SkillState& state) const noexcept {
    state.session_start_time = std::chrono::system_clock::now();
    // Bug fix #3: inicializar también el punto de referencia monótono usado en updateTransitionDecay
    state.session_start_time_steady = std::chrono::steady_clock::now();
    state.last_update_time_steady = state.session_start_time_steady;
    state.m_pTransition = DEFAULT_P_TRANSITION;
    state.validationProbabilityRanges();
}

void SessionManager::endSession(SkillState& state) const noexcept {
    state.session_count++;
    // Cleanup: reset both time_points to epoch/zero to signal no active session
    state.session_start_time = std::chrono::system_clock::time_point{};
    state.session_start_time_steady = std::chrono::steady_clock::time_point{};
}

bool SessionManager::isResponseTimeAnomalous(double response_time_ms) const noexcept {
    // 5 minutes in milliseconds
    return response_time_ms > 300000.0;
}

double SessionManager::getSessionElapsedMinutes(const SkillState& state) const noexcept {
    // Bug fix #3: usar steady_clock (monótono) en lugar de system_clock.
    // Verificamos con el campo steady: si es el epoch (zero) no hay sesión activa.
    if (state.session_start_time_steady.time_since_epoch().count() == 0) {
        return 0.0;
    }
    
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - state.session_start_time_steady;
    using minute_double_cast = std::chrono::duration<double, std::ratio<60>>;
    return std::chrono::duration_cast<minute_double_cast>(duration).count();
}

void SessionManager::applyTransitionDecay(SkillState& state, double lambda) const noexcept {
    if (state.last_update_time_steady.time_since_epoch().count() == 0) {
        state.last_update_time_steady = std::chrono::steady_clock::now();
        return;
    }
    
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - state.last_update_time_steady;
    using minute_double_cast = std::chrono::duration<double, std::ratio<60>>;
    double minutes_elapsed = std::chrono::duration_cast<minute_double_cast>(duration).count();
    
    state.m_pTransition = state.m_pTransition * std::exp((-lambda) * minutes_elapsed);
    state.last_update_time_steady = std::chrono::steady_clock::now();
    state.validationProbabilityRanges();
}

} // namespace hestia::bkt
