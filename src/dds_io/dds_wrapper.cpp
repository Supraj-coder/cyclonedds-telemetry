// Implementation of dds_io::Publisher and Subscriber using Cyclone DDS C++ API
#include "dds_wrapper.hpp"

namespace dds_io {

#ifdef HAVE_CYCLONEDDS
#include <dds/dds.hpp>
#include "Telemetry.hpp"
#include <thread>
#include <atomic>

using namespace org::eclipse::cyclonedds;

struct Publisher::Impl {
    dds::domain::DomainParticipant participant;
    dds::topic::Topic<Telemetry::Message> topic;
    dds::pub::Publisher publisher;
    dds::pub::DataWriter<Telemetry::Message> writer;

    Impl()
    : participant(dds::domain::DomainParticipant(domain::default_id()))
    , topic(participant, "/lab/telemetry")
    , publisher(participant)
    , writer(publisher, topic)
    {}
};

Publisher::Publisher() : impl(new Impl()) {}
Publisher::~Publisher(){ delete impl; }

void Publisher::publish(const TelemetryMessage &m) {
    Telemetry::Message msg;
    msg.sensor_id(m.sensor_id);
    msg.value(m.value);
    msg.units(m.units);
    msg.seq(m.sequence);
    impl->writer.write(msg);
}

// Subscriber implementation: runs a reader thread that takes samples and calls callback.
struct Subscriber::Impl {
    dds::domain::DomainParticipant participant;
    dds::topic::Topic<Telemetry::Message> topic;
    dds::sub::Subscriber subscriber;
    dds::sub::DataReader<Telemetry::Message> reader;
    std::function<void(const TelemetryMessage&)> cb;
    std::thread thr;
    std::atomic<bool> running{true};

    Impl() : participant(dds::domain::DomainParticipant(domain::default_id())), 
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
                            out.timestamp_ms = 0; // Not in IDL, set to 0
                            if (cb) cb(out);
                        }
                    }
                } catch (const std::exception &e) {
                    // swallow and continue
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    ~Impl(){
        running.store(false);
        if (thr.joinable()) thr.join();
    }
};

Subscriber::Subscriber(): impl(new Impl()){}
Subscriber::~Subscriber(){ delete impl; }

void Subscriber::set_callback(std::function<void(const TelemetryMessage&)> cb){ impl->cb = std::move(cb); }

#else

// Stub implementations when DDS is not enabled
struct Publisher::Impl {};
Publisher::Publisher(){ impl = new Impl(); }
Publisher::~Publisher(){ delete impl; }
void Publisher::publish(const TelemetryMessage &m){ (void)m; /* no-op */ }

struct Subscriber::Impl {};
Subscriber::Subscriber(){ impl = new Impl(); std::cout<<"dds_io: running stub subscriber (DDS not enabled)"<<std::endl; }
Subscriber::~Subscriber(){ delete impl; }
void Subscriber::set_callback(std::function<void(const TelemetryMessage&)> cb){ (void)cb; }

#endif

} // namespace dds_io
