/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace OpenRCT2::Audio
{
    // Kaiser-windowed sinc resampler
    class SincResampler
    {
    public:
        static constexpr int kHalfTaps = 12;
        static constexpr int kTaps = kHalfTaps * 2;
        static constexpr int kNumPhases = 512;
        static constexpr int kPhaseDensity = 512;
        static constexpr int kWing = kHalfTaps * kPhaseDensity;
        static constexpr double kKaiserBeta = 8.6;
        static constexpr double kMinFilterScale = 0.125;

        static const SincResampler& get()
        {
            static const SincResampler instance;
            return instance;
        }

        void mixMono(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR) const
        {
            if (step <= 1.0)
                mixPoly<1>(pcm, len, looping, step, position, out, frames, gainL, gainR);
            else
                mixStretched<1>(pcm, len, looping, step, position, out, frames, gainL, gainR);
        }

        void mixStereo(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR) const
        {
            if (step <= 1.0)
                mixPoly<2>(pcm, len, looping, step, position, out, frames, gainL, gainR);
            else
                mixStretched<2>(pcm, len, looping, step, position, out, frames, gainL, gainR);
        }

        [[nodiscard]] float sampleMono(
            const float* pcm, uint64_t len, bool looping, double position, double dhb, float filterScale) const
        {
            auto idx = static_cast<int64_t>(position);
            double frac = position - static_cast<double>(idx);
            float acc = 0.0f;

            double phase = frac * dhb;
            int64_t i = idx;
            while (phase < kWing)
            {
                acc += fetchMono(pcm, len, i, looping) * tap(phase);
                phase += dhb;
                i--;
            }

            phase = (1.0 - frac) * dhb;
            i = idx + 1;
            while (phase < kWing)
            {
                acc += fetchMono(pcm, len, i, looping) * tap(phase);
                phase += dhb;
                i++;
            }

            return acc * filterScale;
        }

        void sampleStereo(
            const float* pcm, uint64_t len, bool looping, double position, double dhb, float filterScale, float& outL,
            float& outR) const
        {
            auto idx = static_cast<int64_t>(position);
            double frac = position - static_cast<double>(idx);
            float accL = 0.0f;
            float accR = 0.0f;

            double phase = frac * dhb;
            int64_t i = idx;
            while (phase < kWing)
            {
                float c = tap(phase);
                int64_t s = wrapIndex(i, len, looping);
                if (s >= 0)
                {
                    accL += pcm[s * 2] * c;
                    accR += pcm[s * 2 + 1] * c;
                }
                phase += dhb;
                i--;
            }

            phase = (1.0 - frac) * dhb;
            i = idx + 1;
            while (phase < kWing)
            {
                float c = tap(phase);
                int64_t s = wrapIndex(i, len, looping);
                if (s >= 0)
                {
                    accL += pcm[s * 2] * c;
                    accR += pcm[s * 2 + 1] * c;
                }
                phase += dhb;
                i++;
            }

            outL = accL * filterScale;
            outR = accR * filterScale;
        }

        struct Step
        {
            double step;
            double dhb;
            float filterScale;
            int wingSamples;
        };

        [[nodiscard]] static Step makeStep(double rateRatio)
        {
            Step s{};
            s.step = rateRatio;
            float scale = rateRatio > 1.0 ? static_cast<float>(1.0 / rateRatio) : 1.0f;
            if (scale < static_cast<float>(kMinFilterScale))
                scale = static_cast<float>(kMinFilterScale);
            s.filterScale = scale;
            s.dhb = static_cast<double>(kPhaseDensity) * static_cast<double>(scale);
            s.wingSamples = static_cast<int>(std::ceil(static_cast<double>(kWing) / s.dhb)) + 1;
            return s;
        }

    private:
        SincResampler()
        {
            const double i0Beta = besselI0(kKaiserBeta);

            for (int i = 0; i <= kWing; i++)
            {
                double t = static_cast<double>(i) / kPhaseDensity;
                _proto[i] = static_cast<float>(kernel(t, i0Beta));
            }
            double protoScale = 1.0 / dcGainMidpoint();
            for (int i = 0; i <= kWing; i++)
                _proto[i] = static_cast<float>(_proto[i] * protoScale);
            for (int i = 0; i < kWing; i++)
                _delta[i] = _proto[i + 1] - _proto[i];
            _delta[kWing] = 0.0f;

            for (int p = 0; p <= kNumPhases; p++)
            {
                double frac = static_cast<double>(p) / kNumPhases;
                double sum = 0.0;
                for (int j = 0; j < kTaps; j++)
                {
                    double dist = frac - static_cast<double>(j - (kHalfTaps - 1));
                    double c = kernel(std::abs(dist), i0Beta);
                    _bank[p * kTaps + j] = static_cast<float>(c);
                    sum += c;
                }
                double inv = sum != 0.0 ? 1.0 / sum : 1.0;
                for (int j = 0; j < kTaps; j++)
                    _bank[p * kTaps + j] = static_cast<float>(_bank[p * kTaps + j] * inv);
            }
        }

        [[nodiscard]] static double kernel(double t, double i0Beta)
        {
            double sinc = (t == 0.0) ? 1.0 : std::sin(kPi * t) / (kPi * t);
            double x = t / kHalfTaps;
            double window = (x >= 1.0) ? 0.0 : besselI0(kKaiserBeta * std::sqrt(1.0 - x * x)) / i0Beta;
            return sinc * window;
        }

        [[nodiscard]] static int polyIndex(double frac)
        {
            auto p = static_cast<int>(frac * kNumPhases + 0.5);
            if (p > kNumPhases)
                p = kNumPhases;
            return p;
        }

        [[nodiscard]] float tap(double phase) const
        {
            auto i = static_cast<int>(phase);
            auto frac = static_cast<float>(phase - static_cast<double>(i));
            return _proto[i] + frac * _delta[i];
        }

        [[nodiscard]] static int64_t wrapIndex(int64_t i, uint64_t len, bool looping)
        {
            if (i >= 0 && static_cast<uint64_t>(i) < len)
                return i;
            if (!looping)
                return -1;
            int64_t m = i % static_cast<int64_t>(len);
            if (m < 0)
                m += static_cast<int64_t>(len);
            return m;
        }

        [[nodiscard]] float fetchMono(const float* pcm, uint64_t len, int64_t i, bool looping) const
        {
            int64_t s = wrapIndex(i, len, looping);
            return s >= 0 ? pcm[s] : 0.0f;
        }

        void samplePolyMono(const float* pcm, uint64_t len, bool looping, int64_t idx, double frac, float& out) const
        {
            const float* row = &_bank[polyIndex(frac) * kTaps];
            float acc = 0.0f;
            for (int j = 0; j < kTaps; j++)
            {
                int64_t s = wrapIndex(idx - (kHalfTaps - 1) + j, len, looping);
                if (s >= 0)
                    acc += pcm[s] * row[j];
            }
            out = acc;
        }

        void samplePolyStereo(
            const float* pcm, uint64_t len, bool looping, int64_t idx, double frac, float& outL, float& outR) const
        {
            const float* row = &_bank[polyIndex(frac) * kTaps];
            float accL = 0.0f;
            float accR = 0.0f;
            for (int j = 0; j < kTaps; j++)
            {
                int64_t s = wrapIndex(idx - (kHalfTaps - 1) + j, len, looping);
                if (s >= 0)
                {
                    accL += pcm[s * 2] * row[j];
                    accR += pcm[s * 2 + 1] * row[j];
                }
            }
            outL = accL;
            outR = accR;
        }

        template<int Channels>
        void mixPoly(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR) const
        {
            double pos = position;
            for (size_t f = 0; f < frames;)
            {
                auto idx = static_cast<int64_t>(pos);

                // Interior fast path
                bool interior = idx >= kHalfTaps && (idx + kHalfTaps) < static_cast<int64_t>(len);
                if (interior)
                {
                    double frac = pos - static_cast<double>(idx);
                    const float* row = &_bank[polyIndex(frac) * kTaps];
                    if constexpr (Channels == 1)
                    {
                        const float* x = &pcm[idx - (kHalfTaps - 1)];
                        float acc = 0.0f;
                        for (int j = 0; j < kTaps; j++)
                            acc += x[j] * row[j];
                        out[f * 2] += acc * gainL;
                        out[f * 2 + 1] += acc * gainR;
                    }
                    else
                    {
                        const float* x = &pcm[(idx - (kHalfTaps - 1)) * 2];
                        float accL = 0.0f;
                        float accR = 0.0f;
                        for (int j = 0; j < kTaps; j++)
                        {
                            accL += x[j * 2] * row[j];
                            accR += x[j * 2 + 1] * row[j];
                        }
                        out[f * 2] += accL * gainL;
                        out[f * 2 + 1] += accR * gainR;
                    }

                    pos += step;
                    f++;
                    continue;
                }

                if (!looping && idx >= static_cast<int64_t>(len))
                    break;

                double frac = pos - static_cast<double>(idx);
                if constexpr (Channels == 1)
                {
                    float sample;
                    samplePolyMono(pcm, len, looping, idx, frac, sample);
                    out[f * 2] += sample * gainL;
                    out[f * 2 + 1] += sample * gainR;
                }
                else
                {
                    float l, r;
                    samplePolyStereo(pcm, len, looping, idx, frac, l, r);
                    out[f * 2] += l * gainL;
                    out[f * 2 + 1] += r * gainR;
                }

                pos += step;
                f++;

                if (looping && pos >= static_cast<double>(len))
                    pos = std::fmod(pos, static_cast<double>(len));
            }

            position = pos;
        }

        template<int Channels>
        void mixStretched(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR) const
        {
            Step s = makeStep(step);

            double pos = position;
            for (size_t f = 0; f < frames;)
            {
                auto idx = static_cast<int64_t>(pos);

                bool interior = idx >= s.wingSamples && (idx + s.wingSamples + 1) < static_cast<int64_t>(len);
                if (interior)
                {
                    double frac = pos - static_cast<double>(idx);
                    if constexpr (Channels == 1)
                    {
                        float acc = 0.0f;
                        double phase = frac * s.dhb;
                        int64_t i = idx;
                        while (phase < kWing)
                        {
                            acc += pcm[i] * tap(phase);
                            phase += s.dhb;
                            i--;
                        }
                        phase = (1.0 - frac) * s.dhb;
                        i = idx + 1;
                        while (phase < kWing)
                        {
                            acc += pcm[i] * tap(phase);
                            phase += s.dhb;
                            i++;
                        }
                        acc *= s.filterScale;
                        out[f * 2] += acc * gainL;
                        out[f * 2 + 1] += acc * gainR;
                    }
                    else
                    {
                        float accL = 0.0f;
                        float accR = 0.0f;
                        double phase = frac * s.dhb;
                        int64_t i = idx;
                        while (phase < kWing)
                        {
                            float c = tap(phase);
                            accL += pcm[i * 2] * c;
                            accR += pcm[i * 2 + 1] * c;
                            phase += s.dhb;
                            i--;
                        }
                        phase = (1.0 - frac) * s.dhb;
                        i = idx + 1;
                        while (phase < kWing)
                        {
                            float c = tap(phase);
                            accL += pcm[i * 2] * c;
                            accR += pcm[i * 2 + 1] * c;
                            phase += s.dhb;
                            i++;
                        }
                        out[f * 2] += accL * s.filterScale * gainL;
                        out[f * 2 + 1] += accR * s.filterScale * gainR;
                    }

                    pos += step;
                    f++;
                    continue;
                }

                if (!looping && idx >= static_cast<int64_t>(len))
                    break;

                if constexpr (Channels == 1)
                {
                    float sample = sampleMono(pcm, len, looping, pos, s.dhb, s.filterScale);
                    out[f * 2] += sample * gainL;
                    out[f * 2 + 1] += sample * gainR;
                }
                else
                {
                    float l, r;
                    sampleStereo(pcm, len, looping, pos, s.dhb, s.filterScale, l, r);
                    out[f * 2] += l * gainL;
                    out[f * 2 + 1] += r * gainR;
                }

                pos += step;
                f++;

                if (looping && pos >= static_cast<double>(len))
                    pos = std::fmod(pos, static_cast<double>(len));
            }

            position = pos;
        }

        [[nodiscard]] double dcGainMidpoint() const
        {
            double worst = 0.0;
            double best = 1e30;
            for (int p = 0; p < kPhaseDensity; p++)
            {
                double frac = static_cast<double>(p) / kPhaseDensity;
                double sum = 0.0;
                double phase = frac * kPhaseDensity;
                while (phase < kWing)
                {
                    sum += protoRaw(phase);
                    phase += kPhaseDensity;
                }
                phase = (1.0 - frac) * kPhaseDensity;
                while (phase < kWing)
                {
                    sum += protoRaw(phase);
                    phase += kPhaseDensity;
                }
                worst = std::max(worst, sum);
                best = std::min(best, sum);
            }
            return 0.5 * (worst + best);
        }

        [[nodiscard]] double protoRaw(double phase) const
        {
            auto i = static_cast<int>(phase);
            if (i >= kWing)
                return 0.0;
            double frac = phase - static_cast<double>(i);
            return _proto[i] + frac * (_proto[i + 1] - _proto[i]);
        }

        static double besselI0(double x)
        {
            double sum = 1.0;
            double term = 1.0;
            double xx = x * x * 0.25;
            for (int k = 1; k < 64; k++)
            {
                term *= xx / (static_cast<double>(k) * static_cast<double>(k));
                sum += term;
                if (term < sum * 1e-16)
                    break;
            }
            return sum;
        }

        static constexpr double kPi = 3.14159265358979323846;

        std::array<float, kWing + 1> _proto{};
        std::array<float, kWing + 1> _delta{};
        std::array<float, (kNumPhases + 1) * kTaps> _bank{};
    };

    // Cheap two-tap linear fallback thats used when high quality resampling is off
    struct LinearResampler
    {
        static void mixMono(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR)
        {
            mixBlock<1>(pcm, len, looping, step, position, out, frames, gainL, gainR);
        }

        static void mixStereo(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR)
        {
            mixBlock<2>(pcm, len, looping, step, position, out, frames, gainL, gainR);
        }

        [[nodiscard]] static float sampleMono(const float* pcm, uint64_t len, bool looping, double position)
        {
            auto idx = static_cast<int64_t>(position);
            auto frac = static_cast<float>(position - static_cast<double>(idx));
            int64_t i0 = wrap(idx, len, looping);
            int64_t i1 = wrap(idx + 1, len, looping);
            float s0 = i0 >= 0 ? pcm[i0] : 0.0f;
            float s1 = i1 >= 0 ? pcm[i1] : 0.0f;
            return s0 + (s1 - s0) * frac;
        }

        static void sampleStereo(const float* pcm, uint64_t len, bool looping, double position, float& outL, float& outR)
        {
            auto idx = static_cast<int64_t>(position);
            auto frac = static_cast<float>(position - static_cast<double>(idx));
            int64_t i0 = wrap(idx, len, looping);
            int64_t i1 = wrap(idx + 1, len, looping);
            float l0 = i0 >= 0 ? pcm[i0 * 2] : 0.0f;
            float l1 = i1 >= 0 ? pcm[i1 * 2] : 0.0f;
            float r0 = i0 >= 0 ? pcm[i0 * 2 + 1] : 0.0f;
            float r1 = i1 >= 0 ? pcm[i1 * 2 + 1] : 0.0f;
            outL = l0 + (l1 - l0) * frac;
            outR = r0 + (r1 - r0) * frac;
        }

    private:
        [[nodiscard]] static int64_t wrap(int64_t i, uint64_t len, bool looping)
        {
            if (i >= 0 && static_cast<uint64_t>(i) < len)
                return i;
            if (!looping)
                return -1;
            int64_t m = i % static_cast<int64_t>(len);
            if (m < 0)
                m += static_cast<int64_t>(len);
            return m;
        }

        template<int Channels>
        static void mixBlock(
            const float* pcm, uint64_t len, bool looping, double step, double& position, float* out, size_t frames, float gainL,
            float gainR)
        {
            double pos = position;
            for (size_t f = 0; f < frames; f++)
            {
                if (!looping && static_cast<int64_t>(pos) >= static_cast<int64_t>(len))
                    break;

                if constexpr (Channels == 1)
                {
                    float s = sampleMono(pcm, len, looping, pos);
                    out[f * 2] += s * gainL;
                    out[f * 2 + 1] += s * gainR;
                }
                else
                {
                    float l, r;
                    sampleStereo(pcm, len, looping, pos, l, r);
                    out[f * 2] += l * gainL;
                    out[f * 2 + 1] += r * gainR;
                }

                pos += step;

                if (looping && pos >= static_cast<double>(len))
                    pos = std::fmod(pos, static_cast<double>(len));
            }
            position = pos;
        }
    };

} // namespace OpenRCT2::Audio
