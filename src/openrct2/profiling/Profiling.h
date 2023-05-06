/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include "ProfilingMacros.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace OpenRCT2::Profiling
{
    void Enable();
    void Disable();
    bool IsEnabled();

    struct Function
    {
        virtual ~Function() = default;

        // This returns the full function prototype not just the name.
        virtual const char* GetName() const noexcept = 0;

        // Total calls made to the function.
        virtual uint64_t GetCallCount() const noexcept = 0;

        // Returns a small window of the most recent times.
        virtual std::vector<double> GetTimeSamples() const = 0;

        // Returns all time accumulated in microseconds.
        virtual double GetTotalTime() const = 0;

        // Returns the min. time in microseconds.
        virtual double GetMinTime() const = 0;

        // Returns the max. time in microseconds.
        virtual double GetMaxTime() const = 0;

        // Returns a list of all functions that called this function.
        virtual std::vector<Function*> GetParents() const = 0;

        // Returns a list of function this function is calling.
        virtual std::vector<Function*> GetChildren() const = 0;
    };

    namespace Detail
    {
        static constexpr auto MaxSamplesSize = 1024;
        static constexpr auto MaxNameSize = 250;

        std::vector<Function*>& GetRegistry();

        struct FunctionInternal : Function
        {
            FunctionInternal()
            {
                GetRegistry().push_back(this);
            }

            virtual ~FunctionInternal() = default;

            // Functions marked MT require locking, this is per function basis.
            mutable std::mutex Mutex;

            std::array<char, MaxNameSize> Name{};

            // Call count of function.
            std::atomic<uint64_t> CallCount{};

            // Function times in microseconds.
            std::array<double, MaxSamplesSize> Samples{};

            // Used internally to write into Samples without a lock.
            std::atomic<size_t> SampleIterator{};

            double MinTimeUs{};

            double MaxTimeUs{};

            double TotalTimeUs{};

            // Functions that called us.
            std::unordered_set<Function*> Parents;

            // Functions that this function called.
            std::unordered_set<Function*> Children;

            uint64_t GetCallCount() const noexcept override
            {
                return CallCount.load();
            }

            std::vector<double> GetTimeSamples() const override
            {
                const auto numSamples = std::min(SampleIterator.load(), Samples.size());
                return { Samples.begin(), Samples.begin() + numSamples };
            }

            std::vector<Function*> GetParents() const override
            {
                std::scoped_lock lock(Mutex);
                return { Parents.begin(), Parents.end() };
            }

            std::vector<Function*> GetChildren() const override
            {
                std::scoped_lock lock(Mutex);
                return { Children.begin(), Children.end() };
            }

            double GetTotalTime() const override
            {
                std::scoped_lock lock(Mutex);
                return TotalTimeUs;
            }

            double GetMinTime() const override
            {
                std::scoped_lock lock(Mutex);
                return MinTimeUs;
            }

            double GetMaxTime() const override
            {
                std::scoped_lock lock(Mutex);
                return MaxTimeUs;
            }
        };

        template<typename TName> struct FunctionWrapper : FunctionInternal
        {
            const char* GetName() const noexcept override
            {
                return TName::Str();
            }
        };

        // Wrapper to avoid static initialization inside the function.
        // This avoids the compiler generating thread-safe initialization
        // by making a unique type per function which hosts a global using
        // the inline keyword for the variable (C++17).
        template<typename TName> struct Storage
        {
            static inline FunctionWrapper<TName> Data;
        };

        void FunctionEnter(Function& func);
        void FunctionExit(Function& func);

    } // namespace Detail

    template<typename T> class ScopedProfiling
    {
        bool _enabled;
        T& _func;

    public:
        ScopedProfiling(T& func)
            : _enabled{ IsEnabled() }
            , _func(func)
        {
            if (_enabled)
            {
                Detail::FunctionEnter(_func);
            }
        }
        ~ScopedProfiling()
        {
            if (!_enabled)
                return;
            Detail::FunctionExit(_func);
        }
    };

    // Clears all the current data of each function.
    void ResetData();

    // Returns all functions.
    const std::vector<Function*>& GetData();

    bool ExportCSV(const std::string& filePath);

} // namespace OpenRCT2::Profiling
