#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "TaskQueue.h"

using std::mutex;
using std::function;
using std::unique_lock;

void TaskQueue::addTask(const function<void()>& task) {
    unique_lock<mutex> lock(mutex_);
    queue_.push(task);
    condition_.notify_one();
}

function<void()> TaskQueue::getTask() {
    unique_lock<mutex> lock(mutex_);
    condition_.wait(lock, [this]() { return !queue_.empty() || stop_; });

    if(queue_.empty() || stop_)
        return nullptr;
    auto task = queue_.front();
    queue_.pop();
    return task;
}

void TaskQueue::stop(){
    {
        unique_lock<mutex> lock(mutex_);
        stop_ = true;
    }
    condition_.notify_all();
}

void workerThread(TaskQueue* taskQueue){
    while(true){
        auto task = taskQueue->getTask();
        if (task == nullptr)
            break;
        task();
    }
}