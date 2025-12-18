// Implementation of dds_io::Publisher and Subscriber using Cyclone DDS C++ API
#include "dds_wrapper.hpp"
#include <thread>
#include <atomic>
// Forward declare the actual implementation that will be compiled separately
namespace dds_io {
namespace impl {
    struct PublisherImpl;
    struct SubscriberImpl;
    
    PublisherImpl* create_publisher_impl();
    void destroy_publisher_impl(PublisherImpl* p);
    void publisher_publish(PublisherImpl* p, const TelemetryMessage& m);
    
    SubscriberImpl* create_subscriber_impl();
    void destroy_subscriber_impl(SubscriberImpl* p);
    void subscriber_set_callback(SubscriberImpl* p, std::function<void(const TelemetryMessage&)> cb);
}
}

namespace dds_io {

struct Publisher::Impl {
    impl::PublisherImpl* real_impl;
};

Publisher::Publisher() {
    impl = new Impl{impl::create_publisher_impl()};
}

Publisher::~Publisher() {
    if (impl) {
        impl::destroy_publisher_impl(impl->real_impl);
        delete impl;
    }
}

void Publisher::publish(const TelemetryMessage &m) {
    impl::publisher_publish(impl->real_impl, m);
}

struct Subscriber::Impl {
    impl::SubscriberImpl* real_impl;
};

Subscriber::Subscriber() {
    impl = new Impl{impl::create_subscriber_impl()};
}

Subscriber::~Subscriber() {
    if (impl) {
        impl::destroy_subscriber_impl(impl->real_impl);
        delete impl;
    }
}

void Subscriber::set_callback(std::function<void(const TelemetryMessage&)> cb) {
    impl::subscriber_set_callback(impl->real_impl, std::move(cb));
}


} // namespace dds_io
