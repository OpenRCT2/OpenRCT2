// ****************************************************************************
// * This file is part of the HqMAME project. It is distributed under         *
// * GNU General Public License: http://www.gnu.org/licenses/gpl-3.0          *
// * Copyright (C) Zenju (zenju AT gmx DOT de) - All Rights Reserved          *
// *                                                                          *
// * Additionally and as a special exception, the author gives permission     *
// * to link the code of this program with the MAME library (or with modified *
// * versions of MAME that use the same license as MAME), and distribute      *
// * linked combinations including the two. You must obey the GNU General     *
// * Public License in all respects for all of the code used other than MAME. *
// * If you modify this file, you may extend this exception to your version   *
// * of the file, but you are not obligated to do so. If you do not wish to   *
// * do so, delete this exception statement from your version.                *
// ****************************************************************************

#include "xbrz.h"
#include <cassert>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iostream>

#define NDEBUG

void xbrz_scale_2x(const uint32_t *src, uint32_t *dst, int w, int h)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

	xbrz::ColorFormat cf = xbrz::ColorFormat::RGB;
	xbrz::scale(2, src, dst, w, h, cf);

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

namespace
{
template <uint32_t N> inline
unsigned char getByte(uint32_t val) { return static_cast<unsigned char>((val >> (8 * N)) & 0xff); }

inline unsigned char getAlpha(uint32_t pix) { return getByte<3>(pix); }
inline unsigned char getRed  (uint32_t pix) { return getByte<2>(pix); }
inline unsigned char getGreen(uint32_t pix) { return getByte<1>(pix); }
inline unsigned char getBlue (uint32_t pix) { return getByte<0>(pix); }

inline uint32_t makePixel(                 unsigned char r, unsigned char g, unsigned char b) { return             (r << 16) | (g << 8) | b; }
inline uint32_t makePixel(unsigned char a, unsigned char r, unsigned char g, unsigned char b) { return (a << 24) | (r << 16) | (g << 8) | b; }


template <unsigned int M, unsigned int N> inline
uint32_t gradientRGB(uint32_t pixFront, uint32_t pixBack) //blend front color with opacity M / N over opaque background: http://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending
{
    static_assert(0 < M && M < N && N <= 1000, "");

    auto calcColor = [](unsigned char colFront, unsigned char colBack) -> unsigned char { return (colFront * M + colBack * (N - M)) / N; };

    return makePixel(calcColor(getRed  (pixFront), getRed  (pixBack)),
                     calcColor(getGreen(pixFront), getGreen(pixBack)),
                     calcColor(getBlue (pixFront), getBlue (pixBack)));
}


template <unsigned int M, unsigned int N> inline
uint32_t gradientARGB(uint32_t pixFront, uint32_t pixBack) //find intermediate color between two colors with alpha channels (=> NO alpha blending!!!)
{
    static_assert(0 < M && M < N && N <= 1000, "");

    const unsigned int weightFront = getAlpha(pixFront) * M;
    const unsigned int weightBack  = getAlpha(pixBack) * (N - M);
    const unsigned int weightSum   = weightFront + weightBack;
    if (weightSum == 0)
        return 0;

    auto calcColor = [=](unsigned char colFront, unsigned char colBack)
    {
        return static_cast<unsigned char>((colFront * weightFront + colBack * weightBack) / weightSum);
    };

    return makePixel(static_cast<unsigned char>(weightSum / N),
                     calcColor(getRed  (pixFront), getRed  (pixBack)),
                     calcColor(getGreen(pixFront), getGreen(pixBack)),
                     calcColor(getBlue (pixFront), getBlue (pixBack)));
}


//inline
//double fastSqrt(double n)
//{
//    __asm //speeds up xBRZ by about 9% compared to std::sqrt which internally uses the same assembler instructions but adds some "fluff"
//    {
//        fld n
//        fsqrt
//    }
//}
//


uint32_t*       byteAdvance(      uint32_t* ptr, int bytes) { return reinterpret_cast<      uint32_t*>(reinterpret_cast<      char*>(ptr) + bytes); }
const uint32_t* byteAdvance(const uint32_t* ptr, int bytes) { return reinterpret_cast<const uint32_t*>(reinterpret_cast<const char*>(ptr) + bytes); }


//fill block  with the given color
inline
void fillBlock(uint32_t* trg, int pitch, uint32_t col, int blockWidth, int blockHeight)
{
    //for (int y = 0; y < blockHeight; ++y, trg = byteAdvance(trg, pitch))
    //    std::fill(trg, trg + blockWidth, col);

    for (int y = 0; y < blockHeight; ++y, trg = byteAdvance(trg, pitch))
        for (int x = 0; x < blockWidth; ++x)
            trg[x] = col;
}

inline
void fillBlock(uint32_t* trg, int pitch, uint32_t col, int n) { fillBlock(trg, pitch, col, n, n); }


#ifdef _MSC_VER
    #define FORCE_INLINE __forceinline
#elif defined __GNUC__
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif


enum RotationDegree //clock-wise
{
    ROT_0,
    ROT_90,
    ROT_180,
    ROT_270
};

//calculate input matrix coordinates after rotation at compile time
template <RotationDegree rotDeg, size_t I, size_t J, size_t N>
struct MatrixRotation;

template <size_t I, size_t J, size_t N>
struct MatrixRotation<ROT_0, I, J, N>
{
    static const size_t I_old = I;
    static const size_t J_old = J;
};

template <RotationDegree rotDeg, size_t I, size_t J, size_t N> //(i, j) = (row, col) indices, N = size of (square) matrix
struct MatrixRotation
{
    static const size_t I_old = N - 1 - MatrixRotation<static_cast<RotationDegree>(rotDeg - 1), I, J, N>::J_old; //old coordinates before rotation!
    static const size_t J_old =         MatrixRotation<static_cast<RotationDegree>(rotDeg - 1), I, J, N>::I_old; //
};


template <size_t N, RotationDegree rotDeg>
class OutputMatrix
{
public:
    OutputMatrix(uint32_t* out, int outWidth) : //access matrix area, top-left at position "out" for image with given width
        out_(out),
        outWidth_(outWidth) {}

