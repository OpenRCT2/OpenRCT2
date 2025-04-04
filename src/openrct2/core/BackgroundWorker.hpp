/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <vector>

namespace OpenRCT2
{
    class BackgroundWorker
    {
        class JobBase
        {
        public:
            virtual ~JobBase() = default;

            virtual void run() = 0;
            virtual void dispatch() = 0;

            void cancel()
            {
                _stopSource.request_stop();
                _valid.store(false);
            }

            bool isCompleted() const
            {
                return _completed.load();
            }

            bool isValid() const
            {
                return _valid.load();
            }

        protected:
            std::atomic<bool> _valid{ true };
            std::atomic<bool> _completed{ false };
            std::stop_source _stopSource;
        };

        template<typename Result>
        class JobImpl : public JobBase
        {
        public:
            using WorkFunc = std::function<Result(std::stop_token)>;
            using CompletionFunc = std::function<void(Result)>;

            JobImpl(WorkFunc work, CompletionFunc completion)
                : _workFn(std::move(work))
                , _completionFn(std::move(completion))
            {
            }

            void run() override
            {
                if (!_stopSource.stop_requested())
                {
                    _result.emplace(_workFn(_stopSource.get_token()));
                    _completed.store(true);
                }
            }

            void dispatch() override
            {
                if (!_stopSource.stop_requested() && _completed.load() && _completionFn)
                {
                    _completionFn(std::move(_result.value()));
                    _valid.store(false);
                }
            }

        private:
            WorkFunc _workFn;
            CompletionFunc _completionFn;
            std::optional<Result> _result;
        };

        template<>
        class JobImpl<void> : public JobBase
        {
        public:
            using WorkFunc = std::function<void(std::stop_token)>;
            using CompletionFunc = std::function<void()>;

            JobImpl(WorkFunc work, CompletionFunc completion)
                : _workFn(std::move(work))
                , _completionFn(std::move(completion))
            {
            }

            void run() override
            {
                if (!_stopSource.stop_requested())
                {
                    _workFn(_stopSource.get_token());
                    _completed.store(true);
                }
            }

            void dispatch() override
            {
                if (!_stopSource.stop_requested() && _completed.load() && _completionFn)
                {
                    _completionFn();
                }
            }

        private:
            WorkFunc _workFn;
            CompletionFunc _completionFn;
        };

        template<typename TFn, typename TWantsToken>
        struct ResultType;

        template<typename TFn>
        struct ResultType<TFn, std::true_type>
        {
            using type = std::invoke_result_t<TFn, std::stop_token>;
        };

        template<typename TFn>
        struct ResultType<TFn, std::false_type>
        {
            using type = std::invoke_result_t<TFn>;
        };

    public:
        class Job
        {
        public:
            Job() = default;
            explicit Job(std::shared_ptr<JobBase> job)
                : _jobRef(job)
            {
            }

            bool isValid() const
            {
                auto job = _jobRef.lock();
                return job && job->isValid();
            }

            void cancel()
            {
                if (auto job = _jobRef.lock())
                {
                    job->cancel();
                }
            }

            bool isCompleted() const
            {
                auto job = _jobRef.lock();
                return job && job->isCompleted();
            }

        private:
            std::weak_ptr<JobBase> _jobRef;
        };

    public:
        BackgroundWorker()
        {
            const auto threadsAvailable = std::max(std::thread::hardware_concurrency(), 1U);

            // NOTE: We don't want to use all available threads, this is for background work only.
            // Adjust the number of threads if needed.
            const auto numThreads = std::min(threadsAvailable, 2U);

            for (auto i = 0U; i < numThreads; ++i)
            {
                _workThreads.emplace_back([this] { processJobs(); });
            }
        }

        ~BackgroundWorker()
        {
            {
                std::lock_guard lock(_mtx);
                _shouldStop = true;
            }
            _cv.notify_all();
        }

        template<typename WorkFunc, typename CompletionFunc>
        Job addJob(WorkFunc&& work, CompletionFunc&& completion)
        {
            static_assert(
                std::is_invocable_v<WorkFunc, std::stop_token> || std::is_invocable_v<WorkFunc>,
                "Work function must be callable with or without stop_token");

            constexpr bool expectsToken = std::is_invocable_v<WorkFunc, std::stop_token>;
            using Result = typename ResultType<WorkFunc, std::integral_constant<bool, expectsToken>>::type;

            const auto wrappedFunc = [wf = std::forward<WorkFunc>(work)](std::stop_token token) {
                if constexpr (expectsToken)
                {
                    return wf(token);
                }
                else
                {
                    return wf();
                }
            };

            if constexpr (std::is_void_v<Result>)
            {
                static_assert(std::is_invocable_v<CompletionFunc>, "Completion function must take no arguments for void work");
            }
            else
            {
                static_assert(std::is_invocable_v<CompletionFunc, Result>, "Completion function must match work result type");
            }

            auto job = std::make_shared<JobImpl<Result>>(std::move(wrappedFunc), std::forward<CompletionFunc>(completion));

            {
                std::lock_guard lock(_mtx);
                _jobs.push_back(job);
                _pending.push_back(job);
            }
            _cv.notify_one();

            return Job(job);
        }

        void dispatchCompleted()
        {
            std::vector<std::shared_ptr<JobBase>> completed;

            {
                std::lock_guard lock(_mtx);
                _jobs.erase(
                    std::remove_if(
                        _jobs.begin(), _jobs.end(),
                        [&](const auto& job) {
                            // Check if job is completed and valid
                            if (job->isCompleted() && job->isValid())
                            {
                                completed.push_back(std::move(job));
                                return true;
                            }
                            // Remove invalid jobs
                            if (!job->isValid())
                            {
                                return true;
                            }
                            // Keep the job in the list.
                            return false;
                        }),
                    _jobs.end());
            }

            for (auto& job : completed)
            {
                job->dispatch();
            }
        }

        bool empty() const
        {
            std::lock_guard lock(_mtx);
            return _jobs.empty();
        }

        size_t size() const
        {
            std::lock_guard lock(_mtx);
            return _jobs.size();
        }

    private:
        void processJobs()
        {
            while (true)
            {
                std::shared_ptr<JobBase> job;
                {
                    std::unique_lock lock(_mtx);
                    _cv.wait(lock, [this] { return !_pending.empty() || _shouldStop; });

                    if (_shouldStop)
                    {
                        break;
                    }

                    job = _pending.front();
                    _pending.pop_front();
                }
                job->run();
            }
        }

        mutable std::mutex _mtx;
        std::vector<std::jthread> _workThreads;
        std::condition_variable _cv;
        std::atomic<bool> _shouldStop{ false };
        std::vector<std::shared_ptr<JobBase>> _jobs;
        std::deque<std::shared_ptr<JobBase>> _pending;
    };

} // namespace OpenRCT2
