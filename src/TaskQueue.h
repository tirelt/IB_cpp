#pragma once
#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class TaskQueue {
    public:
        void addTask(const std::function<void()> task);
        std::function<void()> getTask();
        void stop();
    private:
        std::queue<std::function<void()>> queue_;
        std::mutex mutex_;
        std::condition_variable condition_;
        bool stop_ = false;
    };

void workerThread(TaskQueue* taskQueue);

#endif
