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

class JobPool
{
private:
    struct TaskData_t
    {
        const std::function<void()> workFn;
        const std::function<void()> completionFn;
    };

    std::atomic_bool _shouldStop = false;
    std::atomic<size_t> _processing = 0;
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
            if (th.joinable())
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

    void join()
    {
        while (true)
        {
            unique_lock lock(_mutex);
            _condComplete.wait(lock, [this]()
            {
                return (_pending.empty() && _processing == 0) ||
                    (_completed.empty() == false);
            });

            if (_completed.empty() &&
                _pending.empty() &&
                _processing == 0)
            {
                break;
            }

            if (!_completed.empty())
            {
                auto taskData = _completed.front();
                _completed.pop_front();

                lock.unlock();

                taskData.completionFn();

                lock.lock();
            }
        }
    }

private:
    void processQueue()
    {
        while (true)
        {
            unique_lock lock(_mutex);
            _condPending.wait(lock, [this]() {
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

                if (taskData.completionFn)
                {
                    _completed.push_back(taskData);
                }

                _processing--;
                _condComplete.notify_one();
            }
            if (_shouldStop)
                break;
        }
    }
};
