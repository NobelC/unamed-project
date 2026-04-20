#pragma once
#include <chrono>
#include <unordered_map>
#include <vector>
#include <array>

namespace hestia::srs {

/// Intervalos estándar de repetición espaciada: 1d → 3d → 7d → 14d → 30d
inline constexpr std::array<int, 5> INTERVALS_DAYS = {1, 3, 7, 14, 30};

struct SRSEntry {
    int skill_id;
    int correct_streak{0};
    std::chrono::system_clock::time_point next_review;
};

class SRSQueue {
public:
    /// Programa (o reprograma) una skill con la racha dada
    void schedule(int skill_id, int correct_streak);

    /// Retorna los IDs de skills cuyo tiempo de revisión ya venció
    [[nodiscard]] std::vector<int> getDueSkills() const;

    /// Actualiza la racha: correcto → incrementa streak + recalcula intervalo;
    /// incorrecto → resetea streak a 0 + intervalo a 1 día
    void markResult(int skill_id, bool correct);

    /// Verifica si una skill tiene entrada en la cola
    [[nodiscard]] bool hasEntry(int skill_id) const;

private:
    std::unordered_map<int, SRSEntry> m_entries;

    /// Mapea la racha al intervalo correspondiente en horas
    [[nodiscard]] static std::chrono::hours getInterval(int streak) noexcept;
};

} // namespace hestia::srs
