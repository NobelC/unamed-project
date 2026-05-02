-- HESTIA Database Schema v4.0 - Migration 004
-- Feature 20: Índices faltantes en el schema SQL

PRAGMA user_version = 4;

CREATE INDEX IF NOT EXISTS idx_log_student_skill ON response_log(student_id, skill_id);
CREATE INDEX IF NOT EXISTS idx_log_timestamp ON response_log(timestamp);
