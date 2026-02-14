/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Profiling.h"

#include "../core/Json.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>

namespace OpenRCT2::Profiling
{
    // Global enable flag, atomic for thread safety
    static std::atomic<bool> _enabled{ false };

    void enable()
    {
        _enabled.store(true, std::memory_order_release);
    }

    void disable()
    {
        _enabled.store(false, std::memory_order_release);
    }

    bool isEnabled()
    {
        return _enabled.load(std::memory_order_acquire);
    }

    namespace Detail
    {
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = Clock::time_point;

        struct StackEntry
        {
            FunctionInternal* Parent;
            FunctionInternal* Func;
            TimePoint EntryTime;
        };

        static thread_local std::vector<StackEntry> _callStack;
        static std::vector<Function*> _registry;
        static std::mutex _registryMutex;

        std::vector<Function*>& getRegistry()
        {
            return _registry;
        }

        std::mutex& getRegistryMutex()
        {
            return _registryMutex;
        }

        void registerFunction(FunctionInternal* func)
        {
            std::scoped_lock lock(_registryMutex);
            _registry.push_back(func);
        }

        void functionEnter(FunctionInternal& func)
        {
            const auto entryTime = Clock::now();
            func.CallCount.fetch_add(1, std::memory_order_relaxed);

            FunctionInternal* parent = nullptr;
            if (!_callStack.empty())
            {
                parent = _callStack.back().Func;
            }

            _callStack.push_back({ parent, &func, entryTime });
        }

        bool FunctionInternal::tryAddParent(FunctionInternal* parent)
        {
            std::scoped_lock lock(Mutex);
            if (std::find(Parents.begin(), Parents.end(), parent) == Parents.end())
            {
                Parents.push_back(parent);
                return true;
            }
            return false;
        }

        bool FunctionInternal::tryAddChild(FunctionInternal* child)
        {
            std::scoped_lock lock(Mutex);
            if (std::find(Children.begin(), Children.end(), child) == Children.end())
            {
                Children.push_back(child);
                return true;
            }
            return false;
        }

        std::vector<double> FunctionInternal::getTimeSamples() const
        {
            const size_t totalSamples = SampleIndex.load(std::memory_order_relaxed);
            const size_t count = std::min(totalSamples, MaxSamplesSize);
            std::vector<double> result;
            result.reserve(count);

            const size_t startIdx = (totalSamples > MaxSamplesSize) ? (totalSamples % MaxSamplesSize) : 0;
            for (size_t i = 0; i < count; ++i)
            {
                const size_t idx = (startIdx + i) % MaxSamplesSize;
                result.push_back(static_cast<double>(Samples[idx].load(std::memory_order_relaxed)) / 1000.0);
            }
            return result;
        }

        void functionExit(FunctionInternal& func)
        {
            const auto exitTime = Clock::now();

            assert(!_callStack.empty() && "FunctionExit called without matching FunctionEnter");

            const auto& entry = _callStack.back();
            assert(entry.Func == &func && "FunctionExit called for wrong function");

            const auto elapsed = exitTime - entry.EntryTime;
            const auto elapsedNs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());

            // All timing updates are lock free
            func.TotalTimeNs.fetch_add(elapsedNs, std::memory_order_relaxed);
            func.updateMin(elapsedNs);
            func.updateMax(elapsedNs);

            const size_t idx = func.SampleIndex.fetch_add(1, std::memory_order_relaxed) % MaxSamplesSize;
            func.Samples[idx].store(elapsedNs, std::memory_order_relaxed);

            // Only take locks when discovering new relationships
            if (entry.Parent != nullptr)
            {
                // If this is the same parent as last time, skip lock
                FunctionInternal* lastParent = func.LastParent.load(std::memory_order_relaxed);
                if (lastParent != entry.Parent)
                {
                    // Potentially new relationship so take locks and check
                    if (func.tryAddParent(entry.Parent))
                    {
                        entry.Parent->tryAddChild(&func);
                    }
                    func.LastParent.store(entry.Parent, std::memory_order_relaxed);
                }
            }

