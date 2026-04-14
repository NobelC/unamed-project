-- HESTIA Database Schema v1.0
-- Decisión: INTEGER PRIMARY KEY para auditoría sin penalización de AUTOINCREMENT.
-- Decisión: Sin índice en timestamp para priorizar O(1) en INSERT de la vía crítica.

PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA user_version = 1;

CREATE TABLE IF NOT EXISTS skill_state (
    student_id INTEGER NOT NULL,
    skill_id INTEGER NOT NULL,
    p_learn_operative REAL NOT NULL,
    p_learn_theorical REAL NOT NULL,
    p_transition REAL NOT NULL,
    p_slip REAL NOT NULL,
    p_guess REAL NOT NULL,
    p_forget REAL NOT NULL,
    avg_response_time REAL DEFAULT 0.0,
    last_practice_time INTEGER DEFAULT 0,
    PRIMARY KEY (student_id, skill_id)
) WITHOUT ROWID;

CREATE TABLE IF NOT EXISTS method_state (
    student_id INTEGER NOT NULL,
    skill_id INTEGER NOT NULL,
    method_id INTEGER NOT NULL,
    attempts INTEGER DEFAULT 0,
    successes INTEGER DEFAULT 0,
    PRIMARY KEY (student_id, skill_id, method_id)
) WITHOUT ROWID;

CREATE TABLE IF NOT EXISTS response_log (
    log_id INTEGER PRIMARY KEY,
    student_id INTEGER NOT NULL,
    skill_id INTEGER NOT NULL,
    method_id INTEGER NOT NULL,
    timestamp INTEGER NOT NULL,
    is_correct INTEGER NOT NULL,
    response_ms INTEGER NOT NULL
);
