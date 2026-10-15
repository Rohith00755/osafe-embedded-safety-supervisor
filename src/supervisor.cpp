#include "protocol.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {
volatile std::sig_atomic_t stopping = 0;
void onSignal(int) { stopping = 1; }
enum class State { Waiting, Healthy, Degraded, Fault };
const char* name(State s) {
    switch (s) {
        case State::Waiting: return "WAITING";
        case State::Healthy: return "HEALTHY";
        case State::Degraded: return "DEGRADED";
        case State::Fault: return "FAULT";
    }
    return "UNKNOWN";
}
int portFromEnv() {
    const char* raw = std::getenv("OSAFE_PORT");
    if (!raw) return 5000;
    char* end = nullptr; long n = std::strtol(raw, &end, 10);
    return end && *end == '\0' && n >= 1 && n <= 65535 ? static_cast<int>(n) : 5000;
}
void logState(State s, const std::string& reason) {
    std::cout << "[SUPERVISOR] state=" << name(s) << " reason=" << reason << std::endl;
}
void serve(int fd) {
    using Clock = std::chrono::steady_clock;
    auto lastBeat = Clock::now();
    constexpr auto timeout = std::chrono::seconds(5);
    State state = State::Waiting;
    std::string pending;
    char buf[128];
    logState(state, "client_connected");
    while (!stopping) {
        fd_set set; FD_ZERO(&set); FD_SET(fd, &set);
        timeval tv{1, 0};
        int ready = select(fd + 1, &set, nullptr, nullptr, &tv);
        if (ready < 0) { if (errno == EINTR) continue; logState(State::Degraded, "select_error"); break; }
        if (ready > 0 && FD_ISSET(fd, &set)) {
            auto n = recv(fd, buf, sizeof(buf), 0);
            if (n <= 0) { logState(State::Degraded, "client_disconnected"); break; }
            pending.append(buf, static_cast<std::size_t>(n));
            if (pending.size() > osafe::kMaxMessageLength * 4) {
                logState(State::Degraded, "input_buffer_limit"); break;
            }
            std::size_t pos;
            while ((pos = pending.find('\n')) != std::string::npos) {
                std::string line = pending.substr(0, pos);
                pending.erase(0, pos + 1);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                auto m = osafe::parseMessage(line);
                switch (m.type) {
                    case osafe::MessageType::Heartbeat:
                        lastBeat = Clock::now();
                        if (state != State::Fault) { state = State::Healthy; logState(state, "heartbeat_received"); }
                        break;
                    case osafe::MessageType::Status:
                        std::cout << "[STATUS] " << m.key << "=" << m.value << std::endl; break;
                    case osafe::MessageType::Alert:
                        state = State::Fault; logState(state, m.key + ":" + m.value); break;
                    case osafe::MessageType::Invalid:
                        std::cout << "[PROTOCOL] rejected_message" << std::endl; break;
                }
            }
        }
        if (state != State::Fault && state != State::Degraded && Clock::now() - lastBeat > timeout) {
            state = State::Degraded; logState(state, "heartbeat_timeout");
        }
    }
}
}
int main() {
    std::signal(SIGINT, onSignal); std::signal(SIGTERM, onSignal);
    int port = portFromEnv();
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { std::cerr << "socket: " << std::strerror(errno) << std::endl; return 1; }
    int reuse = 1; setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<unsigned short>(port));
    if (bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0 ||
        listen(server, 1) < 0) {
        std::cerr << "bind/listen: " << std::strerror(errno) << std::endl; close(server); return 1;
    }
    std::cout << "O-SAFE prototype listening on TCP port " << port
              << " (diagnostic only; not safety-certified)" << std::endl;
    while (!stopping) {
        sockaddr_in peer{}; socklen_t len = sizeof(peer);
        int client = accept(server, reinterpret_cast<sockaddr*>(&peer), &len);
        if (client < 0) { if (errno == EINTR) continue; std::cerr << "accept: " << std::strerror(errno) << std::endl; continue; }
        serve(client); close(client);
    }
    close(server); return 0;
}