            _callStack.pop_back();
        }

    } // namespace Detail

    const std::vector<Function*>& getData()
    {
        // eturns reference to static vector. Safe because functions are only registered during static initialization.
        return Detail::getRegistry();
    }

    void resetData()
    {
        std::scoped_lock lock(Detail::getRegistryMutex());

        for (auto* func : Detail::getRegistry())
        {
            auto* internal = static_cast<Detail::FunctionInternal*>(func);

            internal->CallCount.store(0, std::memory_order_relaxed);
            internal->TotalTimeNs.store(0, std::memory_order_relaxed);
            internal->MinTimeNs.store(0, std::memory_order_relaxed);
            internal->MaxTimeNs.store(0, std::memory_order_relaxed);
            internal->SampleIndex.store(0, std::memory_order_relaxed);
            internal->LastParent.store(nullptr, std::memory_order_relaxed);

            for (auto& sample : internal->Samples)
            {
                sample.store(0, std::memory_order_relaxed);
            }

            {
                std::scoped_lock funcLock(internal->Mutex);
                internal->Parents.clear();
                internal->Children.clear();
            }
        }
    }

    namespace
    {
        bool writeCSV(const std::string& filePath)
        {
            std::ofstream out(filePath);
            if (!out.is_open())
                return false;

            out << std::setprecision(12);
            out << "function_name,calls,min_microseconds,max_microseconds,average_microseconds,total_microseconds\n";

            std::scoped_lock lock(Detail::getRegistryMutex());

            for (const auto* func : Detail::getRegistry())
            {
                out << "\"" << func->getName() << "\",";
                out << func->getCallCount() << ",";
                out << func->getMinTime() << ",";
                out << func->getMaxTime() << ",";
                out << func->getAverageTime() << ",";
                out << func->getTotalTime() << "\n";
            }

            return true;
        }

        bool writeJSON(const std::string& filePath)
        {
            std::scoped_lock lock(Detail::getRegistryMutex());
            const auto& registry = Detail::getRegistry();

            json_t functions = json_t::array();

            for (size_t i = 0; i < registry.size(); ++i)
            {
                const auto* func = registry[i];

                json_t parentIndices = json_t::array();
                for (const auto* parent : func->getParents())
                {
                    auto it = std::find(registry.begin(), registry.end(), parent);
                    if (it != registry.end())
                        parentIndices.push_back(std::distance(registry.begin(), it));
                }

                json_t childIndices = json_t::array();
                for (const auto* child : func->getChildren())
                {
                    auto it = std::find(registry.begin(), registry.end(), child);
                    if (it != registry.end())
                        childIndices.push_back(std::distance(registry.begin(), it));
                }

                functions.push_back(
                    {
                        { "name", func->getName() },
                        { "callCount", func->getCallCount() },
                        { "minTime", func->getMinTime() },
                        { "maxTime", func->getMaxTime() },
                        { "avgTime", func->getAverageTime() },
                        { "totalTime", func->getTotalTime() },
                        { "parents", parentIndices },
                        { "children", childIndices },
                    });
            }

            json_t root = { { "functions", functions } };

            try
            {
                Json::WriteToFile(filePath, root);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        bool hasExtension(const std::string& path, const std::string& ext)
        {
            if (path.size() < ext.size())
                return false;

            // Case insensitive extension comparison
            auto pathEnd = path.substr(path.size() - ext.size());
            std::transform(pathEnd.begin(), pathEnd.end(), pathEnd.begin(), ::tolower);
            return pathEnd == ext;
        }
    } // namespace

    bool exportData(const std::string& filePath)
    {
        if (hasExtension(filePath, ".json"))
            return writeJSON(filePath);
        return writeCSV(filePath);
    }

} // namespace OpenRCT2::Profiling
