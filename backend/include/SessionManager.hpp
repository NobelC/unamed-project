#pragma once
#include "BKTEngine.hpp"

namespace hestia::bkt {

class SessionManager {
public:
    SessionManager() = default;
    ~SessionManager() = default;

    // Registra session_start_time en el estado y resetea P(T) al valor nominal (DEFAULT_P_TRANSITION)
    void startSession(SkillState& state) const noexcept;

    // Incrementa session_count y realiza cleanup
    void endSession(SkillState& state) const noexcept;

    // Retorna true si el tiempo supera 5 minutos (300,000 ms)
    [[nodiscard]] bool isResponseTimeAnomalous(double response_time_ms) const noexcept;

    // Helper que usa session_start_time para calcular minutos transcurridos
    [[nodiscard]] double getSessionElapsedMinutes(const SkillState& state) const noexcept;

    // Aplica el decaimiento de P(T) de forma incremental
    void applyTransitionDecay(SkillState& state, double lambda) const noexcept;
};

} // namespace hestia::bkt
