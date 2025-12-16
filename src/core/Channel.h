#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>

// Simple thread-safe channel (multiple producers, single or multiple consumers)
// Blocking pop with optional shutdown support.

template<typename T>
class Channel {
public:
    Channel() : closed(false) {}

    void push(T item) {
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::move(item));
        }
        cv.notify_one();
    }

    // Blocking pop. Returns std::nullopt if channel closed and empty.
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]{ return !q.empty() || closed; });
        if (q.empty()) return std::nullopt;
        T v = std::move(q.front()); q.pop();
        return v;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lk(m);
            closed = true;
        }
        cv.notify_all();
    }

    bool empty() {
        std::lock_guard<std::mutex> lk(m);
        return q.empty();
    }

private:
    std::mutex m;
    std::condition_variable cv;
    std::queue<T> q;
    bool closed;
};
