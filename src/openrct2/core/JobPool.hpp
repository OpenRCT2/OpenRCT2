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

    std::vector<std::thread> _threads;
    std::deque<TaskData_t> _pending;
    std::deque<TaskData_t> _completed;
    std::condition_variable _condPending;
    std::condition_variable _condComplete;
    std::atomic<size_t> _processing;
    std::atomic_bool _shouldStop;
    std::mutex _mutex;

    typedef std::unique_lock<std::mutex> unique_lock;

public:
    JobPool();
    ~JobPool();

    void addTask(std::function<void()> workFn,
                 std::function<void()> completionFn);

    void addTask(std::function<void()> workFn);

    void join();

private:
    void processQueue();
};
