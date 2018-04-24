#include "JobPool.hpp"

JobPool::JobPool()
    : _shouldStop(false),
    _processing(0)
{
    for (size_t n = 0; n < std::thread::hardware_concurrency(); n++)
    {
        _threads.emplace_back(&JobPool::processQueue, this);
    }
}

JobPool::~JobPool()
{
    {
        unique_lock lock(_mutex);
        _shouldStop = true;
        _condPending.notify_all();
    }

    for (auto&& th : _threads)
    {
        if(th.joinable())
            th.join();
    }
}

void JobPool::addTask(std::function<void()> workFn, std::function<void()> completionFn)
{
    unique_lock lock(_mutex);
    _pending.push_back(TaskData_t{workFn, completionFn});
    _condPending.notify_one();
}

void JobPool::addTask(std::function<void()> workFn)
{
    unique_lock lock(_mutex);
    _pending.push_back(TaskData_t{ workFn, nullptr });
    _condPending.notify_one();
}

void JobPool::join()
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

        auto taskData = _completed.front();
        _completed.pop_front();

        lock.unlock();

        taskData.completionFn();
    }
}

void JobPool::processQueue()
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
        if(_shouldStop)
            break;
    }
}


