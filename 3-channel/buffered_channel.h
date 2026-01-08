#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <condition_variable>
#include <mutex>
#include <stdexcept>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : size_(size), closed_(false) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mutex_);
        can_send_.wait(lock, [this]
                          { return queue_.size() < this->size_ || closed_; });
        if (closed_) {
            throw std::runtime_error("Channel is closed :(");
        }
        queue_.push(std::move(value));
        can_receive_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);
        can_receive_.wait(lock, [this]
                             { return !(queue_.empty()) || closed_; });
        if (!queue_.empty()) {
            T value = std::move(queue_.front());
            queue_.pop();
            can_send_.notify_one();
            return {std::move(value), true};
        }
        else {
            return {T(), false};
        }
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mutex_);
        this->closed_ = true;
        can_receive_.notify_all();
        can_send_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable can_send_;
    std::condition_variable can_receive_;
    int size_;
    bool closed_;
};

#endif // BUFFERED_CHANNEL_H_