#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace hestia::graph {

struct SkillNode {
    int id;
    std::string name;
    std::string domain;
    std::vector<int> prerequisites;
};

class SkillGraph {
public:
    /// Carga el grafo desde un archivo JSON con la estructura { "skills": [...] }
    bool load(const std::string& path);

    /// Retorna los prerequisitos de una skill, o vector vacío si no existe
    [[nodiscard]] std::vector<int> getPrerequisites(int skill_id) const;

    /// Retorna las skills desbloqueadas dado un conjunto de skills dominadas.
    /// Una skill se desbloquea si TODAS sus prereqs están en mastered_ids
    /// y la skill misma NO está en mastered_ids.
    [[nodiscard]] std::vector<int> getUnlockedSkills(const std::vector<int>& mastered_ids) const;

    /// Validación de integridad: retorna true si la skill existe en el grafo
    [[nodiscard]] bool exists(int skill_id) const;

    /// Cantidad total de skills en el grafo
    [[nodiscard]] size_t size() const noexcept;

private:
    std::unordered_map<int, SkillNode> m_skills;
};

} // namespace hestia::graph
