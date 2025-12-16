#include <catch2/catch_test_macros.hpp>
#include "../src/core/telemetry.hpp"

TEST_CASE("JSON round-trip serialization") {
    TelemetryMessage m = make_msg("temp", 42.5, "C", 7);
    nlohmann::json j = m;
    TelemetryMessage out = j.get<TelemetryMessage>();
    CHECK(out.sensor_id == m.sensor_id);
    CHECK(out.value == m.value);
    CHECK(out.units == m.units);
    CHECK(out.sequence == m.sequence);
}
#include <catch2/catch_test_macros.hpp>
#include "../src/core/telemetry.hpp"
#include <nlohmann/json.hpp>

TEST_CASE("Telemetry JSON roundtrip") {
    TelemetryMessage m = make_msg("sensorX", 42.5, "C", 123);
    nlohmann::json j = m;
    auto m2 = j.get<TelemetryMessage>();
    REQUIRE(m2.sensor_id == m.sensor_id);
    REQUIRE(m2.value == m.value);
    REQUIRE(m2.sequence == m.sequence);
}
