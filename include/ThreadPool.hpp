#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <utility>


class ThreadPool
{
public:
    explicit ThreadPool(unsigned threadAmount = std::thread::hardware_concurrency())
        : threadAmount{threadAmount}
    {
        createThreads();
    }

    ~ThreadPool()
    {
        waitForTasks();
        destroyThreads();
    }

    size_t getTasksAmount() const
    {
        return tasksAmount;
    }

    auto getThreadAmount() const
    {
        return threadAmount;
    }

    template <typename F>
    void pushTask(F&& task)
    {
        {
            std::unique_lock lock(tasksMutex);
            tasks.push(std::function<void()>(std::forward<F>(task)));
        }
        ++tasksAmount;
        taskAvailable.notify_one();
    }

    void waitForTasks()
    {
        waiting = true;
        std::unique_lock lock(tasksMutex);
        taskDone.wait(lock, [this] { return tasksAmount == 0; });
        waiting = false;
    }

private:
    void createThreads()
    {
        running = true;
        for (auto i = 0u; i < threadAmount; ++i)
            threads.push_back(std::thread([this]{ worker(); }));
    }

    void destroyThreads()
    {
        running = false;
        taskAvailable.notify_all();
        for (auto& thread : threads)
            thread.join();
    }

    void worker()
    {
        while (running)
        {
            std::function<void()> task;
            std::unique_lock lock(tasksMutex);
            taskAvailable.wait(lock, [&] { return !tasks.empty() || !running; });
            if (running)
            {
                task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                task();
                --tasksAmount;
                if (waiting)
                    taskDone.notify_one();
            }
        }
    }

    std::atomic<bool> running = false;
    std::condition_variable taskAvailable = {};
    std::condition_variable taskDone = {};
    std::queue<std::function<void()>> tasks = {};
    std::atomic<size_t> tasksAmount = 0;
    mutable std::mutex tasksMutex = {};
    unsigned threadAmount = 0;
    std::vector<std::thread> threads;
    std::atomic<bool> waiting = false;
};
