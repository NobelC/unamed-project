#include "../include/ZoneBlender.hpp"

namespace hestia::zone {

ZoneBlender::ZoneBlender(uint64_t seed) {
    if (seed == 0) {
        std::random_device rd;
        m_rng.seed(rd());
    } else {
        m_rng.seed(seed);
    }
}

double ZoneBlender::getLowZoneProbability(double pL) noexcept {
    // Tabla de selección de zona según P(L) operativo:
    // P(L) < 0.40       → 80% zona baja
    // 0.40 ≤ P(L) < 0.60 → 60% zona baja
    // 0.60 ≤ P(L) < 0.80 → 30% zona baja
    // 0.80 ≤ P(L) < 0.90 → 10% zona baja
    // P(L) ≥ 0.90        → 10% zona baja (fáciles para refuerzo)
    if (pL < 0.40) return 0.80;
    if (pL < 0.60) return 0.60;
    if (pL < 0.80) return 0.30;
    return 0.10; // P(L) >= 0.80
}

Zone ZoneBlender::selectZone(const bkt::SkillState& state) {
    double prob_low = getLowZoneProbability(state.m_pLearn_operative);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double roll = dist(m_rng);
    return (roll < prob_low) ? Zone::LOW : Zone::CURRENT;
}

ExerciseSelection ZoneBlender::selectExercise(
    int skill_id,
    const bkt::SkillState& state,
    mab::MABEngine& mab_engine)
{
    Zone zone = selectZone(state);
    mab::METHOD method = mab_engine.selectMethod();
    return {zone, method, skill_id};
}

} // namespace hestia::zone