    template <size_t I, size_t J>
    uint32_t& ref() const
    {
        static const size_t I_old = MatrixRotation<rotDeg, I, J, N>::I_old;
        static const size_t J_old = MatrixRotation<rotDeg, I, J, N>::J_old;
        return *(out_ + J_old + I_old * outWidth_);
    }

private:
    uint32_t* out_;
    const int outWidth_;
};


template <class T> inline
T square(T value) { return value * value; }



inline
double distRGB(uint32_t pix1, uint32_t pix2)
{
    const double r_diff = static_cast<int>(getRed  (pix1)) - getRed  (pix2);
    const double g_diff = static_cast<int>(getGreen(pix1)) - getGreen(pix2);
    const double b_diff = static_cast<int>(getBlue (pix1)) - getBlue (pix2);

    //euklidean RGB distance
    return std::sqrt(square(r_diff) + square(g_diff) + square(b_diff));
}


inline
double distYCbCr(uint32_t pix1, uint32_t pix2, double lumaWeight)
{
    //http://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.601_conversion
    //YCbCr conversion is a matrix multiplication => take advantage of linearity by subtracting first!
    const int r_diff = static_cast<int>(getRed  (pix1)) - getRed  (pix2); //we may delay division by 255 to after matrix multiplication
    const int g_diff = static_cast<int>(getGreen(pix1)) - getGreen(pix2); //
    const int b_diff = static_cast<int>(getBlue (pix1)) - getBlue (pix2); //substraction for int is noticeable faster than for double!

    //const double k_b = 0.0722; //ITU-R BT.709 conversion
    //const double k_r = 0.2126; //
    const double k_b = 0.0593; //ITU-R BT.2020 conversion
    const double k_r = 0.2627; //
    const double k_g = 1 - k_b - k_r;

    const double scale_b = 0.5 / (1 - k_b);
    const double scale_r = 0.5 / (1 - k_r);

    const double y   = k_r * r_diff + k_g * g_diff + k_b * b_diff; //[!], analog YCbCr!
    const double c_b = scale_b * (b_diff - y);
    const double c_r = scale_r * (r_diff - y);

    //we skip division by 255 to have similar range like other distance functions
    return std::sqrt(square(lumaWeight * y) + square(c_b) + square(c_r));
}


struct DistYCbCrBuffer //30% perf boost compared to distYCbCr()!
{
public:
    static double dist(uint32_t pix1, uint32_t pix2)
    {
#if defined _MSC_VER && _MSC_VER < 1900
#error function scope static initialization is not yet thread-safe!
#endif
        static const DistYCbCrBuffer inst;
        return inst.distImpl(pix1, pix2);
    }

private:
    DistYCbCrBuffer() : buffer(256 * 256 * 256)
    {
        for (uint32_t i = 0; i < 256 * 256 * 256; ++i) //startup time: 114 ms on Intel Core i5 (four cores)
        {
            const int r_diff = getByte<2>(i) * 2 - 255;
            const int g_diff = getByte<1>(i) * 2 - 255;
            const int b_diff = getByte<0>(i) * 2 - 255;

            const double k_b = 0.0593; //ITU-R BT.2020 conversion
            const double k_r = 0.2627; //
            const double k_g = 1 - k_b - k_r;

            const double scale_b = 0.5 / (1 - k_b);
            const double scale_r = 0.5 / (1 - k_r);

            const double y   = k_r * r_diff + k_g * g_diff + k_b * b_diff; //[!], analog YCbCr!
            const double c_b = scale_b * (b_diff - y);
            const double c_r = scale_r * (r_diff - y);

            buffer[i] = static_cast<float>(std::sqrt(square(y) + square(c_b) + square(c_r)));
        }
    }

    double distImpl(uint32_t pix1, uint32_t pix2) const
    {
        //if (pix1 == pix2) -> 8% perf degradation!
        //    return 0;
        //if (pix1 > pix2)
        //	  std::swap(pix1, pix2); -> 30% perf degradation!!!

        const int r_diff = static_cast<int>(getRed  (pix1)) - getRed  (pix2);
        const int g_diff = static_cast<int>(getGreen(pix1)) - getGreen(pix2);
        const int b_diff = static_cast<int>(getBlue (pix1)) - getBlue (pix2);

        return buffer[(((r_diff + 255) / 2) << 16) | //slightly reduce precision (division by 2) to squeeze value into single byte
                      (((g_diff + 255) / 2) <<  8) |
                      (( b_diff + 255) / 2)];
    }

