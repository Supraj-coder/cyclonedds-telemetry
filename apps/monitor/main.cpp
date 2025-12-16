#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include "../../src/core/telemetry.hpp"
#include "../../src/dds_io/dds_wrapper.hpp"

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    std::atomic<bool> running{true};

    // rolling stats per sensor
    struct Stats { double min{1e308}; double max{-1e308}; double sum{0.0}; uint64_t cnt{0}; int64_t last_ts{0}; };
    std::mutex mtx;
    std::unordered_map<std::string, Stats> stats;

    dds_io::Subscriber sub;
    sub.set_callback([&](const TelemetryMessage &m){
        std::lock_guard<std::mutex> lk(mtx);
        auto &s = stats[m.sensor_id];
        s.min = std::min(s.min, m.value);
        s.max = std::max(s.max, m.value);
        s.sum += m.value;
        s.cnt += 1;
        s.last_ts = m.timestamp_ms;
        // simple immediate warning check
        if (m.value < 0.0 || m.value > 1000.0) {
            std::cerr << "WARNING: " << m.sensor_id << " out of range: " << m.value << "\n";
        }
    });

    // status printer
    std::thread printer([&]{
        while (running.load()){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::lock_guard<std::mutex> lk(mtx);
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::cout << "--- monitor status ---\n";
            for (auto &p : stats){
                const auto &id = p.first;
                const auto &s = p.second;
                double avg = s.cnt ? (s.sum / double(s.cnt)) : 0.0;
                std::cout << id << ": cnt=" << s.cnt << " min=" << s.min << " max=" << s.max << " avg=" << avg;
                if (now_ms - s.last_ts > 2000) std::cout << "  [STALE]";
                std::cout << "\n";
            }
        }
    });

    // run for 10s
    std::this_thread::sleep_for(std::chrono::seconds(10));
    running.store(false);
    if (printer.joinable()) printer.join();
    std::cout << "monitor: exiting." << std::endl;
    return 0;
}
