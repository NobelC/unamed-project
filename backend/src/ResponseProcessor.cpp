#include "../include/ResponseProcessor.hpp"

namespace hestia::core {

ResponseProcessor::ResponseProcessor(
    bkt::BKTEngine& bkt,
    mab::MABEngine& mab,
    bkt::SessionManager& session,
    persistence::PersistenceLayer& storage,
    zone::ZoneBlender& blender,
    double lambda)
    : m_bkt(bkt)
    , m_mab(mab)
    , m_session(session)
    , m_storage(storage)
    , m_blender(blender)
    , m_lambda(lambda)
{}

ResponseResult ResponseProcessor::processResponse(
    int student_id, int skill_id,
    bool correct, double response_ms)
{
    // 1. Cargar estado desde persistencia (o usar default si no existe)
    auto loaded = m_storage.loadSkillState(student_id, skill_id);
    bkt::SkillState state = loaded.value_or(bkt::SkillState{});

    // 2. Verificar si el tiempo de respuesta es anómalo
    bool anomalous = m_session.isResponseTimeAnomalous(response_ms);

    // 3. Solo actualizar modelos si el tiempo NO es anómalo
    if (!anomalous) {
        m_bkt.updateKnowledge(state, correct, response_ms, m_lambda);
        mab::METHOD current_method = m_mab.selectMethod();
        m_mab.updateMethod(current_method, correct);
    }

    // 4. Seleccionar siguiente ejercicio (zona + método)
    auto selection = m_blender.selectExercise(skill_id, state, m_mab);

    // 5. Persistir el estado actualizado
    mab::METHOD used_method = selection.method;
    // Obtenemos el method state actual para persistencia
    auto method_states = m_storage.loadMethodStates(student_id, skill_id);
    auto method_idx = static_cast<size_t>(used_method);
    [[maybe_unused]] auto save_result = m_storage.saveInteraction(
        student_id, skill_id, state, used_method, correct,
        static_cast<int>(response_ms),
        method_states[method_idx].count_attempts,
        method_states[method_idx].successes);

    // 6. Retornar resultado
    return {
        selection.method,
        selection.zone,
        state.m_pLearn_operative,
        anomalous
    };
}

} // namespace hestia::core
