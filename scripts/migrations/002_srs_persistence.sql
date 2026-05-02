-- HESTIA Database Schema v2.0 - Migration 002
-- Bug fix #6: persistir el estado SRS entre sesiones.
-- La cola SRS era completamente en memoria; al cerrar la app todos los intervalos
-- de revisión programados se perdían. Esta tabla preserva el estado por (estudiante × skill).

PRAGMA user_version = 2;

CREATE TABLE IF NOT EXISTS srs_state (
    student_id    INTEGER NOT NULL,
    skill_id      INTEGER NOT NULL,
    correct_streak INTEGER NOT NULL DEFAULT 0,
    next_review    INTEGER NOT NULL DEFAULT 0,  -- Unix timestamp (segundos)
    PRIMARY KEY (student_id, skill_id)
) WITHOUT ROWID;
