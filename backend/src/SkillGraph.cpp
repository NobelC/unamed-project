#include "../include/SkillGraph.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_set>

namespace hestia::graph {

bool SkillGraph::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json doc;
    try {
        doc = nlohmann::json::parse(file);
    } catch (const nlohmann::json::parse_error&) {
        return false;
    }

    if (!doc.contains("skills") || !doc["skills"].is_array()) return false;

    std::unordered_map<int, SkillNode> parsed;

    for (const auto& entry : doc["skills"]) {
        if (!entry.contains("id") || !entry["id"].is_number_integer()) return false;

        SkillNode node;
        node.id   = entry["id"].get<int>();
        node.name = entry.value("name", "");
        node.domain = entry.value("domain", "");

        if (entry.contains("prerequisites") && entry["prerequisites"].is_array()) {
            for (const auto& prereq : entry["prerequisites"]) {
                if (!prereq.is_number_integer()) return false;
                node.prerequisites.push_back(prereq.get<int>());
            }
        }

        parsed[node.id] = std::move(node);
    }

    // Validación de integridad: todos los prerequisitos deben referenciar skills existentes
    for (const auto& [id, node] : parsed) {
        for (int prereq_id : node.prerequisites) {
            if (!parsed.contains(prereq_id)) return false;
        }
    }

    m_skills = std::move(parsed);
    return true;
}

std::vector<int> SkillGraph::getPrerequisites(int skill_id) const {
    auto it = m_skills.find(skill_id);
    if (it == m_skills.end()) return {};
    return it->second.prerequisites;
}

std::vector<int> SkillGraph::getUnlockedSkills(const std::vector<int>& mastered_ids) const {
    std::unordered_set<int> mastered_set(mastered_ids.begin(), mastered_ids.end());
    std::vector<int> unlocked;

    for (const auto& [id, node] : m_skills) {
        // Ya dominada → no se "desbloquea"
        if (mastered_set.contains(id)) continue;

        // Verificar que TODAS las prereqs estén dominadas
        bool all_met = true;
        for (int prereq_id : node.prerequisites) {
            if (!mastered_set.contains(prereq_id)) {
                all_met = false;
                break;
            }
        }

        if (all_met) {
            unlocked.push_back(id);
        }
    }

    return unlocked;
}

bool SkillGraph::exists(int skill_id) const {
    return m_skills.contains(skill_id);
}

size_t SkillGraph::size() const noexcept {
    return m_skills.size();
}

} // namespace hestia::graph
