/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

class JobPool
{
private:
    struct TaskData
    {
        const std::function<void()> WorkFn;
        const std::function<void()> CompletionFn;

        TaskData(std::function<void()> workFn, std::function<void()> completionFn);
    };

    std::atomic_bool _shouldStop = { false };
    std::atomic<size_t> _processing = { 0 };
    std::vector<std::thread> _threads;
    std::deque<TaskData> _pending;
    std::deque<TaskData> _completed;
    std::condition_variable _condPending;
    std::condition_variable _condComplete;
    std::mutex _mutex;

    using unique_lock = std::unique_lock<std::mutex>;

public:
    JobPool(size_t maxThreads = 255);
    ~JobPool();

    void AddTask(std::function<void()> workFn, std::function<void()> completionFn = nullptr);
    void Join(std::function<void()> reportFn = nullptr);
    size_t CountPending();

private:
    void ProcessQueue();
};
