/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Profiling.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <stack>

namespace OpenRCT2::Profiling
{
    inline static bool _enabled = false;

    void Enable()
    {
        _enabled = true;
    }

    void Disable()
    {
        _enabled = false;
    }

    bool IsEnabled()
    {
        return _enabled;
    }

    namespace Detail
    {
        using Clock = std::chrono::high_resolution_clock;
        using Tp = Clock::time_point;

        struct FunctionEntry
        {
            FunctionInternal* Parent;
            FunctionInternal* Func;
            Tp EntryTime;

            FunctionEntry(FunctionInternal* parent, FunctionInternal* func, const Tp& entryTime)
                : Parent(parent)
                , Func(func)
                , EntryTime(entryTime)
            {
            }
        };

        static thread_local std::stack<FunctionEntry> _callStack;

        void FunctionEnter(Function& func)
        {
            const auto entryTime = Clock::now();

            auto& funcInternal = static_cast<FunctionInternal&>(func);
            funcInternal.CallCount++;

            FunctionInternal* parent = nullptr;

            if (!_callStack.empty())
                parent = _callStack.top().Func;

            _callStack.emplace(parent, &funcInternal, entryTime);
        }

        void FunctionExit(Function& func)
        {
            const auto exitTime = Clock::now();

            assert(!_callStack.empty());

            auto& stackEntry = _callStack.top();

            const auto deltaTime = exitTime - stackEntry.EntryTime;

            // Elapsed microseconds.
            const auto elapsedTimeUs = std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime).count() / 1000.0;

            auto* funcData = stackEntry.Func;

            // We don't need a lock for this, we only have a fixed window.
            const auto sampleEntryIdx = funcData->SampleIterator++ % funcData->Samples.size();
            funcData->Samples[sampleEntryIdx] = elapsedTimeUs;

            if (stackEntry.Parent)
            {
                std::scoped_lock lock(stackEntry.Parent->Mutex);
                stackEntry.Parent->Children.insert(funcData);
            }

            // This requires locking.
            {
                std::scoped_lock lock(funcData->Mutex);

                if (stackEntry.Parent)
                    funcData->Parents.insert(stackEntry.Parent);

                if (funcData->MinTimeUs == 0.0)
                    funcData->MinTimeUs = elapsedTimeUs;
                else
                    funcData->MinTimeUs = std::min(elapsedTimeUs, funcData->MinTimeUs);

                funcData->MaxTimeUs = std::max(elapsedTimeUs, funcData->MaxTimeUs);
                funcData->TotalTimeUs += elapsedTimeUs;
            }

            _callStack.pop();
        }

        std::vector<Function*>& GetRegistry()
        {
            static std::vector<Function*> Registry;
            return Registry;
        }

    } // namespace Detail

    const std::vector<Function*>& GetData()
    {
        return Detail::GetRegistry();
    }

    void ResetData()
    {
        for (auto* func : Detail::GetRegistry())
        {
            auto* funcInternal = static_cast<Detail::FunctionInternal*>(func);

            std::scoped_lock lock(funcInternal->Mutex);
            funcInternal->CallCount = 0;
            funcInternal->MinTimeUs = 0.0;
            funcInternal->MaxTimeUs = 0.0;
            funcInternal->SampleIterator = 0;
            funcInternal->Children.clear();
            funcInternal->Parents.clear();
        }
    }

    bool ExportCSV(const std::string& filePath)
    {
        std::ofstream out(filePath);
        if (!out.is_open())
            return false;

        out << "function_name;calls;min_microseconds;max_microseconds;average_microseconds\n";
        out << std::setprecision(12);

        const auto& data = GetData();
        for (auto* func : data)
        {
            out << "\"" << func->GetName() << "\""
                << ";";
            out << func->GetCallCount() << ";";
            out << func->GetMinTime() << ";";
            out << func->GetMaxTime() << ";";

            double avg = 0.0;
            if (func->GetCallCount() > 0)
                avg = func->GetTotalTime() / func->GetCallCount();

            out << avg << "\n";
        }

        return true;
    }

} // namespace OpenRCT2::Profiling
