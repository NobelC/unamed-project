#include "../include/ResponseProcessor.hpp"

namespace hestia::core {

SessionReport ResponseProcessor::generateSessionReport(int student_id,
                                                       int64_t session_start_ts) const {
    SessionReport report;
    report.total_attempts = 0;
    report.hit_rate = 0.0;
    report.most_used_method = mab::METHOD::VISUAL;
    report.total_session_time_minutes = 0.0;

    // To implement this fully, we need access to the sqlite3 database from PersistenceLayer
    // We will assume a simple implementation for now, and rely on the database directly.
    return report;
}

}  // namespace hestia::core
