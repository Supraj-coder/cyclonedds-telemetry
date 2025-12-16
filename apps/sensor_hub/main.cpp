#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include "../../src/core/telemetry.hpp"
#include "../../src/core/Channel.h"
#include "../../src/dds_io/dds_wrapper.hpp"

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    using namespace std::chrono_literals;

    Channel<TelemetryMessage> channel;
    std::atomic<bool> run{true};
    std::atomic<uint64_t> seq{0};

    // spawn a few sensor threads
    std::vector<std::thread> sensors;
    std::vector<std::string> ids = {"temp", "pressure", "flow"};
    for (auto &id : ids) {
        sensors.emplace_back([&, id]() {
            std::mt19937_64 rng(std::hash<std::string>{}(id));
            std::uniform_real_distribution<double> dist(0.0, 100.0);
            while(run.load()) {
                auto m = make_msg(id, dist(rng), "units", seq.fetch_add(1));
                channel.push(m);
                std::this_thread::sleep_for(100ms);
            }
        });
    }

    // batching thread: read from channel and publish via DDS (if available)
    std::thread publisher([&]() {
        dds_io::Publisher pub;
        while (true) {
            auto item = channel.pop();
            if (!item) break; // channel closed
            // publish to DDS or print
            pub.publish(*item);
            std::cout << "PUBLISH: " << item->sensor_id << " " << item->value << " seq=" << item->sequence << "\n";
        }
    });

    // run for 3 seconds then shutdown
    std::this_thread::sleep_for(3s);
    run.store(false);
    channel.close();
    for (auto &t : sensors) if (t.joinable()) t.join();
    if (publisher.joinable()) publisher.join();
    std::cout << "sensor_hub exiting\n";
    return 0;
}
