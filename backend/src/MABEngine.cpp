#include "MABEngine.hpp"
#include <cmath>
#include <limits>
#include <cassert>

namespace hestia::mab {

MABEngine::MABEngine(double exploration_c) noexcept 
    : m_exploration_constant(exploration_c) {
    m_total_attempts = 0;
    m_session_total_attempts = 0;
}

void MABEngine::loadFrom(const std::array<MethodState, METHOD_COUNT>& persisted) noexcept {
    m_method_data = persisted;
    // Recalcular el total de intentos a partir del estado cargado para que UCB sea correcto
    m_total_attempts = 0;
    for (const auto& s : m_method_data) {
        m_total_attempts += s.count_attempts;
    }
}

void MABEngine::resetSession() noexcept {
    m_session_data.fill(MethodState{0, 0});
    m_session_total_attempts = 0;
}

void MABEngine::updateMethod(METHOD used_method, bool success) noexcept {
    const auto idx = static_cast<std::size_t>(used_method);
    assert(idx < m_method_data.size() && "MABEngine: Index out of bounds");

    m_method_data[idx].count_attempts++;
    m_session_data[idx].count_attempts++;
    if (success) {
        m_method_data[idx].successes++;
        m_session_data[idx].successes++;
    }
    m_total_attempts++;
    m_session_total_attempts++;
}

const MethodState& MABEngine::getMethodState(METHOD m) const noexcept {
    const auto idx = static_cast<std::size_t>(m);
    assert(idx < m_method_data.size());
    return m_method_data[idx];
}

[[nodiscard]] METHOD MABEngine::selectMethod() const noexcept {
    if (m_total_attempts == 0) {
        return static_cast<METHOD>(0);
    }

    for (std::size_t i = 0; i < m_method_data.size(); ++i) {
        if (m_method_data[i].count_attempts == 0) {
            return static_cast<METHOD>(i);
        }
    }

    double max_upper_bound = -std::numeric_limits<double>::infinity();
    std::size_t best_idx = 0;

    for (std::size_t i = 0; i < m_method_data.size(); ++i) {
        double score = calculateUCB(m_method_data[i], m_total_attempts, m_exploration_constant);
        
        if (score > max_upper_bound) {
            max_upper_bound = score;
            best_idx = i;
        }
    }
    return static_cast<METHOD>(best_idx);
}

double MABEngine::calculateUCB(const MethodState& state, uint32_t total_n, double c_param) noexcept {
    const double mu = static_cast<double>(state.successes) / state.count_attempts;
    
    const double exploration = c_param * std::sqrt(std::log(static_cast<double>(total_n)) / state.count_attempts);
    
    return mu + exploration;
}

}