    std::vector<float> buffer; //consumes 64 MB memory; using double is only 2% faster, but takes 128 MB
};


enum BlendType
{
    BLEND_NONE = 0,
    BLEND_NORMAL,   //a normal indication to blend
    BLEND_DOMINANT, //a strong indication to blend
    //attention: BlendType must fit into the value range of 2 bit!!!
};

struct BlendResult
{
    BlendType
    /**/blend_f, blend_g,
    /**/blend_j, blend_k;
};


struct Kernel_4x4 //kernel for preprocessing step
{
    uint32_t
    /**/a, b, c, d,
    /**/e, f, g, h,
    /**/i, j, k, l,
    /**/m, n, o, p;
};

/*
input kernel area naming convention:
-----------------
| A | B | C | D |
----|---|---|---|
| E | F | G | H |   //evaluate the four corners between F, G, J, K
----|---|---|---|   //input pixel is at position F
| I | J | K | L |
----|---|---|---|
| M | N | O | P |
-----------------
*/
template <class ColorDistance>
FORCE_INLINE //detect blend direction
BlendResult preProcessCorners(const Kernel_4x4& ker, const xbrz::ScalerCfg& cfg) //result: F, G, J, K corners of "GradientType"
{
    BlendResult result = {};

    if ((ker.f == ker.g &&
         ker.j == ker.k) ||
        (ker.f == ker.j &&
         ker.g == ker.k))
        return result;

    auto dist = [&](uint32_t pix1, uint32_t pix2) { return ColorDistance::dist(pix1, pix2, cfg.luminanceWeight); };

    const int weight = 4;
    double jg = dist(ker.i, ker.f) + dist(ker.f, ker.c) + dist(ker.n, ker.k) + dist(ker.k, ker.h) + weight * dist(ker.j, ker.g);
    double fk = dist(ker.e, ker.j) + dist(ker.j, ker.o) + dist(ker.b, ker.g) + dist(ker.g, ker.l) + weight * dist(ker.f, ker.k);

    if (jg < fk) //test sample: 70% of values max(jg, fk) / min(jg, fk) are between 1.1 and 3.7 with median being 1.8
    {
        const bool dominantGradient = cfg.dominantDirectionThreshold * jg < fk;
        if (ker.f != ker.g && ker.f != ker.j)
            result.blend_f = dominantGradient ? BLEND_DOMINANT : BLEND_NORMAL;

        if (ker.k != ker.j && ker.k != ker.g)
            result.blend_k = dominantGradient ? BLEND_DOMINANT : BLEND_NORMAL;
    }
    else if (fk < jg)
    {
        const bool dominantGradient = cfg.dominantDirectionThreshold * fk < jg;
        if (ker.j != ker.f && ker.j != ker.k)
            result.blend_j = dominantGradient ? BLEND_DOMINANT : BLEND_NORMAL;

        if (ker.g != ker.f && ker.g != ker.k)
            result.blend_g = dominantGradient ? BLEND_DOMINANT : BLEND_NORMAL;
    }
    return result;
}

struct Kernel_3x3
{
    uint32_t
    /**/a,  b,  c,
    /**/d,  e,  f,
    /**/g,  h,  i;
};

#define DEF_GETTER(x) template <RotationDegree rotDeg> uint32_t inline get_##x(const Kernel_3x3& ker) { return ker.x; }
//we cannot and NEED NOT write "ker.##x" since ## concatenates preprocessor tokens but "." is not a token
DEF_GETTER(a) DEF_GETTER(b) DEF_GETTER(c)
DEF_GETTER(d) DEF_GETTER(e) DEF_GETTER(f)
DEF_GETTER(g) DEF_GETTER(h) DEF_GETTER(i)
#undef DEF_GETTER

#define DEF_GETTER(x, y) template <> inline uint32_t get_##x<ROT_90>(const Kernel_3x3& ker) { return ker.y; }
DEF_GETTER(a, g) DEF_GETTER(b, d) DEF_GETTER(c, a)
DEF_GETTER(d, h) DEF_GETTER(e, e) DEF_GETTER(f, b)
DEF_GETTER(g, i) DEF_GETTER(h, f) DEF_GETTER(i, c)
#undef DEF_GETTER

#define DEF_GETTER(x, y) template <> inline uint32_t get_##x<ROT_180>(const Kernel_3x3& ker) { return ker.y; }
DEF_GETTER(a, i) DEF_GETTER(b, h) DEF_GETTER(c, g)
DEF_GETTER(d, f) DEF_GETTER(e, e) DEF_GETTER(f, d)
DEF_GETTER(g, c) DEF_GETTER(h, b) DEF_GETTER(i, a)
#undef DEF_GETTER

#define DEF_GETTER(x, y) template <> inline uint32_t get_##x<ROT_270>(const Kernel_3x3& ker) { return ker.y; }
DEF_GETTER(a, c) DEF_GETTER(b, f) DEF_GETTER(c, i)
DEF_GETTER(d, b) DEF_GETTER(e, e) DEF_GETTER(f, h)
DEF_GETTER(g, a) DEF_GETTER(h, d) DEF_GETTER(i,	g)
#undef DEF_GETTER


//compress four blend types into a single byte
inline BlendType getTopL   (unsigned char b) { return static_cast<BlendType>(0x3 & b); }
inline BlendType getTopR   (unsigned char b) { return static_cast<BlendType>(0x3 & (b >> 2)); }
inline BlendType getBottomR(unsigned char b) { return static_cast<BlendType>(0x3 & (b >> 4)); }
inline BlendType getBottomL(unsigned char b) { return static_cast<BlendType>(0x3 & (b >> 6)); }

inline void setTopL   (unsigned char& b, BlendType bt) { b |= bt; } //buffer is assumed to be initialized before preprocessing!
inline void setTopR   (unsigned char& b, BlendType bt) { b |= (bt << 2); }
inline void setBottomR(unsigned char& b, BlendType bt) { b |= (bt << 4); }
inline void setBottomL(unsigned char& b, BlendType bt) { b |= (bt << 6); }

inline bool blendingNeeded(unsigned char b) { return b != 0; }

template <RotationDegree rotDeg> inline
unsigned char rotateBlendInfo(unsigned char b) { return b; }
template <> inline unsigned char rotateBlendInfo<ROT_90 >(unsigned char b) { return ((b << 2) | (b >> 6)) & 0xff; }
template <> inline unsigned char rotateBlendInfo<ROT_180>(unsigned char b) { return ((b << 4) | (b >> 4)) & 0xff; }
template <> inline unsigned char rotateBlendInfo<ROT_270>(unsigned char b) { return ((b << 6) | (b >> 2)) & 0xff; }


#ifndef NDEBUG
    int debugPixelX = -1;
    int debugPixelY = 12;
    __declspec(thread) bool breakIntoDebugger = false;
#endif


/*
input kernel area naming convention:
-------------
| A | B | C |
----|---|---|
| D | E | F | //input pixel is at position E
----|---|---|
| G | H | I |
-------------
*/
template <class Scaler, class ColorDistance, RotationDegree rotDeg>
FORCE_INLINE //perf: quite worth it!
void blendPixel(const Kernel_3x3& ker,
                uint32_t* target, int trgWidth,
                unsigned char blendInfo, //result of preprocessing all four corners of pixel "e"
                const xbrz::ScalerCfg& cfg)
{
#define a get_a<rotDeg>(ker)
#define b get_b<rotDeg>(ker)
#define c get_c<rotDeg>(ker)
#define d get_d<rotDeg>(ker)
#define e get_e<rotDeg>(ker)
#define f get_f<rotDeg>(ker)
#define g get_g<rotDeg>(ker)
#define h get_h<rotDeg>(ker)
#define i get_i<rotDeg>(ker)

#ifndef NDEBUG
    if (breakIntoDebugger)
        __debugbreak(); //__asm int 3;
#endif

    const unsigned char blend = rotateBlendInfo<rotDeg>(blendInfo);

    if (getBottomR(blend) >= BLEND_NORMAL)
    {
        auto eq   = [&](uint32_t pix1, uint32_t pix2) { return ColorDistance::dist(pix1, pix2, cfg.luminanceWeight) < cfg.equalColorTolerance; };
        auto dist = [&](uint32_t pix1, uint32_t pix2) { return ColorDistance::dist(pix1, pix2, cfg.luminanceWeight); };

        const bool doLineBlend = [&]() -> bool
        {
            if (getBottomR(blend) >= BLEND_DOMINANT)
                return true;

            //make sure there is no second blending in an adjacent rotation for this pixel: handles insular pixels, mario eyes
            if (getTopR(blend) != BLEND_NONE && !eq(e, g)) //but support double-blending for 90� corners
                return false;
            if (getBottomL(blend) != BLEND_NONE && !eq(e, c))
                return false;

            //no full blending for L-shapes; blend corner only (handles "mario mushroom eyes")
            if (!eq(e, i) && eq(g, h) && eq(h , i) && eq(i, f) && eq(f, c))
                return false;

            return true;
        }();

        const uint32_t px = dist(e, f) <= dist(e, h) ? f : h; //choose most similar color

        OutputMatrix<Scaler::scale, rotDeg> out(target, trgWidth);

        if (doLineBlend)
        {
            const double fg = dist(f, g); //test sample: 70% of values max(fg, hc) / min(fg, hc) are between 1.1 and 3.7 with median being 1.9
            const double hc = dist(h, c); //

            const bool haveShallowLine = cfg.steepDirectionThreshold * fg <= hc && e != g && d != g;
            const bool haveSteepLine   = cfg.steepDirectionThreshold * hc <= fg && e != c && b != c;

            if (haveShallowLine)
            {
                if (haveSteepLine)
                    Scaler::blendLineSteepAndShallow(px, out);
                else
                    Scaler::blendLineShallow(px, out);
            }
            else
            {
                if (haveSteepLine)
                    Scaler::blendLineSteep(px, out);
                else
                    Scaler::blendLineDiagonal(px,out);
            }
        }
        else
            Scaler::blendCorner(px, out);
    }

#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef h
#undef i
}


template <class Scaler, class ColorDistance> //scaler policy: see "Scaler2x" reference implementation
void scaleImage(const uint32_t* src, uint32_t* trg, int srcWidth, int srcHeight, const xbrz::ScalerCfg& cfg, int yFirst, int yLast)
{
    yFirst = std::max(yFirst, 0);
    yLast  = std::min(yLast, srcHeight);
    if (yFirst >= yLast || srcWidth <= 0)
        return;

    const int trgWidth = srcWidth * Scaler::scale;

    //"use" space at the end of the image as temporary buffer for "on the fly preprocessing": we even could use larger area of
    //"sizeof(uint32_t) * srcWidth * (yLast - yFirst)" bytes without risk of accidental overwriting before accessing
    const int bufferSize = srcWidth;
    unsigned char* preProcBuffer = reinterpret_cast<unsigned char*>(trg + yLast * Scaler::scale * trgWidth) - bufferSize;
    std::fill(preProcBuffer, preProcBuffer + bufferSize, 0);
    static_assert(BLEND_NONE == 0, "");

    //initialize preprocessing buffer for first row of current stripe: detect upper left and right corner blending
    //this cannot be optimized for adjacent processing stripes; we must not allow for a memory race condition!
    if (yFirst > 0)
    {
        const int y = yFirst - 1;

        const uint32_t* s_m1 = src + srcWidth * std::max(y - 1, 0);
        const uint32_t* s_0  = src + srcWidth * y; //center line
        const uint32_t* s_p1 = src + srcWidth * std::min(y + 1, srcHeight - 1);
        const uint32_t* s_p2 = src + srcWidth * std::min(y + 2, srcHeight - 1);

        for (int x = 0; x < srcWidth; ++x)
        {
            const int x_m1 = std::max(x - 1, 0);
            const int x_p1 = std::min(x + 1, srcWidth - 1);
            const int x_p2 = std::min(x + 2, srcWidth - 1);

            Kernel_4x4 ker = {}; //perf: initialization is negligible
            ker.a = s_m1[x_m1]; //read sequentially from memory as far as possible
            ker.b = s_m1[x];
            ker.c = s_m1[x_p1];
            ker.d = s_m1[x_p2];

            ker.e = s_0[x_m1];
            ker.f = s_0[x];
            ker.g = s_0[x_p1];
            ker.h = s_0[x_p2];

            ker.i = s_p1[x_m1];
            ker.j = s_p1[x];
            ker.k = s_p1[x_p1];
            ker.l = s_p1[x_p2];

            ker.m = s_p2[x_m1];
            ker.n = s_p2[x];
            ker.o = s_p2[x_p1];
            ker.p = s_p2[x_p2];

            const BlendResult res = preProcessCorners<ColorDistance>(ker, cfg);
            /*
            preprocessing blend result:
            ---------
            | F | G |   //evalute corner between F, G, J, K
            ----|---|   //input pixel is at position F
            | J | K |
            ---------
            */
            setTopR(preProcBuffer[x], res.blend_j);

            if (x + 1 < bufferSize)
                setTopL(preProcBuffer[x + 1], res.blend_k);
        }
    }
    //------------------------------------------------------------------------------------

    for (int y = yFirst; y < yLast; ++y)
    {
        uint32_t* out = trg + Scaler::scale * y * trgWidth; //consider MT "striped" access

        const uint32_t* s_m1 = src + srcWidth * std::max(y - 1, 0);
        const uint32_t* s_0  = src + srcWidth * y; //center line
        const uint32_t* s_p1 = src + srcWidth * std::min(y + 1, srcHeight - 1);
        const uint32_t* s_p2 = src + srcWidth * std::min(y + 2, srcHeight - 1);

        unsigned char blend_xy1 = 0; //corner blending for current (x, y + 1) position

        for (int x = 0; x < srcWidth; ++x, out += Scaler::scale)
        {
#ifndef NDEBUG
            breakIntoDebugger = debugPixelX == x && debugPixelY == y;
#endif
            //all those bounds checks have only insignificant impact on performance!
            const int x_m1 = std::max(x - 1, 0); //perf: prefer array indexing to additional pointers!
            const int x_p1 = std::min(x + 1, srcWidth - 1);
            const int x_p2 = std::min(x + 2, srcWidth - 1);

            Kernel_4x4 ker4 = {}; //perf: initialization is negligible

            ker4.a = s_m1[x_m1]; //read sequentially from memory as far as possible
            ker4.b = s_m1[x];
            ker4.c = s_m1[x_p1];
            ker4.d = s_m1[x_p2];

            ker4.e = s_0[x_m1];
            ker4.f = s_0[x];
            ker4.g = s_0[x_p1];
            ker4.h = s_0[x_p2];

            ker4.i = s_p1[x_m1];
            ker4.j = s_p1[x];
            ker4.k = s_p1[x_p1];
            ker4.l = s_p1[x_p2];

            ker4.m = s_p2[x_m1];
            ker4.n = s_p2[x];
            ker4.o = s_p2[x_p1];
            ker4.p = s_p2[x_p2];

            //evaluate the four corners on bottom-right of current pixel
            unsigned char blend_xy = 0; //for current (x, y) position
            {
                const BlendResult res = preProcessCorners<ColorDistance>(ker4, cfg);
                /*
                preprocessing blend result:
                ---------
                | F | G |   //evalute corner between F, G, J, K
                ----|---|   //current input pixel is at position F
                | J | K |
                ---------
                */
                blend_xy = preProcBuffer[x];
                setBottomR(blend_xy, res.blend_f); //all four corners of (x, y) have been determined at this point due to processing sequence!

                setTopR(blend_xy1, res.blend_j); //set 2nd known corner for (x, y + 1)
                preProcBuffer[x] = blend_xy1; //store on current buffer position for use on next row

                blend_xy1 = 0;
                setTopL(blend_xy1, res.blend_k); //set 1st known corner for (x + 1, y + 1) and buffer for use on next column

                if (x + 1 < bufferSize) //set 3rd known corner for (x + 1, y)
                    setBottomL(preProcBuffer[x + 1], res.blend_g);
            }

            //fill block of size scale * scale with the given color
            fillBlock(out, trgWidth * sizeof(uint32_t), ker4.f, Scaler::scale); //place *after* preprocessing step, to not overwrite the results while processing the the last pixel!

            //blend four corners of current pixel
            if (blendingNeeded(blend_xy)) //good 5% perf-improvement
            {
                Kernel_3x3 ker3 = {}; //perf: initialization is negligible

                ker3.a = ker4.a;
                ker3.b = ker4.b;
                ker3.c = ker4.c;

                ker3.d = ker4.e;
                ker3.e = ker4.f;
                ker3.f = ker4.g;

                ker3.g = ker4.i;
                ker3.h = ker4.j;
                ker3.i = ker4.k;

                blendPixel<Scaler, ColorDistance, ROT_0  >(ker3, out, trgWidth, blend_xy, cfg);
                blendPixel<Scaler, ColorDistance, ROT_90 >(ker3, out, trgWidth, blend_xy, cfg);
                blendPixel<Scaler, ColorDistance, ROT_180>(ker3, out, trgWidth, blend_xy, cfg);
                blendPixel<Scaler, ColorDistance, ROT_270>(ker3, out, trgWidth, blend_xy, cfg);
            }
        }
    }
}

//------------------------------------------------------------------------------------

template <class ColorGradient>
struct Scaler2x : public ColorGradient
{
    static const int scale = 2;

