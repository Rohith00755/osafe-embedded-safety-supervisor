#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace osafe {
constexpr std::size_t kMaxMessageLength = 256;
enum class MessageType { Heartbeat, Status, Alert, Invalid };
struct Message { MessageType type{MessageType::Invalid}; std::string key; std::string value; };

inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::size_t start = 0;
    for (;;) {
        auto end = s.find(delim, start);
        out.push_back(s.substr(start, end == std::string::npos ? std::string::npos : end - start));
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return out;
}
inline bool safeToken(const std::string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.'))
            return false;
    return true;
}
inline Message parseMessage(const std::string& line) {
    if (line.empty() || line.size() > kMaxMessageLength) return {};
    if (line == "HEARTBEAT") return {MessageType::Heartbeat, {}, {}};
    const auto p = split(line, '|');
    if (p.size() == 3 && (p[0] == "STATUS" || p[0] == "ALERT") &&
        safeToken(p[1]) && safeToken(p[2]))
        return {p[0] == "STATUS" ? MessageType::Status : MessageType::Alert, p[1], p[2]};
    return {};
}
}
