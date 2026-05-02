-- HESTIA Database Schema v5.0 - Migration 005
-- Feature 17/22: p_learn en response_log para tracking de P(L)

PRAGMA user_version = 5;

-- Agregamos la columna a response_log para que el reporte de sesión y queries analíticas funcionen
ALTER TABLE response_log ADD COLUMN p_learn REAL DEFAULT 0.0;