    template <unsigned int M, unsigned int N> //bring template function into scope for GCC
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront) { ColorGradient::template alphaGrad<M, N>(pixBack, pixFront); }


    template <class OutputMatrix>
    static void blendLineShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);
    }

    template <class OutputMatrix>
    static void blendLineSteep(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);
    }

    template <class OutputMatrix>
    static void blendLineSteepAndShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<0, 1>(), col);
        alphaGrad<5, 6>(out.template ref<1, 1>(), col); //[!] fixes 7/8 used in xBR
    }

    template <class OutputMatrix>
    static void blendLineDiagonal(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 2>(out.template ref<1, 1>(), col);
    }

    template <class OutputMatrix>
    static void blendCorner(uint32_t col, OutputMatrix& out)
    {
        //model a round corner
        alphaGrad<21, 100>(out.template ref<1, 1>(), col); //exact: 1 - pi/4 = 0.2146018366
    }
};


template <class ColorGradient>
struct Scaler3x : public ColorGradient
{
    static const int scale = 3;

    template <unsigned int M, unsigned int N> //bring template function into scope for GCC
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront) { ColorGradient::template alphaGrad<M, N>(pixBack, pixFront); }


    template <class OutputMatrix>
    static void blendLineShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 2, 2>(), col);

        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);
        out.template ref<scale - 1, 2>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteep(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<1, 4>(out.template ref<2, scale - 2>(), col);

        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);
        out.template ref<2, scale - 1>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteepAndShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<2, 0>(), col);
        alphaGrad<1, 4>(out.template ref<0, 2>(), col);
        alphaGrad<3, 4>(out.template ref<2, 1>(), col);
        alphaGrad<3, 4>(out.template ref<1, 2>(), col);
        out.template ref<2, 2>() = col;
    }

    template <class OutputMatrix>
    static void blendLineDiagonal(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 8>(out.template ref<1, 2>(), col); //conflict with other rotations for this odd scale
        alphaGrad<1, 8>(out.template ref<2, 1>(), col);
        alphaGrad<7, 8>(out.template ref<2, 2>(), col); //
    }

    template <class OutputMatrix>
    static void blendCorner(uint32_t col, OutputMatrix& out)
    {
        //model a round corner
        alphaGrad<45, 100>(out.template ref<2, 2>(), col); //exact: 0.4545939598
        //alphaGrad<7, 256>(out.template ref<2, 1>(), col); //0.02826017254 -> negligible + avoid conflicts with other rotations for this odd scale
        //alphaGrad<7, 256>(out.template ref<1, 2>(), col); //0.02826017254
    }
};


