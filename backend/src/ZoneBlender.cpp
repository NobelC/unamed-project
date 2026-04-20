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
    // P(L) < 0.60         → 80% zona baja
    // 0.60 ≤ P(L) < 0.90  → 20% zona baja
    // P(L) ≥ 0.90         → 10% zona baja
    if (pL < 0.60) return 0.80;
    if (pL < 0.90) return 0.20;
    return 0.10; // P(L) >= 0.90
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
