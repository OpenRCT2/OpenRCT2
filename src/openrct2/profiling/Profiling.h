/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <vector>

namespace OpenRCT2::Profiling
{
    void enable();
    void disable();
    bool isEnabled();

    struct Function
    {
        virtual ~Function() = default;

        virtual const char* getName() const noexcept = 0;
        virtual uint64_t getCallCount() const noexcept = 0;
        virtual std::vector<double> getTimeSamples() const = 0;
        virtual double getTotalTime() const = 0;
        virtual double getMinTime() const = 0;
        virtual double getMaxTime() const = 0;

        double getAverageTime() const
        {
            const auto count = getCallCount();
            return count > 0 ? getTotalTime() / static_cast<double>(count) : 0.0;
        }

        virtual std::vector<Function*> getParents() const = 0;
        virtual std::vector<Function*> getChildren() const = 0;
    };

    namespace Detail
    {
        static constexpr size_t MaxSamplesSize = 1024;

        struct FunctionInternal;

        std::vector<Function*>& getRegistry();
        std::mutex& getRegistryMutex();
        void registerFunction(FunctionInternal* func);

        struct FunctionInternal : Function
        {
            FunctionInternal()
            {
                registerFunction(this);
            }

            virtual ~FunctionInternal() = default;

            // Mutex only for call graph access, not the hot path.
            mutable std::mutex Mutex;

            // Atomic and lock free
            std::atomic<uint64_t> CallCount{ 0 };
            std::atomic<uint64_t> TotalTimeNs{ 0 };
            std::atomic<uint64_t> MinTimeNs{ 0 };
            std::atomic<uint64_t> MaxTimeNs{ 0 };

            // Circular buffer for recent samples in nanoseconds
            std::array<std::atomic<uint64_t>, MaxSamplesSize> Samples{};
            std::atomic<size_t> SampleIndex{ 0 };

            // Call graph, protected by Mutex, but only accessed on new relationships
            std::vector<FunctionInternal*> Parents;
            std::vector<FunctionInternal*> Children;

            // Fast path cache, skip lock if we just linked this parent
            std::atomic<FunctionInternal*> LastParent{ nullptr };

            // Lock free min update
            void updateMin(uint64_t value)
            {
                uint64_t current = MinTimeNs.load(std::memory_order_relaxed);
                while (current == 0 || value < current)
                {
                    if (MinTimeNs.compare_exchange_weak(current, value, std::memory_order_relaxed))
                        break;
                }
            }

            // Lock free max update
            void updateMax(uint64_t value)
            {
                uint64_t current = MaxTimeNs.load(std::memory_order_relaxed);
                while (value > current)
                {
                    if (MaxTimeNs.compare_exchange_weak(current, value, std::memory_order_relaxed))
                        break;
                }
            }

            bool tryAddParent(FunctionInternal* parent);
            bool tryAddChild(FunctionInternal* child);

            uint64_t getCallCount() const noexcept override
            {
                return CallCount.load(std::memory_order_relaxed);
            }

            std::vector<double> getTimeSamples() const override;

            double getTotalTime() const override
            {
                return static_cast<double>(TotalTimeNs.load(std::memory_order_relaxed)) / 1000.0;
            }

            double getMinTime() const override
            {
                return static_cast<double>(MinTimeNs.load(std::memory_order_relaxed)) / 1000.0;
            }

            double getMaxTime() const override
            {
                return static_cast<double>(MaxTimeNs.load(std::memory_order_relaxed)) / 1000.0;
            }

            std::vector<Function*> getParents() const override
            {
                std::scoped_lock lock(Mutex);
                return { Parents.begin(), Parents.end() };
            }

            std::vector<Function*> getChildren() const override
            {
                std::scoped_lock lock(Mutex);
                return { Children.begin(), Children.end() };
            }
        };

        template<typename TName>
        struct FunctionWrapper final : FunctionInternal
        {
            const char* getName() const noexcept override
            {
                return TName::str();
            }
        };

        // Wrapper to avoid static initialization inside the function.
        // This avoids the compiler generating thread-safe initialization
        // by making a unique type per function which hosts a global using
        // the inline keyword for the variable (C++17).
        template<typename TName>
        struct Storage
        {
            static inline FunctionWrapper<TName> Data;
        };

        void functionEnter(FunctionInternal& func);
        void functionExit(FunctionInternal& func);

    } // namespace Detail

    // RAII wrapper for automatic function timing
    template<typename T>
    class ScopedProfiling
    {
        bool _enabled;
        T& _func;

    public:
        explicit ScopedProfiling(T& func)
            : _enabled(isEnabled())
            , _func(func)
        {
            if (_enabled)
            {
                Detail::functionEnter(_func);
            }
        }

        ~ScopedProfiling()
        {
            if (_enabled)
            {
                Detail::functionExit(_func);
            }
        }

        // Not copyable and not movable
        ScopedProfiling(const ScopedProfiling&) = delete;
        ScopedProfiling& operator=(const ScopedProfiling&) = delete;
        ScopedProfiling(ScopedProfiling&&) = delete;
        ScopedProfiling& operator=(ScopedProfiling&&) = delete;
    };

    void resetData();
    const std::vector<Function*>& getData();
    [[nodiscard]] bool exportData(const std::string& filePath);

} // namespace OpenRCT2::Profiling