template <class ColorGradient>
struct Scaler4x : public ColorGradient
{
    static const int scale = 4;

    template <unsigned int M, unsigned int N> //bring template function into scope for GCC
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront) { ColorGradient::template alphaGrad<M, N>(pixBack, pixFront); }


    template <class OutputMatrix>
    static void blendLineShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 2, 2>(), col);

        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 2, 3>(), col);

        out.template ref<scale - 1, 2>() = col;
        out.template ref<scale - 1, 3>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteep(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<1, 4>(out.template ref<2, scale - 2>(), col);

        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);
        alphaGrad<3, 4>(out.template ref<3, scale - 2>(), col);

        out.template ref<2, scale - 1>() = col;
        out.template ref<3, scale - 1>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteepAndShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<3, 4>(out.template ref<3, 1>(), col);
        alphaGrad<3, 4>(out.template ref<1, 3>(), col);
        alphaGrad<1, 4>(out.template ref<3, 0>(), col);
        alphaGrad<1, 4>(out.template ref<0, 3>(), col);

        alphaGrad<1, 3>(out.template ref<2, 2>(), col); //[!] fixes 1/4 used in xBR

        out.template ref<3, 3>() = col;
        out.template ref<3, 2>() = col;
        out.template ref<2, 3>() = col;
    }

    template <class OutputMatrix>
    static void blendLineDiagonal(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 2>(out.template ref<scale - 1, scale / 2    >(), col);
        alphaGrad<1, 2>(out.template ref<scale - 2, scale / 2 + 1>(), col);
        out.template ref<scale - 1, scale - 1>() = col;
    }

    template <class OutputMatrix>
    static void blendCorner(uint32_t col, OutputMatrix& out)
    {
        //model a round corner
        alphaGrad<68, 100>(out.template ref<3, 3>(), col); //exact: 0.6848532563
        alphaGrad< 9, 100>(out.template ref<3, 2>(), col); //0.08677704501
        alphaGrad< 9, 100>(out.template ref<2, 3>(), col); //0.08677704501
    }
};


