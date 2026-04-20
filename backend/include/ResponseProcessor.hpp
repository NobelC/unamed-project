#pragma once
#include "BKTEngine.hpp"
#include "MABEngine.hpp"
#include "SessionManager.hpp"
#include "PersistenceLayer.hpp"
#include "ZoneBlender.hpp"

namespace hestia::core {

struct ResponseResult {
    mab::METHOD next_method;
    zone::Zone next_zone;
    double current_pL;
    bool was_anomalous;   // true si el tiempo fue filtrado por SessionManager
};

class ResponseProcessor {
public:
    ResponseProcessor(
        bkt::BKTEngine& bkt,
        mab::MABEngine& mab,
        bkt::SessionManager& session,
        persistence::PersistenceLayer& storage,
        zone::ZoneBlender& blender,
        double lambda = 0.5);

    /// Procesa una respuesta del usuario. Ciclo completo:
    /// validar tiempo → actualizar BKT → actualizar MAB → persistir → retornar siguiente
    [[nodiscard]] ResponseResult processResponse(
        int student_id, int skill_id,
        bool correct, double response_ms);

private:
    bkt::BKTEngine& m_bkt;
    mab::MABEngine& m_mab;
    bkt::SessionManager& m_session;
    persistence::PersistenceLayer& m_storage;
    zone::ZoneBlender& m_blender;
    double m_lambda;
};

} // namespace hestia::core
