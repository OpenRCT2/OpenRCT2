#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <functional>
#include <atomic>
#include <vector>
#include <cassert>

class JobPool
{
private:
    struct TaskData_t
    {
        const std::function<void()> workFn;
        const std::function<void()> completionFn;
    };

    std::atomic_bool _shouldStop;
    std::atomic<size_t> _processing;
    std::vector<std::thread> _threads;
    std::deque<TaskData_t> _pending;
    std::deque<TaskData_t> _completed;
    std::condition_variable _condPending;
    std::condition_variable _condComplete;
    std::mutex _mutex;

    typedef std::unique_lock<std::mutex> unique_lock;

public:
    JobPool()
    {
        _shouldStop = false;
        _processing = 0;
        for (size_t n = 0; n < std::thread::hardware_concurrency(); n++)
        {
            _threads.emplace_back(&JobPool::processQueue, this);
        }
    }

    ~JobPool()
    {
        {
            unique_lock lock(_mutex);
            _shouldStop = true;
            _condPending.notify_all();
        }

        for (auto&& th : _threads)
        {
            assert(th.joinable() != false);
            th.join();
        }
    }

    void addTask(std::function<void()> workFn,
        std::function<void()> completionFn)
    {
        {
            unique_lock lock(_mutex);
            _pending.push_back(TaskData_t{ workFn, completionFn });
            _condPending.notify_one();
        }
    }

    void addTask(std::function<void()> workFn)
    {
        return addTask(workFn, nullptr);
    }

    void join(std::function<void()> reportFn = nullptr)
    {
        unique_lock lock(_mutex);
        while (true)
        {
            // Wait for the queue to become empty or having completed tasks.
            _condComplete.wait(lock, [this]()
            {
                return (_pending.empty() && _processing == 0) ||
                        !_completed.empty();
            });

            // Dispatch all completion callbacks if there are any.
            while (!_completed.empty())
            {
                auto taskData = _completed.front();
                _completed.pop_front();

                if (taskData.completionFn)
                {
                    lock.unlock();

                    taskData.completionFn();

                    lock.lock();
                }
            }

            if (reportFn)
            {
                lock.unlock();

                reportFn();

                lock.lock();
            }

            // If everything is empty and no more work has to be done we can stop waiting.
            if (_completed.empty() &&
                _pending.empty() &&
                _processing == 0)
            {
                break;
            }
        }
    }

    size_t countPending()
    {
        return _pending.size();
    }

private:
    void processQueue()
    {
        unique_lock lock(_mutex);
        do
        {
            // Wait for work or cancelation.
            _condPending.wait(lock, [this]()
            {
                return _shouldStop || !_pending.empty();
            });

            if (!_pending.empty())
            {
                _processing++;

                auto taskData = _pending.front();
                _pending.pop_front();

                lock.unlock();

                taskData.workFn();

                lock.lock();

                _completed.push_back(taskData);

                _processing--;
                _condComplete.notify_one();
            }
        }
        while(!_shouldStop);
    }
};
