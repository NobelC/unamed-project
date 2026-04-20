#include "../include/SRSQueue.hpp"

namespace hestia::srs {

std::chrono::hours SRSQueue::getInterval(int streak) noexcept {
    // Mapea streak al array INTERVALS_DAYS, clampeando al máximo
    int index = streak;
    if (index < 0) index = 0;
    if (index >= static_cast<int>(INTERVALS_DAYS.size())) {
        index = static_cast<int>(INTERVALS_DAYS.size()) - 1;
    }
    return std::chrono::hours{INTERVALS_DAYS[index] * 24};
}

void SRSQueue::schedule(int skill_id, int correct_streak) {
    auto now = std::chrono::system_clock::now();
    SRSEntry entry;
    entry.skill_id = skill_id;
    entry.correct_streak = correct_streak;
    entry.next_review = now + getInterval(correct_streak);
    m_entries[skill_id] = entry;
}

std::vector<int> SRSQueue::getDueSkills() const {
    auto now = std::chrono::system_clock::now();
    std::vector<int> due;

    for (const auto& [id, entry] : m_entries) {
        if (entry.next_review <= now) {
            due.push_back(id);
        }
    }

    return due;
}

void SRSQueue::markResult(int skill_id, bool correct) {
    auto now = std::chrono::system_clock::now();
    auto it = m_entries.find(skill_id);

    if (it == m_entries.end()) {
        SRSEntry entry;
        entry.skill_id = skill_id;
        if (correct) {
            entry.correct_streak = 1;
            entry.next_review = now + getInterval(1);
        } else {
            entry.correct_streak = 0;
            entry.next_review = now + getInterval(0);
        }
        m_entries[skill_id] = entry;
    } else {
        if (correct) {
            it->second.correct_streak++;
            it->second.next_review = now + getInterval(it->second.correct_streak);
        } else {
            it->second.correct_streak = 0;
            it->second.next_review = now + getInterval(0);
        }
    }
}

bool SRSQueue::hasEntry(int skill_id) const {
    return m_entries.contains(skill_id);
}

} // namespace hestia::srs
