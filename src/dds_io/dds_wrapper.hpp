#pragma once

#include "../core/telemetry.hpp"
#include <functional>
#include <iostream>

namespace dds_io {

// Forward-declared concrete implementations live in dds_wrapper.cpp.
// Publisher: publish TelemetryMessage to DDS topic using CycloneDDS.
class Publisher {
public:
    Publisher();
    ~Publisher();
    void publish(const TelemetryMessage &m);
private:
    struct Impl;
    Impl *impl{nullptr};
};

// Subscriber: registers a callback to handle incoming TelemetryMessage via DDS.
class Subscriber {
public:
    Subscriber();
    ~Subscriber();
    // callback invoked from a reader thread: void(const TelemetryMessage&)
    void set_callback(std::function<void(const TelemetryMessage&)> cb);
private:
    struct Impl;
    Impl *impl{nullptr};
};

} // namespace dds_io
