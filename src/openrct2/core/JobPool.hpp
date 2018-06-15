/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <atomic>
#include <cassert>
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

        TaskData(std::function<void()> workFn, std::function<void()> completionFn)
            : WorkFn(workFn),
              CompletionFn(completionFn)
        {
        }
    };

    std::atomic_bool _shouldStop = { false };
    std::atomic<size_t> _processing = { 0 };
    std::vector<std::thread> _threads;
    std::deque<TaskData> _pending;
    std::deque<TaskData> _completed;
    std::condition_variable _condPending;
    std::condition_variable _condComplete;
    std::mutex _mutex;

    typedef std::unique_lock<std::mutex> unique_lock;

public:
    JobPool()
    {
        for (size_t n = 0; n < std::thread::hardware_concurrency(); n++)
        {
            _threads.emplace_back(&JobPool::ProcessQueue, this);
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

    void AddTask(std::function<void()> workFn, std::function<void()> completionFn)
    {
        unique_lock lock(_mutex);
        _pending.emplace_back(workFn, completionFn);
        _condPending.notify_one();
    }

    void AddTask(std::function<void()> workFn)
    {
        return AddTask(workFn, nullptr);
    }

    void Join(std::function<void()> reportFn = nullptr)
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

                if (taskData.CompletionFn)
                {
                    lock.unlock();

                    taskData.CompletionFn();

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

    size_t CountPending()
    {
        return _pending.size();
    }

private:
    void ProcessQueue()
    {
        unique_lock lock(_mutex);
        do
        {
            // Wait for work or cancelation.
            _condPending.wait(lock,
                [this]()
                {
                    return _shouldStop || !_pending.empty();
                });

            if (!_pending.empty())
            {
                _processing++;

                auto taskData = _pending.front();
                _pending.pop_front();

                lock.unlock();

                taskData.WorkFn();

                lock.lock();

                _completed.push_back(taskData);

                _processing--;
                _condComplete.notify_one();
            }
        }
        while(!_shouldStop);
    }
};
