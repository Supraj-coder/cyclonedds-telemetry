#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>

struct TelemetryMessage {
    std::string sensor_id;
    double value{0.0};
    std::string units{""};
    uint64_t sequence{0};
    std::int64_t timestamp_ms{0};
};

inline void to_json(nlohmann::json &j, const TelemetryMessage &m) {
    j = nlohmann::json{{"sensor_id", m.sensor_id}, {"value", m.value}, {"units", m.units}, {"seq", m.sequence}, {"ts_ms", m.timestamp_ms}};
}
inline void from_json(const nlohmann::json &j, TelemetryMessage &m) {
    j.at("sensor_id").get_to(m.sensor_id);
    j.at("value").get_to(m.value);
    j.at("units").get_to(m.units);
    j.at("seq").get_to(m.sequence);
    j.at("ts_ms").get_to(m.timestamp_ms);
}

inline TelemetryMessage make_msg(const std::string &id, double value, const std::string &units, uint64_t seq) {
    TelemetryMessage m;
    m.sensor_id = id;
    m.value = value;
    m.units = units;
    m.sequence = seq;
    m.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return m;
}
