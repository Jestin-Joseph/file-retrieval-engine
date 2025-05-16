// include/DualQueue.hpp
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class DualQueue {
public:
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        cv_.notify_one();
    }

    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            if (done_) return false;
            cv_.wait(lock);
        }
        item = queue_.front();
        queue_.pop();
        return true;
    }

    void done() {
        std::unique_lock<std::mutex> lock(mutex_);
        done_ = true;
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool done_ = false;
};