template <class ColorGradient>
struct Scaler5x : public ColorGradient
{
    static const int scale = 5;

    template <unsigned int M, unsigned int N> //bring template function into scope for GCC
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront) { ColorGradient::template alphaGrad<M, N>(pixBack, pixFront); }


    template <class OutputMatrix>
    static void blendLineShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 2, 2>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 3, 4>(), col);

        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 2, 3>(), col);

        out.template ref<scale - 1, 2>() = col;
        out.template ref<scale - 1, 3>() = col;
        out.template ref<scale - 1, 4>() = col;
        out.template ref<scale - 2, 4>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteep(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<1, 4>(out.template ref<2, scale - 2>(), col);
        alphaGrad<1, 4>(out.template ref<4, scale - 3>(), col);

        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);
        alphaGrad<3, 4>(out.template ref<3, scale - 2>(), col);

        out.template ref<2, scale - 1>() = col;
        out.template ref<3, scale - 1>() = col;
        out.template ref<4, scale - 1>() = col;
        out.template ref<4, scale - 2>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteepAndShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<1, 4>(out.template ref<2, scale - 2>(), col);
        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);

        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 2, 2>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);

        alphaGrad<2, 3>(out.template ref<3, 3>(), col);

        out.template ref<2, scale - 1>() = col;
        out.template ref<3, scale - 1>() = col;
        out.template ref<4, scale - 1>() = col;

        out.template ref<scale - 1, 2>() = col;
        out.template ref<scale - 1, 3>() = col;
    }

    template <class OutputMatrix>
    static void blendLineDiagonal(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 8>(out.template ref<scale - 1, scale / 2    >(), col); //conflict with other rotations for this odd scale
        alphaGrad<1, 8>(out.template ref<scale - 2, scale / 2 + 1>(), col);
        alphaGrad<1, 8>(out.template ref<scale - 3, scale / 2 + 2>(), col); //

        alphaGrad<7, 8>(out.template ref<4, 3>(), col);
        alphaGrad<7, 8>(out.template ref<3, 4>(), col);

        out.template ref<4, 4>() = col;
    }

    template <class OutputMatrix>
    static void blendCorner(uint32_t col, OutputMatrix& out)
    {
        //model a round corner
        alphaGrad<86, 100>(out.template ref<4, 4>(), col); //exact: 0.8631434088
        alphaGrad<23, 100>(out.template ref<4, 3>(), col); //0.2306749731
        alphaGrad<23, 100>(out.template ref<3, 4>(), col); //0.2306749731
        //alphaGrad<1, 64>(out.template ref<4, 2>(), col); //0.01676812367 -> negligible + avoid conflicts with other rotations for this odd scale
        //alphaGrad<1, 64>(out.template ref<2, 4>(), col); //0.01676812367
    }
};


