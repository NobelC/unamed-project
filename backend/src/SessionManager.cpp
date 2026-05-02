#include "../include/SessionManager.hpp"
#include <chrono>

namespace hestia::bkt {

void SessionManager::startSession(SkillState& state) const noexcept {
    state.session_start_time = std::chrono::system_clock::now();
    // Bug fix #3: inicializar también el punto de referencia monótono usado en updateTransitionDecay
    state.session_start_time_steady = std::chrono::steady_clock::now();
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

} // namespace hestia::bkt
