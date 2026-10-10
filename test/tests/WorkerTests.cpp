/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <chrono>
#include <gtest/gtest.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/BackgroundWorker.hpp>
#include <openrct2/core/JobPool.h>
#include <thread>

using namespace OpenRCT2;

class WorkerTests : public testing::TestWithParam<bool>
{
    uint8_t _previousValue{};

protected:
    void SetUp() override
    {
        _previousValue = Config::Get().general.multiThreading.exchange(GetParam());
    }

    void TearDown() override
    {
        Config::Get().general.multiThreading = _previousValue;
    }

    void Drain(BackgroundWorker& worker)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (!worker.empty() && std::chrono::steady_clock::now() < deadline)
        {
            worker.dispatchCompleted();
            std::this_thread::yield();
        }
        EXPECT_TRUE(worker.empty());
    }
};

INSTANTIATE_TEST_SUITE_P(Multithreading, WorkerTests, testing::Bool());

TEST_P(WorkerTests, JobPoolPreservesCompletionDispatch)
{
    const auto caller = std::this_thread::get_id();
    JobPool pool(1);
    std::thread::id workThread;
    int completions = 0;
    int reports = 0;
    pool.AddTask(
        [&] { workThread = std::this_thread::get_id(); },
        [&] {
            EXPECT_EQ(std::this_thread::get_id(), caller);
            ++completions;
            // Completion callbacks must be able to submit more work without deadlocking.
            pool.AddTask([] {}, [&] { ++completions; });
        });
    EXPECT_EQ(completions, 0);
    if (!GetParam())
    {
        EXPECT_EQ(workThread, caller);
        EXPECT_FALSE(pool.IsBusy());
    }
    pool.Join([&] { ++reports; });
    EXPECT_EQ(workThread == caller, !GetParam());
    EXPECT_EQ(completions, 2);
    EXPECT_GT(reports, 0);
    EXPECT_FALSE(pool.IsBusy());
    pool.Join();
    EXPECT_EQ(completions, 2);
}

TEST_P(WorkerTests, BackgroundWorkerPreservesResultsAndDispatch)
{
    const auto caller = std::this_thread::get_id();
    BackgroundWorker worker;
    int completions = 0;
    auto job = worker.addJob(
        [] { return std::this_thread::get_id(); },
        [&](std::thread::id workThread) {
            EXPECT_EQ(workThread == caller, !GetParam());
            EXPECT_EQ(std::this_thread::get_id(), caller);
            ++completions;
            worker.addJob([] {}, [&] { ++completions; });
        });
    EXPECT_TRUE(job.isValid());
    EXPECT_EQ(worker.size(), 1u);
    EXPECT_EQ(completions, 0);
    Drain(worker);
    EXPECT_EQ(completions, 2);
    EXPECT_FALSE(job.isValid());
    worker.dispatchCompleted();
    EXPECT_EQ(completions, 2);
}

TEST_P(WorkerTests, BackgroundWorkerSupportsCancellationAndStopToken)
{
    BackgroundWorker worker;
    int completions = 0;
    auto job = worker.addJob([](std::atomic_bool&) { return 42; }, [&](int) { ++completions; });
    job.cancel();
    EXPECT_FALSE(job.isValid());
    Drain(worker);
    EXPECT_EQ(completions, 0);

    const auto caller = std::this_thread::get_id();
    std::thread::id workThread;
    worker.addJob(
        [&](std::atomic_bool& stop) {
            EXPECT_FALSE(stop.load());
            workThread = std::this_thread::get_id();
        },
        [&] {
            EXPECT_EQ(workThread == caller, !GetParam());
            ++completions;
        });
    if (!GetParam())
    {
        EXPECT_EQ(workThread, caller);
    }
    Drain(worker);
    EXPECT_EQ(completions, 1);
}