template <class ColorGradient>
struct Scaler6x : public ColorGradient
{
    static const int scale = 6;

    template <unsigned int M, unsigned int N> //bring template function into scope for GCC
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront) { ColorGradient::template alphaGrad<M, N>(pixBack, pixFront); }


    template <class OutputMatrix>
    static void blendLineShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 2, 2>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 3, 4>(), col);

        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 2, 3>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 3, 5>(), col);

        out.template ref<scale - 1, 2>() = col;
        out.template ref<scale - 1, 3>() = col;
        out.template ref<scale - 1, 4>() = col;
        out.template ref<scale - 1, 5>() = col;

        out.template ref<scale - 2, 4>() = col;
        out.template ref<scale - 2, 5>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteep(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<1, 4>(out.template ref<2, scale - 2>(), col);
        alphaGrad<1, 4>(out.template ref<4, scale - 3>(), col);

        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);
        alphaGrad<3, 4>(out.template ref<3, scale - 2>(), col);
        alphaGrad<3, 4>(out.template ref<5, scale - 3>(), col);

        out.template ref<2, scale - 1>() = col;
        out.template ref<3, scale - 1>() = col;
        out.template ref<4, scale - 1>() = col;
        out.template ref<5, scale - 1>() = col;

        out.template ref<4, scale - 2>() = col;
        out.template ref<5, scale - 2>() = col;
    }

    template <class OutputMatrix>
    static void blendLineSteepAndShallow(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 4>(out.template ref<0, scale - 1>(), col);
        alphaGrad<1, 4>(out.template ref<2, scale - 2>(), col);
        alphaGrad<3, 4>(out.template ref<1, scale - 1>(), col);
        alphaGrad<3, 4>(out.template ref<3, scale - 2>(), col);

        alphaGrad<1, 4>(out.template ref<scale - 1, 0>(), col);
        alphaGrad<1, 4>(out.template ref<scale - 2, 2>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 1, 1>(), col);
        alphaGrad<3, 4>(out.template ref<scale - 2, 3>(), col);

        out.template ref<2, scale - 1>() = col;
        out.template ref<3, scale - 1>() = col;
        out.template ref<4, scale - 1>() = col;
        out.template ref<5, scale - 1>() = col;

        out.template ref<4, scale - 2>() = col;
        out.template ref<5, scale - 2>() = col;

        out.template ref<scale - 1, 2>() = col;
        out.template ref<scale - 1, 3>() = col;
    }

    template <class OutputMatrix>
    static void blendLineDiagonal(uint32_t col, OutputMatrix& out)
    {
        alphaGrad<1, 2>(out.template ref<scale - 1, scale / 2    >(), col);
        alphaGrad<1, 2>(out.template ref<scale - 2, scale / 2 + 1>(), col);
        alphaGrad<1, 2>(out.template ref<scale - 3, scale / 2 + 2>(), col);

        out.template ref<scale - 2, scale - 1>() = col;
        out.template ref<scale - 1, scale - 1>() = col;
        out.template ref<scale - 1, scale - 2>() = col;
    }

    template <class OutputMatrix>
    static void blendCorner(uint32_t col, OutputMatrix& out)
    {
        //model a round corner
        alphaGrad<97, 100>(out.template ref<5, 5>(), col); //exact: 0.9711013910
        alphaGrad<42, 100>(out.template ref<4, 5>(), col); //0.4236372243
        alphaGrad<42, 100>(out.template ref<5, 4>(), col); //0.4236372243
        alphaGrad< 6, 100>(out.template ref<5, 3>(), col); //0.05652034508
        alphaGrad< 6, 100>(out.template ref<3, 5>(), col); //0.05652034508
    }
};

//------------------------------------------------------------------------------------

struct ColorDistanceRGB
{
    static double dist(uint32_t pix1, uint32_t pix2, double luminanceWeight)
    {
        return DistYCbCrBuffer::dist(pix1, pix2);

        //if (pix1 == pix2) //about 4% perf boost
        //    return 0;
        //return distYCbCr(pix1, pix2, luminanceWeight);
    }
};

struct ColorDistanceARGB
{
    static double dist(uint32_t pix1, uint32_t pix2, double luminanceWeight)
    {
        const double a1 = getAlpha(pix1) / 255.0 ;
        const double a2 = getAlpha(pix2) / 255.0 ;
        /*
        Requirements for a color distance handling alpha channel: with a1, a2 in [0, 1]

        	1. if a1 = a2, distance should be: a1 * distYCbCr()
        	2. if a1 = 0,  distance should be: a2 * distYCbCr(black, white) = a2 * 255
        	3. if a1 = 1,  ??? maybe: 255 * (1 - a2) + a2 * distYCbCr()
        */

        //return std::min(a1, a2) * DistYCbCrBuffer::dist(pix1, pix2) + 255 * abs(a1 - a2);
        //=> following code is 15% faster:
        const double d = DistYCbCrBuffer::dist(pix1, pix2);
        if (a1 < a2)
            return a1 * d + 255 * (a2 - a1);
        else
            return a2 * d + 255 * (a1 - a2);

        //alternative? return std::sqrt(a1 * a2 * square(DistYCbCrBuffer::dist(pix1, pix2)) + square(255 * (a1 - a2)));
    }
};


