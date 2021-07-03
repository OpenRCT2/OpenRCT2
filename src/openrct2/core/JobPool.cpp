/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "JobPool.h"

#include <algorithm>
#include <cassert>

JobPool::TaskData::TaskData(std::function<void()> workFn, std::function<void()> completionFn)
    : WorkFn(workFn)
    , CompletionFn(completionFn)
{
}

JobPool::JobPool(size_t maxThreads)
{
    maxThreads = std::min<size_t>(maxThreads, std::thread::hardware_concurrency());
    for (size_t n = 0; n < maxThreads; n++)
    {
        _threads.emplace_back(&JobPool::ProcessQueue, this);
    }
}

JobPool::~JobPool()
{
    {
        unique_lock lock(_mutex);
        _shouldStop = true;
        _condPending.notify_all();
    }

    for (auto& th : _threads)
    {
        assert(th.joinable() != false);
        th.join();
    }
}

void JobPool::AddTask(std::function<void()> workFn, std::function<void()> completionFn)
{
    unique_lock lock(_mutex);
    _pending.emplace_back(workFn, completionFn);
    _condPending.notify_one();
}

void JobPool::Join(std::function<void()> reportFn)
{
    unique_lock lock(_mutex);
    while (true)
    {
        // Wait for the queue to become empty or having completed tasks.
        _condComplete.wait(lock, [this]() { return (_pending.empty() && _processing == 0) || !_completed.empty(); });

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
        if (_completed.empty() && _pending.empty() && _processing == 0)
        {
            break;
        }
    }
}

size_t JobPool::CountPending()
{
    return _pending.size();
}

void JobPool::ProcessQueue()
{
    unique_lock lock(_mutex);
    do
    {
        // Wait for work or cancellation.
        _condPending.wait(lock, [this]() { return _shouldStop || !_pending.empty(); });

        if (!_pending.empty())
        {
            _processing++;

            auto taskData = _pending.front();
            _pending.pop_front();

            lock.unlock();

            taskData.WorkFn();

            lock.lock();

            _completed.push_back(std::move(taskData));

            _processing--;
            _condComplete.notify_one();
        }
    } while (!_shouldStop);
}
