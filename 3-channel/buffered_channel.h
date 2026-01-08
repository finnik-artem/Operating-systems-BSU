#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <condition_variable>
#include <mutex>
#include <stdexcept>

template <class T>
class BufferedChannel
{
public:
    explicit BufferedChannel(int size) : size_(size), closed_(false) {}

    void Send(T value)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_can_send_.wait(lock, [this]
                          { return queue_.size() < this->size_ || closed_; });

        if (closed_)
        {
            throw std::runtime_error("channel closed");
        }

        queue_.push(std::move(value));

        cv_can_receive_.notify_one();
    }

    std::pair<T, bool> Recv()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_can_receive_.wait(lock, [this]
                             { return !(queue_.empty()) || closed_; });

        if (!queue_.empty())
        {
            T value = std::move(queue_.front());
            queue_.pop();
            cv_can_send_.notify_one();
            return {std::move(value), true};
        }
        else
        {
            return {T(), false};
        }
    }

    void Close()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        this->closed_ = true;
        cv_can_receive_.notify_all();
        cv_can_send_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    int size_;
    std::condition_variable cv_can_send_;
    std::condition_variable cv_can_receive_;
    bool closed_;
};

#endif // BUFFERED_CHANNEL_H_