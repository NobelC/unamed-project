-- HESTIA Database Schema v3.0 - Migration 003
-- Feature 19: Tabla students

PRAGMA user_version = 3;

CREATE TABLE IF NOT EXISTS students (
    student_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    created_at INTEGER NOT NULL
);
