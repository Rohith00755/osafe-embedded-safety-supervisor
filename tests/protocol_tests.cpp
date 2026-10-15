#include "protocol.hpp"

#include <cassert>
#include <iostream>

int main() {
    using osafe::MessageType;
    using osafe::parseMessage;

    assert(parseMessage("HEARTBEAT").type == MessageType::Heartbeat);

    const auto status = parseMessage("STATUS|link|up");
    assert(status.type == MessageType::Status);
    assert(status.key == "link");
    assert(status.value == "up");

    const auto alert = parseMessage("ALERT|SENSOR_TIMEOUT|imu");
    assert(alert.type == MessageType::Alert);
    assert(alert.key == "SENSOR_TIMEOUT");
    assert(alert.value == "imu");

    assert(parseMessage("").type == MessageType::Invalid);
    assert(parseMessage("HELLO").type == MessageType::Invalid);
    assert(parseMessage("STATUS|bad key|up").type == MessageType::Invalid);
    assert(parseMessage("STATUS|link").type == MessageType::Invalid);
    assert(parseMessage("HEARTBEAT|extra").type == MessageType::Invalid);
    assert(parseMessage(std::string(osafe::kMaxMessageLength + 1, 'A')).type ==
           MessageType::Invalid);

    std::cout << "All O-SAFE protocol parser tests passed.\n";
    return 0;
}
