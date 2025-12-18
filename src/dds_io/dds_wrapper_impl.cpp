// DDS-specific implementation that depends on generated Telemetry.hpp
#include "dds_wrapper.hpp"
#include <dds/dds.hpp>
#include "Telemetry.hpp"
#include <thread>
#include <atomic>

// Implementation using the generated CycloneDDS C++ types
#include "dds_wrapper.hpp"
#include <dds/dds.hpp>
#include "Telemetry.hpp"
#include <thread>
#include <atomic>
#include <iostream>

namespace dds_io {
namespace impl {

using namespace org::eclipse::cyclonedds;

struct PublisherImpl {
    dds::domain::DomainParticipant participant;
    dds::topic::Topic<Telemetry::Message> topic;
    dds::pub::Publisher publisher;
    dds::pub::DataWriter<Telemetry::Message> writer;

    PublisherImpl()
    : participant(domain::default_id())
    , topic(participant, "/lab/telemetry")
    , publisher(participant)
    , writer(publisher, topic)
    {}
};

PublisherImpl* create_publisher_impl() {
    try {
        return new PublisherImpl();
    } catch (const std::exception& e) {
        std::cerr << "Failed to create publisher: " << e.what() << std::endl;
        return nullptr;
    }
}

void destroy_publisher_impl(PublisherImpl* p) {
    delete p;
}

void publisher_publish(PublisherImpl* p, const TelemetryMessage& m) {
    if (!p) return;
    try {
        Telemetry::Message msg;
        msg.sensor_id(m.sensor_id);
        msg.value(m.value);
        msg.units(m.units);
        msg.seq(m.sequence);
        p->writer.write(msg);
    } catch (const std::exception& e) {
        std::cerr << "Failed to publish: " << e.what() << std::endl;
    }
}

// Subscriber implementation
struct SubscriberImpl {
    dds::domain::DomainParticipant participant;
    dds::topic::Topic<Telemetry::Message> topic;
    dds::sub::Subscriber subscriber;
    dds::sub::DataReader<Telemetry::Message> reader;
    std::function<void(const TelemetryMessage&)> cb;
    std::thread thr;
    std::atomic<bool> running{true};

    SubscriberImpl() 
        : participant(domain::default_id()), 
          topic(participant, "/lab/telemetry"), 
          subscriber(participant), 
          reader(subscriber, topic) {
        thr = std::thread([this](){
            while(running.load()){
                try {
                    dds::sub::LoanedSamples<Telemetry::Message> samples = reader.take();
                    for (auto it = samples.begin(); it < samples.end(); ++it) {
                        if (it->info().valid()) {
                            TelemetryMessage out;
                            out.sensor_id = it->data().sensor_id();
                            out.value = it->data().value();
                            out.units = it->data().units();
                            out.sequence = it->data().seq();
                            out.timestamp_ms = 0;
                            if (cb) cb(out);
                        }
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Subscriber error: " << e.what() << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    ~SubscriberImpl(){
        running.store(false);
        if (thr.joinable()) thr.join();
    }
};

SubscriberImpl* create_subscriber_impl() {
    try {
        return new SubscriberImpl();
    } catch (const std::exception& e) {
        std::cerr << "Failed to create subscriber: " << e.what() << std::endl;
        return nullptr;
    }
}

void destroy_subscriber_impl(SubscriberImpl* p) {
    delete p;
}

void subscriber_set_callback(SubscriberImpl* p, std::function<void(const TelemetryMessage&)> cb) {
    if (p) {
        p->cb = std::move(cb);
    }
}

} // namespace impl
} // namespace dds_io