struct ColorGradientRGB
{
    template <unsigned int M, unsigned int N>
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront)
    {
        pixBack = gradientRGB<M, N>(pixFront, pixBack);
    }
};

struct ColorGradientARGB
{
    template <unsigned int M, unsigned int N>
    static void alphaGrad(uint32_t& pixBack, uint32_t pixFront)
    {
        pixBack = gradientARGB<M, N>(pixFront, pixBack);
    }
};
}


void xbrz::scale(size_t factor, const uint32_t* src, uint32_t* trg, int srcWidth, int srcHeight, ColorFormat colFmt, const xbrz::ScalerCfg& cfg, int yFirst, int yLast)
{
    switch (colFmt)
    {
        case ColorFormat::ARGB:
            switch (factor)
            {
                case 2:
                    return scaleImage<Scaler2x<ColorGradientARGB>, ColorDistanceARGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 3:
                    return scaleImage<Scaler3x<ColorGradientARGB>, ColorDistanceARGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 4:
                    return scaleImage<Scaler4x<ColorGradientARGB>, ColorDistanceARGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 5:
                    return scaleImage<Scaler5x<ColorGradientARGB>, ColorDistanceARGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 6:
                    return scaleImage<Scaler6x<ColorGradientARGB>, ColorDistanceARGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
            }
            break;

        case ColorFormat::RGB:
            switch (factor)
            {
                case 2:
                    return scaleImage<Scaler2x<ColorGradientRGB>, ColorDistanceRGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 3:
                    return scaleImage<Scaler3x<ColorGradientRGB>, ColorDistanceRGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 4:
                    return scaleImage<Scaler4x<ColorGradientRGB>, ColorDistanceRGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 5:
                    return scaleImage<Scaler5x<ColorGradientRGB>, ColorDistanceRGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
                case 6:
                    return scaleImage<Scaler6x<ColorGradientRGB>, ColorDistanceRGB>(src, trg, srcWidth, srcHeight, cfg, yFirst, yLast);
            }
            break;
    }
    assert(false);
}


bool xbrz::equalColorTest(uint32_t col1, uint32_t col2, ColorFormat colFmt, double luminanceWeight, double equalColorTolerance)
{
    switch (colFmt)
    {
        case ColorFormat::ARGB:
            return ColorDistanceARGB::dist(col1, col2, luminanceWeight) < equalColorTolerance;

        case ColorFormat::RGB:
            return ColorDistanceRGB::dist(col1, col2, luminanceWeight) < equalColorTolerance;
    }
    assert(false);
    return false;
}


void xbrz::nearestNeighborScale(const uint32_t* src, int srcWidth, int srcHeight, int srcPitch,
                                uint32_t* trg, int trgWidth, int trgHeight, int trgPitch,
                                SliceType st, int yFirst, int yLast)
{
    if (srcPitch < srcWidth * static_cast<int>(sizeof(uint32_t))  ||
        trgPitch < trgWidth * static_cast<int>(sizeof(uint32_t)))
    {
        assert(false);
        return;
    }

    switch (st)
    {
        case NN_SCALE_SLICE_SOURCE:
            //nearest-neighbor (going over source image - fast for upscaling, since source is read only once
            yFirst = std::max(yFirst, 0);
            yLast  = std::min(yLast, srcHeight);
            if (yFirst >= yLast || trgWidth <= 0 || trgHeight <= 0) return;

            for (int y = yFirst; y < yLast; ++y)
            {
                //mathematically: ySrc = floor(srcHeight * yTrg / trgHeight)
                // => search for integers in: [ySrc, ySrc + 1) * trgHeight / srcHeight

                //keep within for loop to support MT input slices!
                const int yTrg_first = ( y      * trgHeight + srcHeight - 1) / srcHeight; //=ceil(y * trgHeight / srcHeight)
                const int yTrg_last  = ((y + 1) * trgHeight + srcHeight - 1) / srcHeight; //=ceil(((y + 1) * trgHeight) / srcHeight)
                const int blockHeight = yTrg_last - yTrg_first;

                if (blockHeight > 0)
                {
                    const uint32_t* srcLine = byteAdvance(src, y * srcPitch);
                    uint32_t* trgLine  = byteAdvance(trg, yTrg_first * trgPitch);
                    int xTrg_first = 0;

                    for (int x = 0; x < srcWidth; ++x)
                    {
                        int xTrg_last = ((x + 1) * trgWidth + srcWidth - 1) / srcWidth;
                        const int blockWidth = xTrg_last - xTrg_first;
                        if (blockWidth > 0)
                        {
                            xTrg_first = xTrg_last;
                            fillBlock(trgLine, trgPitch, srcLine[x], blockWidth, blockHeight);
                            trgLine += blockWidth;
                        }
                    }
                }
            }
            break;

        case NN_SCALE_SLICE_TARGET:
            //nearest-neighbor (going over target image - slow for upscaling, since source is read multiple times missing out on cache! Fast for similar image sizes!)
            yFirst = std::max(yFirst, 0);
            yLast  = std::min(yLast, trgHeight);
            if (yFirst >= yLast || srcHeight <= 0 || srcWidth <= 0) return;

            for (int y = yFirst; y < yLast; ++y)
            {
                uint32_t* trgLine = byteAdvance(trg, y * trgPitch);
                const int ySrc = srcHeight * y / trgHeight;
                const uint32_t* srcLine = byteAdvance(src, ySrc * srcPitch);
                for (int x = 0; x < trgWidth; ++x)
                {
                    const int xSrc = srcWidth * x / trgWidth;
                    trgLine[x] = srcLine[xSrc];
                }
            }
            break;
    }
}
